readme.txt file for glcd armcmx library
Ported Arduino glcd library v3
to ARM Cortex M4 (tested on STM32F4 Discovery) with armcmx lib

D/I PE7
Bus PE8 -- 15 // base pin can be moved but 8 bits must be continuous
RW  PD4
EN  PD5
CS1 PD6
CS2 PD7

Font drawing routine will be modified, since font data structure is
not general and complicated to transform from others, for me.

It is still just a private trial.
LICENCE, Term of use inherits the original's:

The Arduino GLCD library files are either licensed under the terms of the
GNU Lesser General Public License or are compatible with this license when
the GLCD library code is used in the Arduino software environment.
See licence.txt for details.

The glcd library was created by Michael Margolis. Extensive enhancements 
including the majority of the improvements in this release were led
by Bill Perry.

 
