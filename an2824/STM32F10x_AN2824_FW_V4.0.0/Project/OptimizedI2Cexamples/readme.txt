/**
  @page OptimizedI2Cexamples AN2824 Optimized I2C examples Readme File
  
  @verbatim
  ******************** (C) COPYRIGHT 2010 STMicroelectronics *******************
  * @file OptimizedI2Cexamples/readme.txt 
  * @author   MCD Application Team
  * @version  V4.0.0
  * @date     06/18/2010
  * @brief    Description of the STM32 I2C examples using Polling, DMA and IT.
  ******************************************************************************
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  ******************************************************************************
   @endverbatim

@par Description

This software allows the STM32 I2C to operate as master transmitter/receiver 
using Polling or DMA or Interrupt or as slave transmitter/receiver using DMA 
or Interrupt.

If the user wants to perform an I2C communication between two boards based on 
the STM32, he just has to compile the project for slave and then for master. 
The master transmits/receives a buffer of bytes (with prefixed  size by the user), 
into/from the slave.

In the provided routines, direct I2C registers' accesses are performed. The 
purpose is to provide optimized examples. The I2C standard library is used only 
for I2C initialization (mode, clock etc...) 

The communication clock speed is set to 400KHz. You can change it in the I2CRoutines.h.

@par Directory contents 

 - inc: contains the header files
   - Optimized I2C examples/inc/I2Routines.h      Header for I2CRoutines.c
   - Optimized I2C examples/inc/stm32f10x_conf.h  Library Configuration files
   - Optimized I2C examples/inc/stm32f10x_it.h    Interrupt handlers header files
     
 - src: contains the source files 
   - Optimized I2C examples/src/stm32f10x_it.c    Interrupt handlers
   - Optimized I2C examples/src/main.c            Main program
   - Optimized I2C examples/src/I2CRoutines.c     Contains the I2C read and 
     write routines ,I2Cx(1 or 2), GPIOB and  DMA Channels configurations.
   
@par Hardware and Software environment

  - This example runs on STM32F10x Connectivity line, High-Density, Medium-Density, 
    XL-Density, Medium-Density Value line, Low-Density and Low-Density Value line Devices.
  
  - The example using I2C1, has been tested with STMicroelectronics STM32100B-EVAL (Medium-Density
    Value line), STM3210C-EVAL (Connectivity line), STM3210E-EVAL (High-Density and
    XL-Density) and STM3210B-EVAL (Medium-Density) evaluation boards and can be easily
    tailored to any other supported device and development board.
    
    The example using I2C2 can't be tested with STMicroelectronics STM3210C-EVAL (STM32F10x 
    Connectivity-Line) evaluation boards since the I2C2 pins (PB10 and PB11) are 
    already used by Ethernet PHY module. 
    The example using I2C2 can't be tested with STMicroelectronics STM32100B-EVAL (STM32F10x 
    Medium-Density Value line) evaluation boards since the I2C2 pins (PB10 and PB11)
    are already used by HDMI-CEC module.  
  
  - If the user wants to perform an I2C communication between two boards based on 
    the STM32:
    
    - Connect I2Cx SCL pin (PB.06 or PB.10) to the other I2Cx SCL pin (PB.06 or PB.10).
  
    - Connect I2Cx SDA pin (PB.07 or PB.11) to the other I2Cx SDA pin (PB.07 or PB.11).  
 
    - Check that a pull-up resistor is connected on one I2C SDA pin
  
    - Check that a pull-up resistor is connected on one I2C SCL pin

@par How to use it ?  

In order to make the program work, you must do the following:

1. Load the Master code in the Master Board (see below)
2. Load the Slave code in the Slave Board (see below)
3. Run program in Slave Board
4. Run program in Master Board

In order to load the code, you have do the following:

 - EWARM: 
    - Open the Optimized I2C examples.eww workspace
    - In the workspace toolbar select the project config:
        - STM3210B-EVAL: to configure the project for STM32 Medium-density devices
    - Rebuild all files: Project->Rebuild all
    - Load project image: Project->Debug
    - Run program: Debug->Go(F5)

 - RIDE 
    - Open the Optimized I2C examples.rprj project
    - In the configuration toolbar(Project->properties) select the project config:
        - STM3210B-EVAL: to configure the project for STM32 Medium-density devices
    - Rebuild all files: Project->build project
    - Load project image: Debug->start(ctrl+D)
    - Run program: Debug->Run(ctrl+F9)

 - RVMDK 
    - Open the Optimized I2C examples.Uv2 project
    - In the build toolbar select the project config:
        - STM3210B-EVAL: to configure the project for STM32 Medium-density devices
    - Rebuild all files: Project->Rebuild all target files
    - Load project image: Debug->Start/Stop Debug Session
    - Run program: Debug->Run (F5)  

 - HiTOP
    - Open the HiTOP toolchain, a "using projects in HiTOP" window appears.
    - Select open an existing project.
    - Browse to open the Optimized I2C examples.htp:
        - under STM32F10B_EVAL directory: to select the project for STM32 Medium-density devices.
        - under STM32F10C_EVAL directory: to select the project for STM32 Connectivity-Line devices.
        - under STM32F10E_EVAL directory: to select the project for STM32 High-density devices
        - under STM32F10E_EVAL_XL directory: to select the project for STM32 XL-density devices
    - "Download application" window is displayed, click "cancel".
    - Rebuild all files: Project->Rebuild all
    - Click on ok in the "download project" window.
    - Run program: Debug->Go(F5).     
    
 - TrueSTUDIO
    - Open the TrueSTUDIO toolchain.
    - Click on File->Switch Workspace->Other and browse to TrueSTUDIO workspace 
      directory.
    - Click on File->Import, select General->'Existing Projects into Workspace' 
      and then click "Next". 
    - Browse to the TrueSTUDIO workspace directory and select the project:  
        - STM3210B-EVAL: to load the project for STM32 Medium-density devices
        - STM3210E-EVAL: to load the project for STM32 High-density devices
        - STM3210E_EVAL_XL: to load the project for STM32 XL-density devices
    - Under Windows->Preferences->General->Workspace->Linked Resources, add 
      a variable path named "CurPath"  which points to the "FIRMWARE" folder.      
    - Rebuild all project files: Select the project in the "Project explorer" 
      window then click on Project->build project menu.
    - Run program: Select the project in the "Project explorer" window then click 
      Run->Debug (F11)

@note
 - Low-density Value line devices are STM32F100xx microcontrollers where the 
   Flash memory density ranges between 16 and 32 Kbytes.
 - Low-density devices are STM32F101xx, STM32F102xx and STM32F103xx 
   microcontrollers where the Flash memory density ranges between 16 and 32 Kbytes.
 - Medium-density Value line devices are STM32F100xx microcontrollers where
   the Flash memory density ranges between 64 and 128 Kbytes.  
 - Medium-density devices are STM32F101xx, STM32F102xx and STM32F103xx 
   microcontrollers where the Flash memory density ranges between 64 and 128 Kbytes.
 - High-density devices are STM32F101xx and STM32F103xx microcontrollers where
   the Flash memory density ranges between 256 and 512 Kbytes.
 - XL-density devices are STM32F101xx and STM32F103xx microcontrollers where
   the Flash memory density ranges between 512 and 1024 Kbytes.
 - Connectivity line devices are STM32F105xx and STM32F107xx microcontrollers.
   
 * <h3><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h3>
 */
