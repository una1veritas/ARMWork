/**
  @page CEC_MultiAddress CEC Multiple Address communication example
  
  @verbatim
  ******************** (C) COPYRIGHT 2012 STMicroelectronics *******************
  * @file    CEC/CEC_MultiAddress/readme.txt 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    20-September-2012
  * @brief   Description of the CEC Multiple Address communication example.
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

This example provides a basic communication between two HDMI-CEC devices using 
interrupts, in which the follower one supports two addresses at the same time. 
The first CEC device sends a command to the CEC other device. 
The data received by the first and second CEC device are stored respectively in 
ReceiveBuffer. In both boards, the data transfers is managed using CEC_IRQHandler
in stm32F37x_it.c file.

The example illustrates the use of the CEC communication between two devices 
(2 x STM32373C-EVAL boards). A device configured as transmitter can send a frame 
to the other device by pressing the Joystick button(Right,Left) on the EVAL board. 
The receiver device supports two addresses(0x03=>Tuner, 0x05=>Audio system).

- Hardware Description

To use this example, you need to load it on two STM32373C-EVAL boards (let's call 
them Device_1 and Device_2) then connect these two boards through CEC lines(PB8 
or HDMI connectors) and GND.
In the firmware example, uncomment the dedicated line in the main.h file  to use
the CEC peripheral as STM32 device_1 or as STM32 device_2.

@verbatim
*------------------------------------------------------------------------------*
|           STM32373C-EVAL                         STM32373C-EVAL              |
|         Device Address :0x01                    Device Address1 :0x03        |
|                                                 Device Address2 :0x05        |
|             (Initiator)                             (Follower)               |
|         ____________________                   ____________________          |
|        |                    |                 |                    |         |
|        |                    |                 |                    |         | 
|        |     __________     |                 |     __________     |         |
|        |    |   CEC    |____|____CECLine______|____|   CEC    |    |         |
|        |    | Device_1 |    |                 |    | Device_2 |    |         |
|        |    |__________|    |                 |    |__________|    |         |
|        |                    |                 |                    |         |
|        |  O LD1             |                 |  O LD1             |         |
|        |  O LD2    Joystick |                 |  O LD2    Joystick |         |
|        |  O LD3        _    |                 |  O LD3        _    |         |
|        |  O LD4       |_|   |                 |  O LD4       |_|   |         |
|        |                    |                 |                    |         |
|        |             GND O--|-----------------|--O GND             |         |
|        |____________________|                 |____________________|         |
|                                                                              |
|                                                                              |
*------------------------------------------------------------------------------*
@endverbatim

- Software Description

At each joystick buttons press:
- The CEC Transmitter device sends a command to the specific receiver device.
  - If Joystick RIGHT is pressed, a CEC message to the Tuner device(0x03) is sent
  - If Joystick LEFT is pressed, a CEC message to the Audio system device(0x05) is sent
  
- At the end of the header reception, the own address is compared with the received
  destination address. In the case of matching address with 
  - Tuner        => The message is received and Green Led is on,
  - Audio system => The message is received and Blue Led is on.

@par Directory contents 

  - CEC/CEC_MultiAddress/stm32f37x_conf.h    Library Configuration file
  - CEC/CEC_MultiAddress/stm32f37x_it.c      Interrupt handlers
  - CEC/CEC_MultiAddress/stm32f37x_it.h      Interrupt handlers header file
  - CEC/CEC_MultiAddress/main.c              Main program
  - CEC/CEC_MultiAddress/main.h              Header for main.c module
  - CEC/CEC_MultiAddress/system_stm32f37x.c  STM32F37x system source file
  
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
    Connect the boards by using one of the two following alternatives:
    - A HDMI Cables between all boards HDMI-CEC connectors (CN1 or CN2) on 
      STM32373C-EVAL . 
    - Use a simple wire between all devices CEC Lines (PB.8), in this case don't 
      forget to connect all boards grounds together.
@par How to use it ? 

In order to make the program work, you must do the following :
 - Copy all source files from this example folder to the template folder under
   Project\STM32F37x_StdPeriph_Templates
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
