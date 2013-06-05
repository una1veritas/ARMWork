/*****************************************************************************
 *   readme.txt:  Description of the LPC11Uxx Example Software Package
 *
 *   Copyright(C) 2012, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2012.01.13  ver 1.00    Preliminary version, first Release. 
 *
 *
******************************************************************************/

/*****************************************************************************/
Software that is described herein is for illustrative purposes only which 
provides customers with programming information regarding the products.
This software is supplied "AS IS" without any warranties. NXP Semiconductors 
assumes no responsibility or liability for the use of the software, conveys no 
license or title under any patent, copyright, or mask work right to the 
product. NXP Semiconductors reserves the right to make changes in the 
software without notification. NXP Semiconductors also make no representation 
or warranty that such application will be suitable for the specified use without 
further testing or modification.
/*****************************************************************************/


The Description of the Example software
===================

This example demonstrates the use of build-in peripherals on the NXP
LPC11Uxx family microcontrollers.

The Example software includes, common library, peripheral APIs, and test modules
for the APIs. The common library include startup file, standard definition and
header files, processor specific setup module, generic interrupt related APIs, 
timer routine. The peripheral directories include, ADC, GPIO, timer, SPI, I2C, 
Watchdog timer, UART, external interrupt, various USB class drivers ,etc.
  
The development environment is Keil's, an ARM company now, uVision MDK 4.14 
or later, and ULINK ICE. The target boards is NXP Internal Eval board with 
LPC11xx MCU on it.

Please note that, the latest Keil MDK may or may not include the startup file, 
the header files, and Cortex related API files for NVIC and Systick driver. These 
files are also included in our common library. If the users wish not to use Keil 
MDK, the compiler needs to: 
  (1) set up the include path to COMMON/INC
  (2) change #include <lpc11Uxx.h> to "#include "lpc11Uxx.h" where is located
under COMMON/INC directory. 
  (3) Add "core_cm0.c" from COMMON/SRC into your own project.

The project is created for both target option "FLASH(Release)" and "RAM(Debug)". 
For "FLASH", the image code can be programmed into the flash and executed;
for "RAM", the image code can be loaded into internal SRAM(IRAM)
for easy debugging. The "RAM.ini" in each directory is a Keil uVision debugger
script file, once the debugger starts, the code will be loaded to the IRAM
first, then, "RAM.ini" will be executed and reset program counter(PC) to 
0x10000000(IRAM starting address) that code will be executed from.

For Flash Update, a FLM file for Keil MDK built-n flash utility is included if 
the revision of Keil MDK you have doesn't support LPC11xx yet. If the LPC11xx 
is not listed in the Keil MDK Device Database, choose "Cortex M0" as shown in 
our example project file, then, go to "Options/Utilities/Flash Programming Setting", 
add "LPC11xx/LPC112x/LPC13xx IAP 32kB Flash" from a list of flash algorithm,
and use the Keil Built-in flash utility to program the flash.

Some external components, such as I2C temperature, SPI serial EEPROM, will be 
required to add on the board in order to complete the test below.

Known problems: 
    (1) Keil MDK configuration Wizard has not been tested for Clock configuration.
    (2) When using Flash download, at the end of the flash update, GUI shows
"Error: Flash download failed, Cortex M0". although, the flash has been updated
correctly. So, this message should be ignored. It depends on the revision of the
Keil MDK you use. For MDK with LPC11xx support, this shouldn't be an issue.


The directory tree and content of the sample software
===================

common
    -- inc
	-- lpc11Uxx.h		CMSIS Cortex-M0 Core Peripheral Access Layer Header 
				File for NXP LPC11xx Device Series 
	-- core_cm0.h		CMSIS Cortex-M0 Core Peripheral Access Layer Header File
	-- system_LPC11xx.h	CMSIS Cortex-M0 Device Peripheral Access Layer Header 
				File for the NXP LPC11xx Device Series
	-- type.h		Type definition Header file for NXP LPC11xx Family
	-- systick.h		Cortex M0 Systemtick header and definition 
	-- gpio.h		GPIO setting and I/O pin connfigured as external interrupt
				definition
	-- uart.h		UART setting header and definiton
	-- timer16.h		16-bit TIMER setting header and definition
	-- timer32.h		32-bit TIMER setting header and definition
	-- nmi.h		NMI header and definition
	-- clkconfig.h          clock configuration header and definition  
    -- src
	-- core_cm0.c		CMSIS Cortex-M0 Core Peripheral Access Layer Source File
	-- startup_LPC11Uxx.s	CMSIS Cortex-M0 Core Device Startup File 
           			for the NXP LPC11xx Device Series
	-- system_LPC11Uxx.c	CMSIS Cortex-M0 Device Peripheral Access Layer Source
    				File for the NXP LPC11xx Device Series
	-- gpio.c		GPIO setting APIs and I/O pin connfigured as external 
				interrupt and interrupt handler
	-- uart.c		UART setting API and interrupt handler
	-- timer16.c		16-bit TIMER setting APIs and interrupt handler
	-- timer32.c		32-bit TIMER setting APIs and interrupt handler
	-- nmi.c		NMI APIs and interrupt handler
	-- clkconfig.c		Misc. clock configuration setting and APIs in addition
				to system_lpc11xx.c  

Blinky
    -- blinky.c			General test, GPIO, Timer and NVIC test module, note: 
				API modules are in the COMMON directory and 
				shared and used by some other peripheral testing.
    -- blinky.uvproj		uVision 4.x project file    

ADC
    -- adc.h			ADC header
    -- adc.c			ADC APIs
    -- adctest.c		ADC controller test module
    -- adc.uvproj		uVision 4.x project file


GPIO
    -- gpiotest.c		External interrupt test module, APIs are 
				from COMMON directory
    -- gpio.uvproj		uVision 4.x project file


Systick
    -- systick.c		ARM Cortext M0 Core system tick test module, APIs 
				are from COMMON directory
    -- systick.uvproj		uVision 4.x project file
                
I2C
    -- i2c.h			I2C header
    -- i2c.c			I2C APIs
    -- i2ctest.c		I2C test module
    -- i2ctest.uvproj		uVision 4.x project file

I2CSlave
    -- i2cslave.h		I2C slave header
    -- i2cslave.c		I2C slave APIs
    -- i2cslvtst.c		I2C slave test module
    -- i2cslave.uvproj		uVision 4.x project file

I2CMonitor
    -- i2cmonitor.h		I2C monitor header
    -- i2cmonitor.c		I2C monitor APIs
    -- i2cmontst.c		I2C monitor test module
    -- i2cmonitor.uvproj	uVision 4.x project file

Timer32
    -- timer32test.c		32-bit Timer test module, note: API modules
				are in the COMMON directory and 
				shared and used by some other peripheral testing.
    -- timer32.uvproj		uVision 4.x project file

PWM
    -- pwmtest.c		PWM test module, note: PWM API modules
				are in the COMMON directory, shared and used 
				by some other peripheral testing.
    -- pwm.uvproj		uVision 4.x project file

UART
    -- uarttest.c		UART test module, note: UART API modules
				are in the COMMON directory, shared and used 
				by some other peripheral testing.
    -- uart.uvproj		uVision 4.x project file

RS485
    -- rs485.h			RS485 header
    -- rs485.c			RS485 APIs
    -- rs485test.c		RS485 test module
    -- rs485.uvproj		uVision 4.x project file

WDT
    -- wdt.h			Watchdog timer header
    -- wdt.c			Watchdog timer APIs
    -- wdttest.c		Watchdog timer test module
    -- wdt.uvproj		uVision 4.x project file

USBDriver
    These files are shared by all the USB class drivers.
    -- usb.h			USB header
    -- usbhw.h			USB hardware header
    -- usbhw.c			USB hardware APIs
    -- usbcore.h		USB Core header
    -- usbcore.c		USB Core APIs

USBHID
    -- hid.h			USB HID related header
    -- hiduser.h		USB HID user specific header
    -- hiduser.c		USB HID user APIs
    -- usbcfg.h			USB configuration header
    -- usbdesc.h		USB descriptor header
    -- usbdesc.c		USB descriptor APIs
    -- usbuser.h		USB user header
    -- useuser.c		USB user APIs
    -- io_event.h		USB Remote Wakeup test header
    -- io_event.c		USB Remote wakeup test APIs
    -- demo.c			USB HID test module main entry
    -- demo.h                   header file for main entry
    -- usbhid.uvproj		uVision 4.x project file
	
USBMEM
    -- msc.h			USB MSC related header
    -- mscuser.h		USB MSC user specific header
    -- mscuser.c		USB MSC user APIs
    -- usbcfg.h			USB configuration header
    -- usbdesc.h		USB descriptor header
    -- usbdesc.c		USB descriptor APIs
    -- usbuser.h		USB user header
    -- useuser.c		USB user APIs
    -- diskimg.c                USB Virtual file page
    -- memory.c			USB MSC test module main entry
    -- memory.h                 header file for main entry
    -- usbmem.uvproj		uVision 4.x project file

USBAudio
    -- usbaudio.h		USB Audio related header
    -- adcuser.h		USB Audio user specific header
    -- adcuser.c		USB Audio user APIs
    -- usbcfg.h			USB configuration header
    -- usbdesc.h		USB descriptor header
    -- usbdesc.c		USB descriptor APIs
    -- usbuser.h		USB user header
    -- useuser.c		USB user APIs
    -- usbdmain.c		USB MSC test module main entry
    -- usbdmain.h		header file for main entry
    -- usbaudio.uvproj		uVision 4.x project file

USBCDC   
    -- cdc.h			USB CDC related header
    -- cdcuser.h		USB CDC Device Class user specific header
    -- cdcuser.c		USB CDC Device Class user APIs
    -- usbcfg.h			USB configuration header
    -- usbdesc.h		USB descriptor header
    -- usbdesc.c		USB descriptor APIs
    -- usbuser.h		USB user header
    -- usbuser.c		USB user APIs
    -- vcomdemo.h		header for VCOM demo
    -- vcomdemo.c		USB CDC(Virtual COM) test module, main entry
    -- virtualCom.uvproj	uVision 4.x project file
    -- serial.h			Header for UART port
    -- serial.c			UART module APIs
    -- lpc11xx-vcom.inf		The host side driver installation file
	

