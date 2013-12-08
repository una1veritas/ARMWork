/**
  @page I2S_DataExchangeInterrupt I2S Interrupt example
  
  @verbatim
  ******************** (C) COPYRIGHT 2012 STMicroelectronics *******************
  * @file    I2S/I2S_DataExchangeInterrupt/readme.txt  
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    20-September-2012
  * @brief   Description of the I2S Interrupt example.
  ******************************************************************************
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  @endverbatim

@par Example Description 

This example provides a description of how to set a communication between two
SPIs in I2S mode using interrupts and performing a transfer from Master to Slave.

- Hardware Description

To use this example, you need to load it on two STM32 boards (let's call them 
Board A and Board B) then connect these two boards through SPI lines and GND.
In the firmware example uncomment the dedicated line in the main.h file  to use
the SPI peripheral as STM32 Master device or as STM32 Slave .

@verbatim
*------------------------------------------------------------------------------*
|                BOARD A                                BOARD B                |
|         ____________________                   ____________________          |
|        |                    |                 |                    |         |
|        |                    |                 |                    |         | 
|        |     __________     |                 |     __________     |         |
|        |    |SPI Master|____|_______SD________|____|SPI Slave |    |         |
|        |    |  Device  |____|_______WS________|____|  Device  |    |         |
|        |    |__________|____|_______CK________|____|__________|    |         |
|        |                    |                 |                    |         |
|        |                    |                 |  O LD1             |         |
|        |              Key   |                 |  O LD2             |         |
|        |               _    |                 |  O LD3             |         |
|        |              |_|   |                 |  O LD4             |         |
|        |                    |                 |                    |         |
|        |             GND O--|-----------------|--O GND             |         |
|        |____________________|                 |____________________|         |
|                                                                              |
|                                                                              |
*------------------------------------------------------------------------------*
@endverbatim

@note
- The connection between the pins should use a short wires and a common Ground.

- Software Description

In master board, I2S peripheral is configured as Master Transmitter with Interrupt,
whereas in Slave board, I2S peripheral is configured as Slave Receiver with Interrupt. 
Both devices are in Philips standard configuration with 16bit extended to 
32 bit data packet and 48KHz audio frequency. 

The I2S transmit interrupt and the I2S receive interrupt are both enabled. And
in these interrupts subroutines, the I2S_Buffer_Tx is transmitted and the received
values are loaded in the I2S_Buffer_Rx buffer. Only the significant 16 MSBs are
sent and received, while the 32 packet remaining 16 LSBs are filled with 0 values
and don't generate any interrupt.

When the user press on the Key Button on the Master Board, the master transmitter
start sending data stored in the I2S_Buffer_Tx buffer while the Slave Board receives
data I2S_Buffer_Rx buffer.

Once the transfer is completed a comparison is done and TransferStatus gives the 
data transfer status:
  - TransferStatus PASSED if transmitted and received data are the same: Green LED is On.
  - TransferStatus FAILED if transmitted and received data are the different: Red LED is on.

@par Directory contents 

  - I2S/I2S_DataExchangeInterrupt/stm32f37x_conf.h    Library Configuration file
  - I2S/I2S_DataExchangeInterrupt/stm32f37x_it.c      Interrupt handlers
  - I2S/I2S_DataExchangeInterrupt/stm32f37x_it.h      Interrupt handlers header file
  - I2S/I2S_DataExchangeInterrupt/main.c              Main program
  - I2S/I2S_DataExchangeInterrupt/main.h              Header for main.c module
  - I2S/I2S_DataExchangeInterrupt/system_stm32f37x.c  STM32F37x system source file
  
@note The "system_stm32f37x.c" is generated by an automatic clock configuration 
      system and can be easily customized to your own configuration. 
      To select different clock setup, use the "STM32F37x_Clock_Configuration_V1.0.0.xls" 
      provided with the AN4132 package available on <a href="http://www.st.com/internet/mcu/family/141.jsp">  ST Microcontrollers </a>
         
@par Hardware and Software environment

  - This example runs on STM32F37x Devices.
  
  - This example has been tested with STMicroelectronics STM32373C-EVAL (STM32F37x)
    evaluation board and can be easily tailored to any other supported device 
    and development board.

  - STM32373C-EVAL Set-up
    - Connect SPI1 SCK pin (PA.05) to SPI1 SCK pin (PA.05)
    - Connect SPI1 WS pin (PA.04) to SPI1 WS pin (PA.04)
    - Connect SPI1 SD pin (PA.07) to SPI1 SD pin (PA.07)

@par How to use it ? 

In order to make the program work, you must do the following :
 - Copy all source files from this example folder to the template folder under
   Project\STM32F37x_StdPeriph_Templates
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
