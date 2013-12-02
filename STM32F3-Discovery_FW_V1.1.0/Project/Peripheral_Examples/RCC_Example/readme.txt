/**
  @page RCC_Example RCC example
  
  @verbatim
  ******************** (C) COPYRIGHT 2012 STMicroelectronics *******************
  * @file    RCC_Example/readme.txt 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    20-September-2012
  * @brief   RCC Example Description.
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

This example shows how to:
  - Configure the HSE (High Speed Clock) as an RCC clock
  - Use the Clock Security System (CSS) feature to generate an NMI interrupt
  - Output the system clock on MCO

For debug purposes, the RCC_GetClocksFreq() function is used to retrieve the current
status and frequencies of different on-chip clocks.
You can see the RCC_ClockFreq structure content, which holds the frequencies of different
on-chip clocks, using your toolchain debugger.

This example also handles the High Speed External clock (HSE) failure detection
(known as Clock Security System, CSS): when the HSE clock disappears (broken or 
disconnected external Quartz), HSE and PLL are disabled (but no change to PLL 
configuration), HSI is selected as a system clock source and an interrupt (NMI) 
is generated. 
In the NMI ISR, the HSE and HSE ready interrupt are enabled. Once the HSE clock 
recovers, the HSERDY interrupt is generated and, in the RCC ISR routine, the system
clock is reconfigured to its previous state (before HSE clock failure). 
You can monitor the system clock on MCO pin (PA.8).
Four LEDs are toggled with a timing defined by the Delay function.

@note To use the CSS feature, you need to modify system_stm32f0xx.c file to configure 
      the HSE as system clock source (select "PLL_SOURCE_HSE" define) then once 
      the application is running try to generate the HSE failure (example: remove
      the HSE crystal from the socket, refer to more details in the section below). 

@par Directory contents 

  - RCC_Example/stm32f30x_conf.h     Library Configuration file
  - RCC_Example/stm32f30x_it.c       Interrupt handlers
  - RCC_Example/stm32f30x_it.h       Header for stm32f30x_it.c
  - RCC_Example/main.c               Main program
  - RCC_Example/main.h               Header for main.c
  - RCC_Example/system_stm32f30x.c   STM32F30x system source file
           
@note The "system_stm32f30x.c" file contains the system clock configuration for
      STM32F30x devices, and is customized for use with STM32F3-Discovery Kit. 
      The STM32F30x is configured to run at 72 MHz, following the three  
      configuration below:
        + PLL_SOURCE_HSI
           - HSI (~8 MHz) used to clock the PLL, and the PLL is used as system 
             clock source.  
        + PLL_SOURCE_HSE          
           - HSE (8 MHz) used to clock the PLL, and the PLL is used as system
             clock source.
           - The HSE crystal is not provided with the Discovery Kit, some 
             hardware modification are needed in manner to connect this crystal.
             For more details, refer to section "4.10 OSC clock" in "STM32F3 discovery kit User manual (UM1570)"
        + PLL_SOURCE_HSE_BYPASS   
           - HSE bypassed with an external clock (fixed at 8 MHz, coming from 
             ST-Link circuit) used to clock the PLL, and the PLL is used as 
             system clock source.
           - Some  hardware modification are needed in manner to bypass the HSE 
             with clock coming from the ST-Link circuit.
             For more details, refer to section "4.10 OSC clock" in "STM32F3 discovery kit User manual (UM1570)"
      User can select one of the three configuration in system_stm32f30x.c file
      (default configuration is PLL_SOURCE_HSE_BYPASS).           


@par Hardware and Software environment

  - This example runs on STM32F30x Devices.
  
  - This example has been tested with STMicroelectronics STM32F3-Discovery (MB1035) 
    and can be easily tailored to any other supported device and development board.

  - STM32F3-Discovery Set-up
    - None.  

@par How to use it ? 

In order to make the program work, you must do the following :

 + EWARM
    - Open the RCC.eww workspace 
    - Rebuild all files: Project->Rebuild all
    - Load project image: Project->Debug
    - Run program: Debug->Go(F5)

 + MDK-ARM
    - Open the RCC.uvproj project
    - Rebuild all files: Project->Rebuild all target files
    - Load project image: Debug->Start/Stop Debug Session
    - Run program: Debug->Run (F5)    

 + TASKING
    - Open TASKING toolchain.
    - Click on File->Import, select General->'Existing Projects into Workspace' 
      and then click "Next". 
    - Browse to  TASKING workspace directory and select the project "RCC"   
    - Rebuild all project files: Select the project in the "Project explorer" 
      window then click on Project->build project menu.
    - Run program: Select the project in the "Project explorer" window then click 
      Run->Debug (F11)

 + TrueSTUDIO for ARM
    - Open the TrueSTUDIO for ARM toolchain.
    - Click on File->Switch Workspace->Other and browse to TrueSTUDIO workspace 
      directory.
    - Click on File->Import, select General->'Existing Projects into Workspace' 
      and then click "Next". 
    - Browse to the TrueSTUDIO workspace directory and select the project "RCC" 
    - Rebuild all project files: Select the project in the "Project explorer" 
      window then click on Project->build project menu.
    - Run program: Select the project in the "Project explorer" window then click 
      Run->Debug (F11)
  
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
