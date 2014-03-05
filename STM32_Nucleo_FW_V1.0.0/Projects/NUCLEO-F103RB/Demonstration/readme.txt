/**
  @page STM32NUCLEO_Demonstration   STM32NUCLEO demonstration firmware
  
  @verbatim
  ******************** (C) COPYRIGHT 2014 STMicroelectronics *******************
  * @file    Demonstration/readme.txt 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    11-February-2014
  * @brief   Description of the STM32NUCLEO demonstration firmware
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

@par Demo Description 
 
The provided demonstration firmware helps you to discover STM32 Cortex-M devices 
that can be plugged on a STM32NUCLEO board. 
If the adafruit 1.8" TFT shield is available, demonstration program will display 
bmp images (using FatFs file system) from SD Card on TFT. If the adafruit 1.8" TFT 
shield is not available, demonstration will simply shows how to toggle an IO 
(PA.05 mapped to LED2) with two different frequencies. please refer to UM1726 
"Getting started with the STM32 Nucleo"  for more details.

Below you find the sequence to discover the demonstration :

The demonstration starts by Checking the availability of adafruit 1.8" TFT 
shield on top of STM32NUCLEO board. This is done by reading the state of IO PB.00 
pin (mapped to JoyStick available on adafruit 1.8" TFT shield). 
If the state of PB.00 is high then the adafruit 1.8" TFT shield is available. 
     

  1- adafruit 1.8" TFT shield is not available:
     LED2 remains toggling with a first frequency equal to ~1Hz. Press User button so
     LED2 will toggle with a second frequency equal to ~5Hz then press User button
     it will toggle with 10Hz frequency. This is done in an infinite loop.
     
  2-  adafruit 1.8" TFT shield is available:
  2-1 SD Card not available:
      A message will be displayed on TFT indicating the missing of an SD Card . 
      Insert one and reset the STM32NUCLEO board.
  
  2-2 SD Card available:
      - A menu will be displayed on TFT. Follow instructions below:
         * Press JoyStick DOWN to continue menu display
         
         * Choose the desired display mode: Press JoyStick DOWN for automatic
           mode or JoyStick UP for manual mode.
           
         * If manual mode is selected, Press JoyStick RIGHT to display next image
           or JoyStick LEFT to display previous one. By pressing JoyStick SEL,
           display mode will switch to automatic.
           If SD Card is unplugged during this mode, a message indicating its 
           absence will be displayed on the first attempt to display next or 
           previous image.
           
         * If automatic mode is selected, images available on SD Card will be 
           displayed sequentially in a forever loop. To switch to manual mode, 
           a long press (~1s) on JoyStick SEL is needed.
           If SD Card is unplugged during this mode, a message indicating its 
           absence will be displayed on the first attempt to display next image.
            
      - If SD Card is not FAT formatted, a message will be displayed on TFT. So 
        a formatting of the SD Card is necessary.
        
      - If the content of the SD Card is other than a bitmap file, a message will 
        be displayed on TFT mentioning that it is not supported. In this case,
        format the SD Card and put into it bmp files with the following criteria:
         * Dimensions: 128x160
         * Width: 128 pixels
         * Height: 160 pixels
         * Bit depth:  16
         * Item type: BMP file
         * The name of the bmp image file must not exceed 11 characters (including
          .bmp extension) and all files must be stored under the root of the uSD.
           
          Note: Ready to use bmp files are available within the FW package under \Media folder                   
 
@par Hardware and Software environment

  - This example runs on STM32F10x Devices.
    
  - This example has been tested with STMicroelectronics STM32NUCLEO board Rev C
    can be easily tailored to any other supported device and development board.      
   
  - Adafruit 1.8" TFT shield must be connected on CN5,CN6, CN8 and CN9 Arduino connectors, 
    for more details please refer to STM32_Nucleo Getting started doc.  

For more details about the adafruit 1.8" TFT shield, please visit: 
http://www.adafruit.com/blog/2012/04/26/new-product-adafruit-1-8-18-bit-color-tft-shield-wmicrosd-and-joystick/


@par How to use it ? 

In order to make the program work, you must do the following :

 + EWARM
    - Open the STM32F103_Nucleo_Demo.eww workspace 
    - Rebuild all files: Project->Rebuild all
    - Load project image: Project->Debug
    - Run program: Debug->Go(F5)

 + MDK-ARM
    - Open the STM32F103_Nucleo_Demo.uvproj project
    - Rebuild all files: Project->Rebuild all target files
    - Load project image: Debug->Start/Stop Debug Session
    - Run program: Debug->Run (F5) 
    
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
