gpio
=====================
This project contains a gpio example for the LPCXpresso board
mounted with an LPC1343 Cortex-M3 part. It configures interrupts on
bit 1 of ports 0, 1, 2, and 3. The interrupt will vector to the
default interrupt handler which does nothing.

When downloaded to the board and executed, this example can be stepped
through to see how to configure I/O ports and interrupts.

The project makes use of code from the following library project:
- CMSISv1p30_LPC13xx : for CMSIS 1.30 files relevant to LPC13xx

This library project must exist in the same workspace in order
for the project to successfully build.
