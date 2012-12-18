/**
  @page I2S_FullDuplexDataExchangeIT I2S Full-Duplex with interrupt data transfer example
  
  @verbatim
  ******************* (C) COPYRIGHT 2012 STMicroelectronics ********************
  * @file    I2S/I2S_FullDuplexDataExchangeIT/readme.txt 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    23-October-2012
  * @brief   Description of the I2S Full-Duplex data exchange with Interrupt example.
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
SPIs in I2S Full-Duplex mode using interrupt and performing a transfer from Master to 
Slave.

- Hardware Description

To use this example, you need to load it on two STM32 boards (let's call them 
Board A and Board B) then connect these two boards through SPI lines and GND.
In the firmware example uncomment the dedicated line in the main.h file  to use
the SPI peripheral as STM32 Master device or as STM32 Slave .

@verbatim
*------------------------------------------------------------------------------*
|                BOARD A                                BOARD B                |
|                Master                                 Slave                  |
|         ____________________                   ____________________          |
|        |                    |                 |                    |         |
|        |                    |                 |                    |         | 
|        |     __________     |                 |     __________     |         |
|        |    | I2Sext   |    |                 |    | I2Sext   |    |         |                                
|        |    | Slave Rx |____|__SDin(I2Sext)___|____| Slave Tx |    |         |
|        |    |__________|    |                 |    |__________|    |         |
|        |    |SPI Master|____|______SDout______|____| SPI Slave|    |         |
|        |    |Tx Device |____|_______WS________|____| Rx Device|    |         |
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

- Software Description

In master board, I2S3 peripheral is configured as Master Transmitter, whereas I2S3ext
is configured as a slave Receiver.
In Slave board, I2S3 peripheral is configured as Slave Receiver, whereas I2S3ext
is configured as a slave Transmitter.
The data transfer is managed with interrupt. 
Both devices are in Philips standard configuration with 16 bit and 48KHz audio frequency. 

This example describes how to Configure I2S3 in Full duplex mode and send/Receive 
in the meantime I2S_Buffer_Tx and I2S_Buffer_Rx or I2Sext_Buffer_Rx buffer from 
(I2S3:BoradA) to (I2S3:BoardB).

The I2S transmit interrupt and the I2S receive interrupt are both enabled. And
in these interrupts subroutines, the I2S_Buffer_Tx is transmitted and the received
values are loaded in the I2S_Buffer_Rx buffer or I2Sext_Buffer_Rx buffer.

When the user press on the Key Button on the Master Board(BoardA), the master 
starts sending data stored in the I2S_Buffer_Tx buffer while the Slave Board (BoardB)
receives data in I2S_Buffer_Rx buffer.
At the same time, the Slave Board(BoardB) send data stored in the I2S_Buffer_Tx 
buffer while the BoardB receives data in I2Sext_Buffer_Rx buffer.

Once the transfer is completed, a comparison is done and TransferStatus gives the 
data transfer status:
  - TransferStatus PASSED if transmitted and received data are the same: Green LED is On.
  - TransferStatus FAILED if transmitted and received data are the different: Red LED is On.

@par Directory contents 

  - I2S/I2S_FullDuplexDataExchangeIT/stm32f30x_conf.h    Library Configuration file
  - I2S/I2S_FullDuplexDataExchangeIT/stm32f30x_it.c      Interrupt handlers
  - I2S/I2S_FullDuplexDataExchangeIT/stm32f30x_it.h      Interrupt handlers header file
  - I2S/I2S_FullDuplexDataExchangeIT/main.c              Main program
  - I2S/I2S_FullDuplexDataExchangeIT/main.h              Header for main.c module
  - I2S/I2S_FullDuplexDataExchangeIT/system_stm32f30x.c  STM32F30x system source file
  
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
    - Connect I2S3_SCK pin (PC.10) toI2S3_SCK pin (PC.10)
    - Connect I2S3_Din pin (PC.11) to I2S3_Din pin (PC.11)
    - Connect I2S3_Dout pin (PC.12) to I2S3_Dout pin (PC.12)
    - Connect I2S3_WS pin (PA.04) to I2S3_WS pin (PA.04)
    - The connection between the pins should use a short wires and a common Ground.

@par How to use it ? 

In order to make the program work, you must do the following :
 - Copy all source files from this example folder to the template folder under
   Project\STM32F30x_StdPeriph_Templates
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example
   
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
