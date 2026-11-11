# 03 - LPUART4 Polling-Based TX/RX Communication

## Overview
Implemented bare-metal UART communication on the NXP FRDM-MCXN236 development board using the LPUART4 peripheral.

The project demonstrates UART initialization, polling-based data transmission, reception, and character echo functionality without using the MCUXpresso SDK.

## Hardware
- MCU: NXP MCXN236
- Development Board: FRDM-MCXN236
- Peripheral: LPUART4 (LP_FLEXCOMM4)
- Terminal: PuTTY
- Serial Port: COM3

## Pin Configuration

| Signal | MCU Pin | Function |
|---|---|---|
| UART RX | P1_8 | FC4_P0 |
| UART TX | P1_9 | FC4_P1 |

## Features Implemented
- FLEXCOMM4 clock configuration
- Peripheral reset and initialization
- UART pin multiplexing
- Baud rate configuration
- 8-bit data transmission and reception
- Polling-based TX using TDRE
- Polling-based RX using RDRF
- UART character echo

## Communication Flow

1. Initialize LPUART4.
2. Wait until RDRF indicates received data is available.
3. Read the received character from DATA.
4. Wait until TDRE indicates the transmitter is ready.
5. Write the received character into DATA.
6. Repeat continuously.

## Verification

Tested UART communication using PuTTY.

**Test case:** Send character `A` from the serial terminal.

**Expected result:** The MCU receives `A` and transmits the same character back.

**Observed result:** Character `A` successfully displayed in PuTTY after transmission.

## Debugging

Used VS Code and Cortex-Debug with the MCU-Link probe.

Verified:
- Execution reaches the DATA register write.
- Received data is correctly read.
- Transmitted character appears in PuTTY.
- One complete TX/RX echo cycle executes per received character.

## Future Improvements
- String transmission and reception
- Circular buffer implementation
- Interrupt-driven UART communication
- UART communication using DMA

