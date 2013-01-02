* Ethernet library for Maple (STM32 micros board) *

This library is inhrerited from the Arduino 1.0 Ethernet library.
It can be used w5200 chip or w5100 chip on MapleIDE and libmaple. 
And it has been fixed UDP recvfrom function bug. 

Inherited and modified by dinau, 2012/02.


Usage:
 1. Copy Ethernet folder into libraries folder in MapleIDE.
 2. Edit Ethernet/utility/w5100_config.h to configure SPI pin setup.
 3. Edit Ethernet/utility/w5100.h, about line 17, to select driver chip.
    Default setting is using w5100 chip.
	If you want to use w5200 chip, uncomment line 17.




License:
 This library is under the original license of the Arduino Ethernet library. 




Reference links:
LeafLabs' Maple
http://leaflabs.com

issue 669: 	Ethernet/UDP libraries loses packets 
if there are multiple packets in w5100 buffer
http://code.google.com/p/arduino/issues/detail?id=669

blog
http://avr.paslog.jp/article/2379948.html
http://avr.paslog.jp/article/2361305.html
