Simple demo project for Olimex STM32-E407 board
In order to work you must program with J-Link 7.0 and IAR for ARM 6.30 or newer and connect USB Serial Cable to the RX3, TX3 and GND pins on the board connector named "BOOT". Open terminal program with following configuration:
BaudRate:	115200
DataBits:	8
StopBits:	1
Parity:		None
You should also check in the device manager the number of the Comm Port where the USB serial cable is connected.

The workspace is located:
\Demo\Project\Test\EWARM\Project.eww

Description
0) RTC Demo:	On the terminal will be shown a timer that counts time (format - "hh:mm:ss") each second. Press space to interrupt this demo and then push the button for the next demo.
1) LED demo:	The LED will flash three times before the next demo.
2) Button demo:	Push and hold the button to flash the LED again. Push the button for the next demo.
3) LAN demo:	Connect a LAN cable to the ethernet connector and press space. Then open a browser and type 192.168.0.100. There is a simple web page. Press space again to finish it and the button for the next demo.
4) OTG1/2 demo:	These two demos are performed just once. (If you want to repeat it you must reset the board). Connect a USB B mini cable to the proper USB connector and press the button. The mouse will start to move describing a square. With another push of the button you move to the next demo.
5) SD card demo:Insert a SD Card into the slot and it will show you its size. Alternatively you can skip it if you press space. Press the button again.
Now you will be asked if you want to run the demos again (without the two of the USB connectors). Press Y to repeat or N otherwise. In second case the LED will blink in an infinity loop.