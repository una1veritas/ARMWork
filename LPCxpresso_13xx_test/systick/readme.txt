systick
======================
This project contains a LED flashing Systick example for the LPCXpresso
board mounted with an LPC1343 Cortex-M3 part.

When downloaded to the board and executed, LED2 will be illuminated.
The state of LED2 will toggle every 2 seconds, timed using the Cortex-M3's
built in Systick timer. Note: The Cortex systick timer will not operate in
sleep mode, however the 16 and 32-bit timer peripherals will.

The project makes use of code from the following library project:
- CMSISv1p30_LPC13xx : for CMSIS 1.30 files relevant to LPC13xx

This library project must exist in the same workspace in order
for the project to successfully build.

For more details, read the comments in config.h
