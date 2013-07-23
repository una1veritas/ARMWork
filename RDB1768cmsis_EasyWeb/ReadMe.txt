The RDB1768cmsis_EasyWeb is distributed with LPCXpresso. It operates on the
LPC17xx Xpresso board and Base Board with few changes.

The header tcpip.h defines several symbols used to assign the IP address. This is
normally displayed on the RDB's LCD display, which is not present on the Base Board.
This file should be changed to have the correct network settings such as IP address
and subnet mask before the project is compiled and run.

RDB1768cmsis_EasyWeb depends upon RDB1768cmsis_LCDlib which can also be found in 
the example project archive distributed with LPCXPresso. It will compile and run 
on the LPCXpresso1768 board, however the Base Board does not have the same
graphical display as the RDB board, and as such this libray will not
actually display any graphics when used on the baseboard.

Location of RDB1768cmsis_EasyWeb and RDB1768cmsis_LCDlib : 
C:\nxp\lpcxpresso\Examples\LPC1000\LPC17xx

To download LPCXpresso: http://lpcxpresso.code-red-tech.com/LPCXpresso/