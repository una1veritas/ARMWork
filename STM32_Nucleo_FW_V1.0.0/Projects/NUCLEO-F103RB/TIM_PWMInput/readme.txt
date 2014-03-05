/**
  @page TIM_PWMInput  TIM_PWMInput example
  
  @verbatim
  ******************** (C) COPYRIGHT 2014 STMicroelectronics *******************
  * @file    TIM_PWMInput/readme.txt 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    11-February-2014
  * @brief   Description of the TIM_PWMInput example
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

This example shows how to use the TIM peripheral to measure the frequency and 
duty cycle of an external signal.

The TIM2CLK frequency is set to 2*APB1 clock. Since since APB1 prescaler is set 
to 2, TIM2CLK = 2*PCLK1 = HCLK = SystemCoreClock.

TIM2 is configured in PWM Input Mode: the external signal is connected to 
TIM2 Channel2 used as input pin.
To measure the frequency and the duty cycle we use the TIM2 CC2 interrupt request,
so In the TIM2_IRQHandler routine, the frequency and the duty cycle of the external 
signal are computed. 
The "Frequency" variable contains the external signal frequency:
External Signal Frequency = SystemCoreClock / TIM2_CCR2 in Hz, 
The "DutyCycle" variable contains the external signal duty cycle:
External Signal DutyCycle = (TIM2_CCR1*100)/(TIM2_CCR2) in %.

The minimum frequency value to measure is 1099 Hz (TIM2 counter clock / CCR MAX).

@par Note

  - For STM32F10x, TIM2 is 16-bit timer (maximum counter clock is 0xFFFF).
  
@par Hardware and Software environment

  - This example runs on STM32F10x Devices.
  
  - This example has been tested with STMicroelectronics STM32NUCLEO Kit (MB1136) Rev C 
    and can be easily tailored to any other supported device and development board.

  - STM32NUCLEO Set-up
    - Connect the external signal to measure to the TIM2 CH2 pin (PA.01). 

@par How to use it? 

In order to make the program work, you must do the following :

 + EWARM
    - Open the TIM_PWMInput.eww workspace 
    - Rebuild all files: Project->Rebuild all
    - Load project image: Project->Debug
    - Run program: Debug->Go(F5)
  
 + MDK-ARM
    - Open the TIM_PWMInput.uvproj project
    - Rebuild all files: Project->Rebuild all target files
    - Load project image: Debug->Start/Stop Debug Session
    - Run program: Debug->Run (F5)  

    
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
