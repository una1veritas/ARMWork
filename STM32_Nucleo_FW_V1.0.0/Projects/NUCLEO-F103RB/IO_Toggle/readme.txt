/**
  @page IO_Toggle  IO_Toggle example
  
  @verbatim
  ******************** (C) COPYRIGHT 2014 STMicroelectronics *******************
  * @file    IO_Toggle/readme.txt 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    11-February-2014
  * @brief   Description of the Template example
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

This example describes how to toggle IO with two different frequency.
This is done using Delay() function which is implemented based on the SysTick 
end-of-count event. 

When starting program, LED2 remain toggling with a frequency equal to ~10Hz 
(a delay of 50ms is put). By pushing the User Button mounted on STM32NUCLEO,
LED2 will toggle with a frequency equal to ~2.5Hz(a delay of 200ms is put).
This is done in an infinite loop.

@par Hardware and Software environment

  - This example runs on STM32F10x Devices.

  - This example has been tested with STMicroelectronics STM32NUCLEO Kit(MB1136) Rev C 
    and can be easily tailored to any other supported device and development board.  

@par How to use it? 

In order to make the program work, you must do the following :
 + EWARM
    - Open the IO_Toggle.eww workspace 
    - Rebuild all files: Project->Rebuild all
    - Load project image: Project->Debug
    - Run program: Debug->Go(F5)
  
 + MDK-ARM
    - Open the IO_Toggle.uvproj project
    - Rebuild all files: Project->Rebuild all target files
    - Load project image: Debug->Start/Stop Debug Session
    - Run program: Debug->Run (F5)   
          
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
