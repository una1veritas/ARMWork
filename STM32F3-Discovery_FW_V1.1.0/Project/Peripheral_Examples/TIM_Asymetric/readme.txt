/**
  @page TIM_Asymetric TIM Asymetric Example
  
  @verbatim
  ******************** (C) COPYRIGHT 2012 STMicroelectronics *******************
  * @file    TIM_Asymetric/readme.txt 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    20-September-2012
  * @brief   TIM Asymetric Example Description.
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

This example shows how to configure the TIM peripheral to generate an Asymetric signal.
TIM8 is configured to generate an Asymetric signal with a programmable Phase-Shifted signal on TIM8_CH2:
	 - TIM8 Channel 1 is configured in PWM2 mode
	 - TIM8 Channel 2 is configured in Asymetric PWM2 mode
	 - The counter mode is center aligned mode
	 - The pulse length and the phase shift are programmed consecutively in TIM8_CCR2 and TIM8_CCR1.

TIM1 is configured to generating the reference signal on Channel1 used by TIM8:
	 - TIM1 is generating a PWM signal with frequency equal to 1.4560KHz.
	 - TIM1 is used as master for TIM8, the update event of TIM1 genarates the Reset counter
	   of TIM8 to synchronize the two signals: the refernce signal (TIM1_CH1) and the shifted signal (TIM8_CH2). 
	   
In this example TIM1 and TIM8 input clock (TIM18CLK) is set to APB2 clock (PCLK2)    
TIM1 and TIM8 signals are at frequency of  (SystemCoreClock / (Period + 1))  
               
TIM8 is gerating a signal with the following caracteristics:
	- Pulse lenght = (TIM8_CCR1 + TIM8_CCR2) / TIM8_CLK
	- Phase shift = TIM8_CCR1/TIM8_CLK
 with TIM8_CLK = (SystemCoreClock / (Period + 1)), as the prescaler is equal to zero.

@par Directory contents 

  - TIM_Asymetric/stm32f30x_conf.h     Library Configuration file
  - TIM_Asymetric/stm32f30x_it.c       Interrupt handlers
  - TIM_Asymetric/stm32f30x_it.h       Header for stm32f30x_it.c
  - TIM_Asymetric/main.c               Main program
  - TIM_Asymetric/main.h               Header for main.c
  - TIM_Asymetric/system_stm32f30x.c   STM32F30x system source file
           
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
     - Connect the following pins to an oscilloscope to monitor the different 
      waveforms:
       - PA.08 (TIM1_CH1)
       - PB.08 (TIM8_CH2)
       - PB.06 (TIM8_CH1)
    The shift is measured using the TIM1_CH1 as refence signal.
      
@par How to use it ? 

In order to make the program work, you must do the following :

 + EWARM
    - Open the TIM_Asymetric.eww workspace 
    - Rebuild all files: Project->Rebuild all
    - Load project image: Project->Debug
    - Run program: Debug->Go(F5)

 + MDK-ARM
    - Open the TIM_Asymetric.uvproj project
    - Rebuild all files: Project->Rebuild all target files
    - Load project image: Debug->Start/Stop Debug Session
    - Run program: Debug->Run (F5)    

 + TASKING
    - Open TASKING toolchain.
    - Click on File->Import, select General->'Existing Projects into Workspace' 
      and then click "Next". 
    - Browse to  TASKING workspace directory and select the project "TIM_Asymetric"   
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
    - Browse to the TrueSTUDIO workspace directory and select the project "TIM_Asymetric" 
    - Rebuild all project files: Select the project in the "Project explorer" 
      window then click on Project->build project menu.
    - Run program: Select the project in the "Project explorer" window then click 
      Run->Debug (F11)
  
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
