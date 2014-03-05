/**
  @page BASIC_EEPROM BASIC_EEPROM
  
  @verbatim
  ******************** (C) COPYRIGHT 2011 STMicroelectronics *******************
  * @file    Project/STM32_CPAL_Examples/STM32_CPAL_I2C/Basic_EEPROM/readme.txt 
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    21-December-2012
  * @brief   Description of the CPAL Basic EEPROM communication example.
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

This example shows how to use the EEPROM driver using the CPAL library.

It provides a basic example of how to use the CPAL firmware library and
an associate I2C EEPROM driver to manage I2C EEPROM memories.
This example could be easily tailored to any similar I2C memory by configuring 
EEPROM parameters (Address, size, page size) in main.h file.

** Hardware Description :

I2C peripheral is configured in Master transmitter during write operation and in
Master receiver during read operation from I2C EEPROM. 


The I2C peripheral is selected depending on STM32xxxx-EVAL using the evaluation board
precompiler define. 
The speed is set to 300 KHz and can be configured into the range of 10 KHz - 400KHz
using the define "I2C_SPEED" in stm32_eval_i2c_ee_cpal.h file. 
Note: in order to reach 400 KHz speed, system clock should be configured to
provide APB frequency multiple of 10MHz for the I2C peripheral. The pull-up resistors
should also be suitable for this range. Refer to I2C protocol specification and STM32
Reference Manual for more details.

*------------------------------------------------------------------------------*
|         ST Evaluation Board                                                  |
|         ____________________                                                 |
|        |                    |                                                |
|        |                    |                                                | 
|        |     __________     |                                                |
|        |    |  STM32   |    |             You can eventually connect to      |
|        |    |__________|    |             an external EEPROM memory using    |
|        |        |  |        |             I2C_SCL, I2C_SDA and Gnd lines.    |
|        |   SCL  |  | SDA    |             In this case, the Example may be   |
|        |     ___|__|____    |             tailored to the selected EEPROM    |
|        |    |I2C EEPROM |   |                                                |
|        |    |___________|   |             Note: No EEPROM is available on     |
|        |      _  _  _  _    |              STM32L152-EVAL evaluation boards  |
|        |     |_||_||_||_|   |                                                |
|        |         LEDs       |                                                |
|        | ___________________|                                                |
|                                                                              |
|                                                                              |
*------------------------------------------------------------------------------*

For M24C64 devices all the memory is accessible through the two-bytes 
addressing mode . In this case, only the physical address has to be defined 
(according to the address pins (E0,E1 and E2) connection).
This address is defined and affected to sEEAddress parameter of sEEx_DevStructure 
(M24C64 address is 0xA0: E0, E1 and E2 tied to ground). 

The EEPROM memory addresses where the program start the write and the read operations 
is defined in the main.c file. 

** Software Description :

First, the content of Tx1_Buffer is written to the EEPROM_WRITE_ADDR1 and the
written data are read. The written and the read buffers data are then compared.
Following the read operation, the program waits that the EEPROM reverts to its 
Standby state. A second write operation is, then, performed and this time, Tx2_Buffer
is written to EEPROM_WRITE_ADDR2, which represents the address just after the last 
written one in the first write. After completion of the second write operation, the 
written data are read. The contents of the written and the read buffers are compared.

Transfer results are monitored with LEDs:

     *- If Transfer1 is complete successfully LED1 is turned on. 
        Else LED3 is turned on.
        
     *- If Transfer2 is complete successfully LED2 is turned on . 
        Else LED3 is turned on.   

A periodic high priority interrupt (generated by a Timer) is managing LED4 toggling 
during Communication.

       
** UserCallbacks :
   - CPAL_TIMEOUT_UserCallback : This Callback is called to manage timeout error.
      At first start by reinitializing I2C Device. After that change EEPROM state 
      to sEE_STATE_ERROR.
      
   - CPAL_I2C_ERR_UserCallback : This Callback is called to manage Device error .  
      At first start by reinitializing I2C Device.  After that change EEPROM state 
      to sEE_STATE_ERROR.
      Note: 
      If you want to generate an I2C error, you can remove the Evaluation board
      I2C SCL jumper if it exist (ie. JP09 on the STM3210C-EVAL evaluation board 
      or JP24 on the STM322xG-EVAL and STM324xG-EVALevaluation board).
      In this case the I2C communication will be corrupted and the CPAL I2C error callbacks
      will be entered. If you fit back the jumper, the communication should be recovered 
      immediately thanks to CPAL error management.
      Note:
      Removing the evaluation board EEPROM WP jumper (ie. JP14 on STM32100E-EVAL evaluation
      board) will cause all write operations to be corrupted, while read operation can
      still be managed correctly.
      
   - CPAL_I2C_TXTC_UserCallback : This Callback is called when Write operation is completed.
      In this Callback sEE_WriteHandler() function is called to handle Communication with EEPROM
      when its state is equal to sEE_STATE_WRITING .
      
   - CPAL_I2C_RXTC_UserCallback : This Callback is called when Read operation is completed.
      In this Callback sEE_ReadHandler() function is called to handle Communication with EEPROM
      when its state is equal to sEE_STATE_READING.

@par Directory contents 

  - Basic_EEPROM/system_stm32xxxx.c   STM32xxxx devices system source file (xxxx can be : f10x or l1xx or f2xx or f40x)
  - Basic_EEPROM/stm32xxxx_conf.h     Library Configuration file for STM32xxxx devices (xxxx can be : f10x or l1xx or f2xx or f40x)
  - Basic_EEPROM/stm32_it.c           Interrupt handlers
  - Basic_EEPROM/stm32_it.h           Interrupt handlers header file  
  - Basic_EEPROM/main.c               Main program
  - Basic_EEPROM/main.h               Header file for main.c    
  - Basic_EEPROM/cpal_conf.h          CPAL Library Configuration file
  - Basic_EEPROM/cpal_usercallback.c  CPAL Usercallbacks definition  


@par Hardware and Software environment 

  - This example runs on STM32F10x Connectivity line, High-Density, Medium-Density, 
    XL-Density, Medium-Density Value line, Low-Density, Low-Density Value line Devices,
    STM32L1xx Ultra Low Power Medium-Density, High-Density Devices and STM32F2xx Devices.
  
  - This example has been tested with STMicroelectronics STM3210C-EVAL RevB
    (Connectivity line), STM32100E-EVAL RevB (High-Density Value line), 
    STM32L152-EVAL RevB (Ultra Low Power Medium-Density),STM32L152D-EVAL
    (Ultra Low Power High-Density) STM322xG-EVAL RevB and STM324xG-EVAL RevA 
    evaluation boards and can be easily tailored to any other supported device 
    and development board.
       
  - STM3210C-EVAL Set-up 
    - Make sure the Jumper JP09 "I2C_SCK" is fitted on the board.
    - Make sure the Jumper JP17 "ROM_WP" is fitted on the board.
    
  - STM32100E-EVAL Set-up   
    - Make sure the Jumper JP14 "E2P_W" is fitted on the board.
    
  - STM32L152-EVAL Set-up
    - There is no EEPROM available on the STM32L152-EVAL evaluation board.
      To run this example, you can connect an external EEPROM to the board through
      the following pins:
       - Connect the pin PB6 to the EEPROM I2C_SCL pin
       - Connect the pin PB7 to the EEPROM I2C_SDA pin
       - Connect the Gnd and Vdd of the EEPROM to the board's Gnd and Vdd 
       - Make sure that the selected EEPROM support the board's voltage level.
       - Configure the EEPROM address and Page size using the defines 
         EEPROM_ADDRESS and EEPROM_PAGE_SIZE in the file main.h
         
  - STM32L152D-EVAL Set-up
    - Make sure that M24LR64 is inserted in CN12 connector.
    - Please refer to cpal_i2c_hal_stm32l1xx.h file and configure I2Cx pins remap
      as follow SCL => PB8 and SDA => PB9.
         
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
        - STM3210C-EVAL: to configure the project for STM32 Connectivity Line devices.
        - STM32100E-EVAL: to configure the project for STM32 High-Density Value line devices.
        - STM32L152-EVAL: to configure the project for STM32 Ultra Low Power Medium-density devices.
        - STM32L152D-EVAL: to configure the project for STM32 Ultra Low Power High-density devices.
        - STM322xG_EVAL: to configure the project for STM32F2xx devices
        - STM324xG_EVAL: to configure the project for STM32F4xx devices
    - Rebuild all files: Project->Rebuild all
    - Load project image: Project->Debug
    - Run program: Debug->Go(F5)
 + MDK-ARM
    - Open the Project.uvproj workspace.
    - In the workspace toolbar select the project config:
        - STM3210C-EVAL: to configure the project for STM32 Connectivity Line devices.
        - STM32100E-EVAL: to configure the project for STM32 High-Density Value line devices.
        - STM32L152-EVAL: to configure the project for STM32 Ultra Low Power Medium-density devices.
        - STM32L152D-EVAL: to configure the project for STM32 Ultra Low Power High-density devices.        
        - STM322xG_EVAL: to configure the project for STM32F2xx devices
        - STM324xG_EVAL: to configure the project for STM32F4xx devices
    - Rebuild all files: Project->Rebuild all target files
    - Load project image: Debug->Start/Stop Debug session
    - Run program: Debug->Run(F5)

 + TrueSTUDIO
    - Open the TrueSTUDIO toolchain.
    - Click on File->Switch Workspace->Other and browse to TrueSTUDIO workspace directory.
    - Click on File->Import, select General->'Existing Projects into Workspace'
      and then click "Next". 
    - Browse to the TrueSTUDIO workspace directory and select the project:  
        - STM3210C-EVAL: to configure the project for STM32 Connectivity Line devices.
        - STM32100E-EVAL: to configure the project for STM32 High-Density Value line devices.
        - STM32L152-EVAL: to configure the project for STM32 Ultra Low Power Medium-density devices.
        - STM32L152D-EVAL: to configure the project for STM32 Ultra Low Power High-density devices.        
        - STM322xG_EVAL: to configure the project for STM32F2xx devices
        - STM324xG_EVAL: to configure the project for STM32F4xx devices
    - Under Windows->Preferences->General->Workspace->Linked Resources, add 
      a variable path named "CurPath" which points to the folder containing
      "Libraries", "Project" and "Utilities" folders.
    - Rebuild all project files: Select the project in the "Project explorer" window
      then click on Project->build project menu.
    - Run program: Select the project in the "Project explorer" window then click 
      Run->Debug (F11)
      
 * <h3><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h3>
 */
