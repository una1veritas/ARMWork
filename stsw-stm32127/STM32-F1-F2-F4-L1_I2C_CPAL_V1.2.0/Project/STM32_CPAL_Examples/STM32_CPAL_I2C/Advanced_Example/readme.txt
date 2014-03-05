/**
  @page Advanced_Example Advanced_Example
  
  @verbatim
  ******************** (C) COPYRIGHT 2011 STMicroelectronics *******************
  * @file    Project/STM32_CPAL_Examples/STM32_CPAL_I2C/Advanced_Example/readme.txt 
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    21-December-2012
  * @brief   Description of the CPAL Advanced communication example.
  ******************************************************************************
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  ******************************************************************************
   @endverbatim

@par Example Description 

This example shows how to use the CPAL library to communicate with different external
devices at the same time and in synchronous and asynchronous ways.

** Hardware Description :

 I2C device, IOExpander, TempSensor and EEPROM are connected on the same Bus. I2C device 
 is set as master and will perform Read and write operation with slave components 
 (IOExpander, TempSensor and EEPROM).
 
*-----------------------------------------------------------------------------* 
|                              _____________  __________  __________          |
|                             | TouchScreen || Joystick ||TempSensor|         |
|                             |_____________||__________||__________|         |
|                                        |       |        |                   |   
|                                       _|_______|________|_                  |
|                                      |     IOExpander     |                 |
|                                      |____________________|                 |
|                                               | |                           |
|                                               | |                           |
|       ____________                            | |                           |
|      |            |___SCL________ ____________|_|________ ___               |
|      |   STM32    |____________ _|____________|________ _|___               |
|      |            |   SDA      | |                     | |                  |
|      |____________|         ___|_|____              ___|_|____              |
|                            |  EEPROM  |            |TempSensor|             |
|                            |__________|            |__________|             |
*-----------------------------------------------------------------------------*

 - TempSensor (through IOEpander when using STM3210C-EVAL, STM322xG-EVAL and STM324xG-EVAL 
   and through LM75 device when using STM32100E-EVAL): Periodically (using a timer interrupt)
   the temperature Sensor is read and the temperature value is stored into the EEPROM memory.  
   For each temperature value, a time value is associated (using RTC peripheral) 
   and stored at the same level in order to generate a formated reports .
     
 - Touch Screen control (through IO Expander): controls a touch zone displaying 
   four rectangles. For each rectangle, when it is touched a message is 
   displayed on the LCD screen.

 - Joystick control (through IO Expander): controls the menu navigation in asynchronous 
   way.
 
 - EEPROM : The values of temperature gathered from the Temperature Sensor module 
  are stored into the EEPROM memory. User may read the historic of stored values.
  Note : User can run this example with any type of EEPROM and this can be done 
  by configuring EEPROM parameters (Address, size, page size) in main.h file.
    

** Software Description :

 The example performs the following actions:
  - Configure I2C device to communicate with EEPROM, IOExpander and TempSensor.
  - Configure RTC timer to handle real time in seconds/minutes/hours format.
  - Configure a Timer to generate periodic high priority interrupt.
 
  - At each periodic interrupt, the following actions are performed:
      - The time and temperature are read from RTC and Temperature sensor then 
        saved in a formated report into the EEPROM memory.
      - Each period of time, two measures are done and saved into a single report.

 - Example manages also asynchronous events:
      - When Joystick is pushed (controlled through IOExpander): the button value
         is read through I2C and displayed on LCD screen allowing to navigate through
         different menus.
      - When a touch screen event occur on the defined rectangles area: the touch
         record is read and the name of the rectangle is displayed on LCD screen.
      - In the main menu:
        - When Up button pushed: the values of reports are displayed and a menu is
          managing the report display (Right/Left for next/previous measure, Sel 
          for next report).
        - When Right button is pushed: Current Temperature value is read and displayed
        - When Left button is pushed: Current time value is read and displayed
        - When Down button is pushed: the EEPROM memory is erased (write all the memory
          with zeros).
          
 - In Demo startup if RTC is not configured Time Config menu is shown. you have the choice 
   to set or not the time. Press key button to ignore the prompt and abort the configuration 
   sequence. Press on SEL and set the time.
   
   To adjust the time:
     -  Modify the first digit of the hour field, use the UP and DOWN push-buttons. 
        Press UP to display the current value plus one. Press DOWN to display the 
        previous digit value.
        
     - After setting the digit value, press SEL. The cursor automatically jumps to the next digit.
        
   Some digit values are limited to a range of values depending on the field (hour, 
   minutes or seconds). When all the time digits have been set, the Time Config menu disappears. 


** UserCallbacks :

   - CPAL_TIMEOUT_UserCallback : This Callback is called to manage timeout error.
      At first start by Generating Stop Condition to close communication in order 
      to release SCL and SDA Lines then reinitialize I2C Device. After that if EEPROM 
      state is sEE_STATE_WRITING or sEE_STATE_READING change this state to sEE_STATE_ERROR.
      
   - CPAL_I2C_ERR_UserCallback : This Callback is called to manage Device error .  
      At first start by Generating Stop Condition to close communication in order 
      to release SCL and SDA Lines then reinitialize I2C Device. After that if EEPROM 
      state is sEE_STATE_WRITING or sEE_STATE_READING change this state to sEE_STATE_ERROR.
      Note: 
      If you want to generate an I2C error, you can remove the Evaluation board
      I2C SCL jumper if it exist (ie. JP09 on the STM3210C-EVAL evaluation board).
      In this case the I2C communication will be corrupted and the CPAL I2C error callbacks
      will be entered. If you fit back the jumper, the communication should be recovered 
      immediately thanks to CPAL error management.
      Note:
      Removing the evaluation board EEPROM WP jumper (ie. JP14 on STM32100E-EVAL evaluation
      board)will cause all write operations to be corrupted, while read operation can
      still be managed correctly.
      
   - CPAL_I2C_TXTC_UserCallback : This Callback is called when Write operation is completed.
      In this Callback sEE_WriteHandler() function is called to handle Communication with EEPROM
      when its state is equal to sEE_STATE_WRITING .
      
   - CPAL_I2C_RXTC_UserCallback : This Callback is called when Read operation is completed.
      In this Callback sEE_ReadHandler() function is called to handle Communication with EEPROM
      when its state is equal to sEE_STATE_READING.      

** Known Limitations :
  - When the Touchscreen zone and the Joystick push buttons are solicited at the
    same time it may happen that the IOExpander stops functioning correctly (both
    Touchscreen and Joystick stop reacting). In this case, push the Key button and
    the error should be recovered. 
  
@par Directory contents 

  - Advanced_Example/system_stm32xxxx.c   STM32xxxx devices system source file (xxxx can be : f10x or l1xx or f2xx or f40x)
  - Advanced_Example/stm32xxxx_conf.h     Library Configuration file for STM32xxxx devices (xxxx can be : f10x or l1xx or f2xx or f40x)
  - Advanced_Example/stm32_it.c           Interrupt handlers
  - Advanced_Example/stm32_it.h           Interrupt handlers header file
  - Advanced_Example/main.c               Main program
  - Advanced_Example/main.h               Header file for main.c  
  - Advanced_Example/demo.c               Menus level functions
  - Advanced_Example/demo.h               Header file for the demo.c  
  - Advanced_Example/cpal_conf.h          CPAL Library Configuration file
  - Advanced_Example/cpal_usercallback.c  CPAL Usercallbacks definition


@par Hardware and Software environment 

  - This example runs on STM32F10x Connectivity line, High-Density, Medium-Density, 
    XL-Density, Medium-Density Value line, Low-Density, Low-Density Value line Devices,
    STM32L1xx Ultra Low Power Medium-Density, High-Density Devices and STM32F2xx Devices.
  
  - This example has been tested with STMicroelectronics STM3210C-EVAL RevB
    (Connectivity line), STM32100E-EVAL RevB (High-Density Value line), 
    STM322xG-EVAL board RevB and STM324xG-EVAL board RevA evaluation boards 
    and can be easily tailored to any other supported device and development board.
   
  - STM3210C-EVAL Set-up 
    - Make sure the Jumper JP09 "I2C_SCK" is fitted on the board.
    - Make sure the Jumper JP17 "ROM_WP" is fitted on the board.
    
  - STM32100E-EVAL Set-up 
    - Make sure the Jumper JP14 "E2P_W" is fitted on the board.
    - Make sure that the same I2C speed is configured for both EEPROM driver and 
      Temperature Sensor driver (through the define I2C_SPEED).

  - STM322xG-EVAL Set-up 
    - Make sure the Jumper JP24 "E2P_W" is fitted on the board.

  - STM324xG-EVAL Set-up 
    - Make sure the Jumper JP24 "E2P_W" is fitted on the board.

Note : Used I2Cx pins remap differs from EVAL-BOARD to another. Please refer to 
       cpal_i2c_hal_stm32xxxx.h file and configure I2Cx pins remap.
       
@par How to use it ? 
 + EWARM
    - Open the Project.eww workspace.
    - In the workspace toolbar select the project config:
        - STM3210C-EVAL: to configure the project for STM32 Connectivity Line devices
        - STM32100E-EVAL: to configure the project for STM32 High-Density Value line devices
        - STM322xG_EVAL: to configure the project for STM32F2xx devices
        - STM324xG_EVAL: to configure the project for STM32F40x devices
    - Rebuild all files: Project->Rebuild all
    - Load project image: Project->Debug
    - Run program: Debug->Go(F5)

 + MDK-ARM
    - Open the Project.uvproj workspace.
    - In the workspace toolbar select the project config:
        - STM3210C-EVAL: to configure the project for STM32 Connectivity Line devices
        - STM32100E-EVAL: to configure the project for STM32 High-Density Value line devices
        - STM322xG_EVAL: to configure the project for STM32F2xx devices
        - STM324xG_EVAL: to configure the project for STM32F40x devices        
    - Rebuild all files: Project>Rebuild all target files
    - Load project image: Debug->Start/Stop Debug session
    - Run program: Debug->Run(F5)

 + TrueSTUDIO
    - Open the TrueSTUDIO toolchain.
    - Click on File->Switch Workspace->Other and browse to TrueSTUDIO workspace directory.
    - Click on File->Import, select General->'Existing Projects into Workspace'
      and then click "Next". 
    - Browse to the TrueSTUDIO workspace directory and select the project:  
        - STM3210C-EVAL: to configure the project for STM32 Connectivity Line devices
        - STM32100E-EVAL: to configure the project for STM32 High-Density Value line devices
        - STM322xG_EVAL: to configure the project for STM32F2xx devices
        - STM324xG_EVAL: to configure the project for STM32F40x devices
    - Under Windows->Preferences->General->Workspace->Linked Resources, add 
      a variable path named "CurPath" which points to the folder containing
      "Libraries", "Project" and "Utilities" folders.
    - Rebuild all project files: Select the project in the "Project explorer" window
      then click on Project->build project menu.
    - Run program: Select the project in the "Project explorer" window then click 
      Run->Debug (F11)
   
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
