# MCXN236 Bare-Metal LPUART4 Interrupt-Driven Communication

## 1. Project Overview

This project implements interrupt-driven UART reception on the **NXP FRDM-MCXN236 development board** using direct register-level programming.

The LPUART4 peripheral receives data from a PC serial terminal. Every received byte generates a peripheral interrupt. The interrupt service routine reads the received byte and stores it in an RX circular buffer.

The main application continuously checks the circular buffer, retrieves available bytes, and transmits them back to the PC using UART polling-based transmission.

The implementation is developed in C without depending on the MCUXpresso SDK or HAL drivers.

## 2. Project Objectives

- Configure LP_FLEXCOMM4 to operate in LPUART mode.
- Configure UART TX and RX pin multiplexing.
- Configure UART baud rate and frame format.
- Understand peripheral interrupt generation.
- Configure the Cortex-M NVIC.
- Implement the peripheral interrupt service routine (ISR).
- Connect the ISR to the correct startup vector-table entry.
- Implement an RX circular buffer.
- Separate interrupt-driven reception from polling-based transmission.
- Understand the producer-consumer model in embedded communication.

## 3. Hardware and Software

### Hardware
- NXP FRDM-MCXN236 development board
- USB connection to the development board
- PC with a serial terminal

### Software and Development Tools
- Visual Studio Code
- Arm GNU Toolchain
- MCUXpresso IDE / LinkServer tools for debugging and flashing
- Cortex-Debug extension
- Serial terminal

## 4. Hardware Configuration

### LPUART Peripheral

The LP_FLEXCOMM4 peripheral is configured to operate in UART mode.

| Configuration | Selection |
|---|---|
| Peripheral | LP_FLEXCOMM4 |
| UART instance | LPUART4 |
| RX pin | P1_8 |
| TX pin | P1_9 |
| RX function | FC4_P0 |
| TX function | FC4_P1 |
| Data bits | 8 |
| Parity | None |
| Stop bits | 1 |
| Flow control | Disabled |

### Clock and Peripheral Configuration

The implementation configures the required peripheral clock and reset registers.

- Enable the LP_FLEXCOMM4 peripheral clock.
- Release the peripheral from reset.
- Select the LPUART clock source through FCCLKSEL4.
- Configure the FLEXCOMM peripheral selection through PSELID.
- Configure the baud-rate divider.
- Enable the UART transmitter and receiver.

### Pin Configuration

| Pin | Function |
|---|---|
| P1_8 | LPUART4 RX |
| P1_9 | LPUART4 TX |

## 5. Interrupt Configuration

UART reception is implemented using interrupts instead of continuously polling the receive status register.

The interrupt configuration involves two independent components.

### 5.1 Peripheral Interrupt Enable

The LPUART receiver interrupt is enabled using the Receiver Interrupt Enable (RIE) field in the UART CTRL register.

When a character is received:
1. The UART hardware receives the serial data.
2. The received byte is transferred into the receive data register.
3. The Receive Data Register Full (RDRF) status flag is asserted.
4. If RIE is enabled, the peripheral requests an interrupt.

### 5.2 NVIC Configuration

The LP_FLEXCOMM4 interrupt is enabled through the Nested Vectored Interrupt Controller.

| Parameter | Value |
|---|---|
| External IRQ number | 39 |
| NVIC register | ISER1 |
| Enable bit | 7 |
| Vector-table index | 55 |
| Vector-table offset | 0xDC |

The NVIC enable register is configured using a write-one-to-set operation.

The interrupt priority is configured through the corresponding NVIC priority register.

### 5.3 Vector Table Mapping

The Cortex-M vector table contains 16 core exception entries before the external peripheral interrupt entries.

Therefore:

`Vector index = 16 + External IRQ number`

For LP_FLEXCOMM4:

`16 + 39 = 55`

The vector-table entry at index 55 is mapped to:

`LP_FLEXCOMM4_IRQHandler`

This mapping ensures that the processor executes the correct ISR when the LP_FLEXCOMM4 interrupt is accepted.

## 6. Interrupt Service Routine

The ISR performs only the essential receive operations:

1. Check whether RDRF is asserted.
2. Read the received byte from the DATA register.
3. Push the received byte into the RX circular buffer.
4. Return from the interrupt.

The ISR does not perform UART transmission or lengthy processing.

This keeps interrupt execution short and allows the main application to process received data independently.

## 7. Circular Buffer Implementation

A circular buffer is used to temporarily store received bytes between the UART ISR and the main application.

### Buffer Structure

The ring buffer maintains:

| Variable | Purpose |
|---|---|
| `buffer[]` | Stores received bytes |
| `head` | Next position for writing data |
| `tail` | Next position for reading data |
| `count` | Tracks the number of stored bytes |
| `buffer_overflow` | Tracks buffer overflow events |

### Producer-Consumer Model

**Producer – UART ISR**
- Receives a byte from the UART DATA register.
- Inserts the byte at the head position.
- Advances the head index.
- Handles the buffer-full condition.

**Consumer – Main Application**
- Checks whether data is available.
- Retrieves a byte from the tail position.
- Advances the tail index.
- Transmits the retrieved byte.

The buffer allows the ISR and main application to perform reception and processing as separate operations.

**Note:** The current count-based implementation requires proper synchronization because both the ISR and main application modify `count`. A production-ready version should use suitable critical-section protection or a single-producer/single-consumer design without a shared count.

## 8. Application Data Flow

```text
             PC Serial Terminal
                     |
                     | UART Serial Data
                     v
              LPUART4 Hardware
                     |
                     | RDRF Flag
                     v
             LP_FLEXCOMM4 IRQ
                     |
                     v
        LP_FLEXCOMM4_IRQHandler()
                     |
                     | Read DATA Register
                     v
              RX Circular Buffer
                     |
                     | Retrieve Byte
                     v
                 Main Loop
                     |
                     | UART Polling TX
                     v
              LPUART4 TX Pin
                     |
                     v
              PC Serial Terminal
```

## 9. Testing and Verification

The project was tested using a PC serial terminal connected to the development board.

### Test Configuration

| Parameter | Value |
|---|---|
| Interface | UART |
| Baud rate | As configured in firmware |
| Data bits | 8 |
| Parity | None |
| Stop bits | 1 |
| Flow control | None |

### Test Cases

| Test | Input | Expected Result |
|---|---|---|
| 1 | `A` | Character A is echoed |
| 2 | `ABC` | Characters are echoed in order |
| 3 | `HELLO` | Received characters are echoed |
| 4 | Multiple characters | RX ISR stores bytes in the circular buffer |

### Debugging Verification

The following variables can be monitored through the debugger:

- `rx_buffer.head`
- `rx_buffer.tail`
- `rx_buffer.count`
- `rx_buffer.buffer_overflow`

These variables help verify byte insertion, retrieval, buffer movement, and overflow behavior.

## 10. Key Learning Outcomes

Through this project, the following concepts were implemented and understood:

- Direct register-level UART configuration.
- Peripheral clock and reset configuration.
- UART pin multiplexing.
- UART baud-rate configuration.
- Interrupt-driven data reception.
- Cortex-M NVIC configuration.
- Interrupt priority configuration.
- Startup vector-table mapping.
- ISR implementation.
- Circular buffer operation.
- Producer-consumer communication.
- Polling-based UART transmission.
- Debugging interrupt-driven embedded software.

## 11. Future Improvements

- Implement a larger configurable RX buffer.
- Add UART overrun and framing error handling.
- Improve circular-buffer concurrency handling.
- Implement interrupt-driven UART transmission.
- Add TX circular buffering.
- Implement line-based message reception.
- Add command parsing through a UART terminal.
- Integrate DMA-based UART communication.

## 12. Build and Flash

Build the project using the supplied Makefile and Arm GNU Toolchain.

Flash the generated firmware using LinkServer:

```powershell
LinkServer flash MCXN236:FRDM-MCXN236 load .\build\firmware.elf
```

Connect the PC serial terminal using the configured COM port and UART settings.

---

**Project:** MCXN236 Bare-Metal LPUART4 Interrupt-Driven Communication

**Implementation:** Register-level C programming

**Architecture:** Interrupt-driven RX + circular buffer + polling TX