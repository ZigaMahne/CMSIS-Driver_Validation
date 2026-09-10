# USART CMSIS-Driver Validation Example

This folder provides a **ready-to-use CMSIS-Driver Validation application** for testing and verifying the **USART driver** on the STMicroelectronics [**NUCLEO-C562RE**](https://www.st.com/en/evaluation-tools/nucleo-c562re.html) development board.

---

## Overview

This application validates USART driver compliance to the [CMSIS-Driver USART Interface Driver Specification](https://arm-software.github.io/CMSIS_6/latest/Driver/group__usart__interface__gr.html).

It serves as a quick-start configuration for developers who need to ensure USART driver compliance and performance using the CMSIS standard.

This example was created using the CMSIS-Driver Validation [**Template**](https://github.com/ARM-software/CMSIS-Driver_Validation/blob/main/Template/README.md) application.

---

## Configuration

Tests and parameters are configured in the [DV_USART_Config.h](RTE/CMSIS_Driver_Validation/DV_USART_Config.h) configuration file.

You can modify this file to customize test parameters, timing, and execution behavior.

---

## Requirements

### **Hardware**
- STMicroelectronics [**NUCLEO-C562RE**] board
- STMicroelectronics [**STM32F429I-DISC1**](https://www.st.com/en/evaluation-tools/32f429idiscovery.html) board (for the USART Server)
- **some wires** ([documentation link](https://arm-software.github.io/CMSIS-Driver_Validation/latest/group__dv__usart.html))
- **USB-A to USB-C** cable
- **USB-A to mini USB** cable (for the SPI Server)

### **Software**
- [Visual Studio Code](https://code.visualstudio.com/)
- [Arm Keil Studio Pack (MDK v6) Extension](https://marketplace.visualstudio.com/items?itemName=Arm.keil-studio-pack)
- Terminal application (e.g., PuTTY, Tera Term)

---

## Usage Instructions

This validation requires **USART Server** running on a dedicated **STM32F429I-DISC1** board.  
Build and download the **USART Server** application, located in **Tools/USART_Server/STM32F429I-DISC1** of the **CMSIS-Driver_Validation pack installation**, to the STM32F429I-DISC1 board.

To run the driver validation:

1. Open the [**CMSIS_DV solution**](CMSIS_DV.csolution.yml) in **Visual Studio Code**.
2. Connect the **NUCLEO-C562RE** board pins to **USART Server** pins as described in the following table:
   | NUCLEO-C562RE                      | STM32F429I-DISC1 (USART Server) |
   |------------------------------------|---------------------------------|
   | ARDUINO ARD.D0,- PB15 (USART1_RX)  | PA9  (USART1_TX)                |
   | ARDUINO ARD.D1 - PB15 (USART1_TX)  | PA10 (USART1_RX)                |
   | GND                                | GND                             |

   > **Note:** For NUCLEO-C562RE schematic see: [board schematic (Arduino on sheet 31)](https://www.st.com/resource/en/user_manual/um3615-stm32c5-nucleo64-board-mb2213-stmicroelectronics.pdf).  
   > **Note:** For connection between DUT and USART Server see: [**Test Mode : USART Server** connections documentation](https://arm-software.github.io/CMSIS-Driver_Validation/latest/group__dv__usart.html).  
3. Connect the board’s **ST-Link USB interface** to your PC using a USB-A to micro USB cable.
4. Open a terminal and connect to the **STLink Virtual COM Port (COMx)** using `115200 8-N-1` settings.
5. Power-up and start the **USART Server** on the **STM32F429I-DISC1** board.
6. Build and run the validation application.
7. Observe the test output in the terminal window.

---

## Example Test Results

A successful validation output should resemble the following:

```
CMSIS-Driver_Validation v3.1.0 CMSIS-Driver USART Test Report   Sep  8 2026   14:46:51 

Test Mode:          USART Server
Default settings:
 - Mode:            Asynchronous
 - Data bits:       8
 - Parity:          None
 - Stop bits:       1
 - Flow control:    None
 - Clock polarity:  CPOL0
 - Clock phase:     CPHA0
 - Bus speed:       115200 bauds
 - Number of Items: 512

TEST 01: USART_GetVersion                 
  DV_USART.c (1924): [INFO] Driver API version 2.4, Driver version 4.0
                                          PASSED
TEST 02: USART_GetCapabilities            PASSED
TEST 03: USART_Initialize_Uninitialize    PASSED
TEST 04: USART_PowerControl               
  DV_USART.c (2235): [WARNING] PowerControl (ARM_POWER_LOW) is not supported
                                          PASSED
TEST 05: USART_Mode_Asynchronous          PASSED
TEST 06: USART_Mode_Synchronous_Master    NOT EXECUTED
TEST 07: USART_Mode_Synchronous_Slave     NOT EXECUTED
TEST 08: USART_Mode_Single_Wire           NOT EXECUTED
TEST 09: USART_Mode_IrDA                  NOT EXECUTED
TEST 10: USART_Data_Bits_5                NOT EXECUTED
TEST 11: USART_Data_Bits_6                NOT EXECUTED
TEST 12: USART_Data_Bits_7                NOT EXECUTED
TEST 13: USART_Data_Bits_8                PASSED
TEST 14: USART_Data_Bits_9                NOT EXECUTED
TEST 15: USART_Parity_None                PASSED
TEST 16: USART_Parity_Even                NOT EXECUTED
TEST 17: USART_Parity_Odd                 NOT EXECUTED
TEST 18: USART_Stop_Bits_1                PASSED
TEST 19: USART_Stop_Bits_2                PASSED
TEST 20: USART_Stop_Bits_1_5              NOT EXECUTED
TEST 21: USART_Stop_Bits_0_5              NOT EXECUTED
TEST 22: USART_Baudrate_Min               PASSED
TEST 23: USART_Baudrate_Max               PASSED
TEST 24: USART_Number_Of_Items            PASSED
TEST 25: USART_GetTxCount                 PASSED
TEST 26: USART_GetRxCount                 PASSED
TEST 27: USART_GetTxRxCount               NOT EXECUTED
TEST 28: USART_AbortSend                  PASSED
TEST 29: USART_AbortReceive               PASSED
TEST 30: USART_AbortTransfer              NOT EXECUTED
TEST 31: USART_TxBreak                    NOT EXECUTED

Test Summary: 31 Tests, 16 Passed, 0 Failed.
Test Result: PASSED
```

> **Note:** Some warnings may indicate optional features not supported by the hardware.
