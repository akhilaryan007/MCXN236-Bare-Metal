
# MCXN236 Bare-Metal LPUART Debug Stories

A log of two real hardware bugs I hit while implementing bare-metal LP_FLEXCOMM4 / LPUART polling on the **FRDM-MCXN236** board. No SDK drivers — just register-level code, a debugger, and a lot of patience.

---

## Bug 1 — No Characters Transmitted: Clock Never Reached LPUART

### Setup

* MCU: **NXP MCXN236**
* Peripheral: **LP_FLEXCOMM4** configured as LPUART
* Baud rate target: **57600**
* Method: Bare-metal polling, TX on P1\_9

### Symptom

LPUART4 was configured — baud divisor set, CTRL TE enabled — but the TX line stayed silent. Nothing appeared in the serial monitor. The `TDRE` flag appeared set (transmit data register empty), but writing to `DATA` had no effect on the output pin.

### Investigation

First suspicion was the baud rate divisor. Calculated OSR and SBR manually:

```
FRO12M = 12,000,000 Hz
OSR    = 15  (oversampling ratio, stored as OSR-1 in register → write 14)
SBR    = 13

Actual baud = 12,000,000 / ((14+1) * 13) = 61,538  ← close enough for 57600
```

Divisor looked fine. Then checked the **BAUD register** — bit **BOTHEDGE (bit 17)** was 0. For OSR values below 7, BOTHEDGE must be 1. Not the root cause here (OSR=15 is fine), but noted.

The real find came from checking the **clock tree**. LPUART4 is clocked through LP\_FLEXCOMM4, and LP\_FLEXCOMM4 needs two things that were both missing:

1. **Clock source not attached** — The functional clock mux for FLEXCOMM4 was left at its reset default (no clock / disabled). Fix:
    ```c
    CLOCK_AttachClk(kFRO12M_to_FLEXCOMM4);
    ```

2. **Clock divider not enabled** — The divider for FLEXCOMM4 clock was at reset value with the HALT bit set, gating the clock entirely. Fix:
    ```c
    CLOCK_SetClkDiv(kCLOCK_DivFlexcom4Clk, 1u);
    ```
    Setting the divider to 1 also clears the HALT bit, which was blocking the clock from reaching the peripheral.

### Root Cause

LP\_FLEXCOMM4 peripheral clock was never enabled. Without a running clock, the baud generator has nothing to divide — LPUART4 is deaf and mute regardless of how the baud registers are set.

### Fix

```c
/* Attach FRO 12 MHz to LP_FLEXCOMM4 and enable clock divider */
CLOCK_SetClkDiv(kCLOCK_DivFlexcom4Clk, 1u);
CLOCK_AttachClk(kFRO12M_to_FLEXCOMM4);
```

Call this **before** configuring BAUD, CTRL, or any other LPUART register.

### Key Takeaway

On MCX N series, attaching a clock source and clearing the divider HALT bit are two separate steps — both are required. A peripheral that powers up silently with `TDRE=1` but never actually transmits is a strong hint the functional clock is absent.

---

## Bug 2 — No RX Data: Input Buffer Never Enabled (IBE)

### Setup

* Same board and peripheral as above
* TX confirmed working (serial monitor showed output correctly)
* RX on P1\_8 (LP\_FLEXCOMM4\_RXD, MUX alt-function 2)
* Polling loop: `while (!(LPUART4->STAT & (1U << 21U)));` — wait for RDRF

### Symptom

TX was fully working. Sending characters from PuTTY at 57600 baud produced no response. The `RDRF` flag (STAT bit 21) stayed permanently zero no matter what was sent. Internal loopback had worked earlier, which proved the LPUART RX logic itself was fine.

### Investigation

Pin MUX was checked first. P1\_9 (TX) was already working with MUX = 2, so MUX = 2 on P1\_8 (RX) was confirmed correct. The PORT1 PCR register for pin 8 read:

```
PCR[8] = 0x00000200   ← MUX=2 set, but IBE=0
```

The **IBE (Input Buffer Enable)** bit was zero. Without IBE = 1 the digital input receiver is physically disconnected from the pin — the signal on P1\_8 never reaches the LPUART RX path regardless of MUX setting.

Finding the correct bit position took an extra step. An earlier reference incorrectly pointed to **bit 6**. Checking the MCXN236 Reference Manual PCR register table directly showed:

```
PCR register layout (MCXN236):
  bits [11:8]  →  MUX   (4-bit alternate function select)
  bit  [12]    →  IBE   (Input Buffer Enable)   ← correct position
  bit  [15]    →  LK    (Lock)
  bits [19:16] →  IRQC  (Interrupt config)
```

IBE is at **bit 12**, not bit 6. Setting bit 6 had silently done nothing — it is a reserved bit on this device.

### Root Cause

PORT PCR IBE bit not set. The MCXN236 input buffer is **disabled by default** at reset. Any pin used as a digital input — including a UART RX pin — must have IBE explicitly enabled, or the signal is invisible to the peripheral.

### Fix

```c
/* P1_8 → LP_FLEXCOMM4_RXD */
PCR_8 &= ~((0xFU << 8U) | (1U << 12U));  // clear MUX[11:8] and IBE[12]
PCR_8 |=  ( (2U  << 8U) | (1U << 12U));  // MUX=2, IBE=1  ← bit 12

/* P1_9 → LP_FLEXCOMM4_TXD  (output only — IBE not required) */
PCR_9 &= ~(0xFU << 8U);
PCR_9 |=  ( 2U  << 8U);                  // MUX=2
```

After this change `PCR[8]` reads `0x00001200` and RDRF sets correctly on the first received byte.

### Verification

Tested in PuTTY at 57600 baud. Each keystroke immediately set RDRF = 1. Added a simple echo loop:

```c
while (1) {
    /* Wait for received byte */
    while (!(LPUART4->STAT & (1U << 21U)));
    uint8_t rx = (uint8_t)(LPUART4->DATA & 0xFFU);

    /* Echo back */
    while (!(LPUART4->STAT & (1U << 23U)));
    LPUART4->DATA = rx;
}
```

Characters typed in PuTTY echoed back correctly — full-duplex UART RX and TX confirmed working.

### Key Takeaway

On MCXN236 (and MCX N series generally), **every RX pin needs IBE = 1 at PCR bit 12**. The MUX alone is not enough. Always cross-check bit positions against the device-specific Reference Manual — different NXP families place IBE at different bit positions (bit 6 on some, bit 12 on MCX N series).

---

## Environment

| Item | Detail |
|---|---|
| Board | FRDM-MCXN236 |
| MCU | NXP MCXN236 |
| Peripheral | LP\_FLEXCOMM4 / LPUART4 |
| Clock source | FRO 12 MHz (`kFRO12M_to_FLEXCOMM4`) |
| Baud rate | 57600 |
| IDE | MCUXpresso / VS Code + J-Link |
| Terminal | PuTTY |
| Approach | Bare-metal, no SDK peripheral drivers |
