/**
  @page IWDG_Reset  IWDG_Reset example
  
  @verbatim
  ******************** (C) COPYRIGHT 2014 STMicroelectronics *******************
  * @file    IWDG_Reset/readme.txt 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    11-February-2014
  * @brief   Description of the IWDG Reset example
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
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

  This example shows how to update at regular period the IWDG reload counter and 
  how to simulate a software fault generating an MCU IWDG reset on expiry of a 
  programmed time period.
  
  The IWDG timeout is set to 250 ms (the timeout may varies due to LSI frequency 
  dispersion then the preload period is set to 220 ms to guarantee a strong behavior).
  
  The program toggle LED2 each 220ms. When user button is pressed, an interrupt is
  generated and a write to a wrong address will be performed. This causes a system
  hard fault then the IWDG counter reaches 00h.       
  As a result, when the IWDG counter reaches 00h, the IWDG reset occurs.
  If the IWDG reset is generated, after the system resumes from reset, LED2 remains
  On for 3 seconds then it is toggled each 220ms.
  
  If the EXTI Line event does not occur, LED2 is toggling and the IWDG counter is 
  indefinitely refreshed in the main program infinite loop, and there is no IWDG reset.


@par Hardware and Software environment

  - This example runs on STM32F10x Devices.
  
  - This example has been tested with STMicroelectronics STM32NUCLEO kit (MB1136) Rev C 
    and can be easily tailored to any other supported device and development board.

@par How to use it? 

In order to make the program work, you must do the following :
 + EWARM
    - Open the IWDG_Reset.eww workspace 
    - Rebuild all files: Project->Rebuild all
    - Load project image: Project->Debug
    - Run program: Debug->Go(F5)
  
 + MDK-ARM
    - Open the IWDG_Reset.uvproj project
    - Rebuild all files: Project->Rebuild all target files
    - Load project image: Debug->Start/Stop Debug Session
    - Run program: Debug->Run (F5)  
    
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
