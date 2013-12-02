/**
  @page DMA_RAM_DAC DMA RAM to DAC example
  
  @verbatim
  ******************** (C) COPYRIGHT 2012 STMicroelectronics *******************
  * @file    DMA_RAM_DAC/readme.txt 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    20-September-2012
  * @brief   Description of the DMA RAM to DAC example.
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

This example provides a description of how to use a DMA channel to transfer 
data buffer from RAM memory to DAC.

DMA2 channel3 is configured to transfer continuously, word by word, a 32-word buffer 
to the dual DAC register DAC_DHR12RD. Both DAC channels conversions are configured 
to be triggered by TIM2 TRGO triggers and without noise/triangle wave generation.
12bit right data alignment is selected since we choose to access DAC_DHR12RD register.

@par Directory contents 

  - DMA_RAM_DAC/stm32f30x_conf.h     Library Configuration file
  - DMA_RAM_DAC/stm32f30x_it.c       Interrupt handlers
  - DMA_RAM_DAC/stm32f30x_it.h       Header for stm32f30x_it.c
  - DMA_RAM_DAC/main.c               Main program
  - DMA_RAM_DAC/main.h               Header for main.c
  - DMA_RAM_DAC/system_stm32f30x.c   STM32F30x system source file
           
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
    - Connect the oscilloscope to PA.04 and PA.05.
      
@par How to use it ? 

In order to make the program work, you must do the following :

 + EWARM
    - Open the DMA_RAM_DAC.eww workspace 
    - Rebuild all files: Project->Rebuild all
    - Load project image: Project->Debug
    - Run program: Debug->Go(F5)

 + MDK-ARM
    - Open the DMA_RAM_DAC.uvproj project
    - Rebuild all files: Project->Rebuild all target files
    - Load project image: Debug->Start/Stop Debug Session
    - Run program: Debug->Run (F5)    

 + TASKING
    - Open TASKING toolchain.
    - Click on File->Import, select General->'Existing Projects into Workspace' 
      and then click "Next". 
    - Browse to  TASKING workspace directory and select the project "DMA_RAM_DAC"   
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
    - Browse to the TrueSTUDIO workspace directory and select the project "DMA_RAM_DAC" 
    - Rebuild all project files: Select the project in the "Project explorer" 
      window then click on Project->build project menu.
    - Run program: Select the project in the "Project explorer" window then click 
      Run->Debug (F11)
  
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
