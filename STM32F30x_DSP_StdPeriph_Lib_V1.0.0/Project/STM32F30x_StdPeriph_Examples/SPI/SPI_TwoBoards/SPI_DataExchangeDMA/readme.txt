/**
  @page SPI_DataExchangeDMA SPI Communication Boards Data Exchange using DMA example
  
  @verbatim
  ******************** (C) COPYRIGHT 2012 STMicroelectronics *******************
  * @file    SPI/SPI_TwoBoards/SPI_DataExchangeDMA/readme.txt 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    23-October-2012
  * @brief   Description of the SPI Communication Boards Data Exchange using 
  *          DMA example.
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

This example provides a small application in which joystick buttons are used 
to trigger SPI communications using DMA and though using SPI firmware library.

- Hardware Description

To use this example, you need to load it on two STM32 boards (let's call them 
Board A and Board B) then connect these two boards through SPI lines and GND.
In the STM32 master board (Board A), connect the timer output capture compare 
channel into NSS line.  
In the firmware example uncomment the dedicated line in the main.h file to use
the SPI peripheral as STM32 Master device or as STM32 Slave.

@verbatim
*------------------------------------------------------------------------------*
|                BOARD A                                BOARD B                |
|         ____________________                   ____________________          |
|        |                    |                 |                    |         |
|        |                    |                 |                    |         | 
|        |     __________ ____|_______SCK_______|____ __________     |         |
|        |    |SPI Master|____|_______MOSI______|____|SPI Slave |    |         |
|        |    |  Device  |____|_______MISO______|____|  Device  |    |         |
|        |    |__________|____|___    NSS   ____|____|__________|    |         |
|        |                    |   |        |    |                    |         |
|        |  O LD1  TIM_CHx O__|___|        |    |  O LD1             |         |
|        |  O LD2    Joystick |            |    |  O LD2    Joystick |         |
|        |  O LD3        _    |            |    |  O LD3        _    |         |
|        |  O LD4       |_|   |            |    |  O LD4       |_|   |         |
|        |                    |            |    |                    |         |
|        |             GND O--|-----------------|--O GND             |         |
|        |____________________|                 |____________________|         |
|                                                                              |
|                                                                              |
*------------------------------------------------------------------------------*
@endverbatim

@note
- The connection between the pins should use a short wires and a common Ground. 

- Software Description

In master board, SPI peripheral is configured as Master full duplex with DMA
and NSS hardware mode.
The TIM2 is configured to generate 4KHz PWM signal with 50% duty cycle on TIM2_CH2 
pin (PA.01), this signal is used as DMA trigger and as NSS signal input to latch
the SPI data transfers. Whereas in Slave board, SPI peripheral is configured as 
Slave Full duplex with DMA and NSS hardware mode.

At each joystick buttons press:
- The Master sends the specific command to the Slave using the TIM2_CH2 DMA 
  request (DMA1_Channel7)(the command contains the transaction code (CMD_RIGHT, 
  CMD_LEFT, CMD_UP, CMD_DOWN or CMD_SEL) and receives the ACK command from the 
  Slave using SPI_Rx DMA request(DMA2_Channel1) 

- The Slave receives the command using SPI_Rx DMA request(DMA2_Channel1) and sends 
  the ACK command using the SPI_Tx DMA request (DMA2_Channel2).

- The Master sends the defined NumberOfByte from TxBuffer using the TIM2_CH2 DMA 
  request (DMA1_Channel7) to the Slave and receives NumberOfByte from the Slave 
  using SPI_Rx DMA request(DMA2_Channel1) into Rxbuffer.

- The Slave sends the defined NumberOfByte from Txbuffer using the SPI_Tx DMA 
  request (DMA2_Channel2) to the Master and receives NumberOfByte from the Master 
  using SPI_Rx DMA request(DMA2_Channel1) into Rxbuffer.

- The Master compares the NumberOfByte received data with the defined ones into 
  TxBuffer and check the ACK command, received data correctness is signaled by LD 
  lightening and though as follow:
   - Joystick JOY_RIGHT and data correctly received ==> LD2 and LD3 ON and LD4 OFF 
   - Joystick JOY_LEFT and data correctly received  ==> LD4 ON, LD2 and LD3 are OFF
   - Joystick JOY_UP and data correctly received    ==> LD2 ON, LD3 and LD4 are OFF
   - Joystick JOY_DOWN and data correctly received  ==> LD3 ON, LD2 and LD4 are OFF

- The Slave compares the NumberOfByte received data with the defined ones into 
  TxBuffer and check the received command,Received data correctness is signaled 
  by LED lightening and though as follow:
   - Received command CMD_RIGHT and data correctly received ==> LD2 and LD3 ON and LD4 OFF 
   - Received command CMD_LEFT and data correctly received  ==> LD2 ON, LD3 and LD4 are OFF
   - Received command CMD_UP and data correctly received    ==> LD2 ON, LD3 and LD4 are OFF
   - Received command CMD_DOWN and data correctly received  ==> LD3 ON, LD2 and LD4 are OFF

In both boards, the SPI is configured to use the slave hardware management to 
manage the NSS pin.
 
The SysTick is configured to generate interrupt each 10ms. A dedicated counter 
inside the SysTick ISR is used to toggle the LD1 each 100ms indicating that the 
firmware is running.
A defined communication timeout is insuring that the application will not remain 
stuck if the SPI communication is corrupted.
You can adjust this timeout through the USER_TIMEOUT  define inside main.h file
depending on CPU frequency and application conditions (interrupts routines, 
number of data to transfer, baudrate, CPU frequency...).

These operations can be repeated infinitely.

@par Directory contents 

  - SPI/SPI_TwoBoards/SPI_DataExchangeDMA/stm32f30x_conf.h    Library Configuration file
  - SPI/SPI_TwoBoards/SPI_DataExchangeDMA/stm32f30x_it.c      Interrupt handlers
  - SPI/SPI_TwoBoards/SPI_DataExchangeDMA/stm32f30x_it.h      Interrupt handlers header file
  - SPI/SPI_TwoBoards/SPI_DataExchangeDMA/main.c              Main program
  - SPI/SPI_TwoBoards/SPI_DataExchangeDMA/main.h              Main program header file
  - SPI/SPI_TwoBoards/SPI_DataExchangeDMA/system_stm32f30x.c  STM32F30x system source file
  
@note The "system_stm32f30x.c" is generated by an automatic clock configuration 
      system and can be easily customized to your own configuration. 
      To select different clock setup, use the "STM32F30x_Clock_Configuration_V1.0.0.xls" 
      provided with the AN4152 package available on <a href="http://www.st.com/internet/mcu/family/141.jsp">  ST Microcontrollers </a>
     
@par Hardware and Software environment

  - This example runs on STM32F30x Devices.
  
  - This example has been tested with STMicroelectronics STM32303C-EVAL (STM32F30x)
    evaluation board and can be easily tailored to any other supported device 
    and development board.

  - STM32303C-EVAL Set-up
    - Connect SPI3 SCK pin (PC.10) to SPI3 SCK pin (PC.10)
    - Connect SPI3 MISO pin (PC.11) to SPI3 MISO pin (PC.11)
    - Connect SPI3 MOSI pin (PC.12) to SPI3 MOSI pin (PC.12)
    - Connect SPI3 NSS pin (PA.04) to SPI3 NSS pin (PA.04)
    - Connect TIM2_CH2 pin (PA.01) to SPI3 NSS pin (PA.04) (On master board only to trigger the DMA transfers)
    - Please make sure that a jumper is not plugged in CN11 connector.
    - Connect the two boards to a common Ground. 

@par How to use it ? 

In order to make the program work, you must do the following :
 - Copy all source files from this example folder to the template folder under
   Project\STM32F30x_StdPeriph_Templates
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example
    
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
