Purspose:
To implement USB CDC-Virtual COM port using the on-chip USB ROM Drivers on                                                                             
NXP LPC11U2x and above Microcontrollers

It demonstrates the USB Virtual COM port based on a Windows USB host 
driver 

The PC will install a virtual COM port on the PC (see Driver Installation).
After installation an additional port "LPC13xx USB VCom Port(COMx)"
can be found under System/Hardware/Device Manager/Ports(COM&LPT).
Number "x" is not fixed as different PC configuration may have different 
"x" displayed on the device manager. The USB host driver assigns "x" 
dynamically based on the existing COM port configuration of the system.
   
Testing the USB Virtual COM port with serial cable:

  Open two Hyperterminal windows. 
      One with "LPC11Uxx USB VCom Port(COMx)" 
      One with "Communications Port (COM1)".
  Connect PC port COM1 to the comport on the board and open "COM1" 
  and "COMx". Data from COM1 will be echoed on "COMx" and visa versa. 
  So, this is bi-directional communication between the physical COM
  port 1 on the board and the virtual COM port COMx on host PC.    

The Virtual COM Port program is available for the following targets:
  NXP LPC11Uxx : configured for on-chip Flash ROM
           (used for production or target debugging)

Driver Installation:
--------------------
     "Welcome to the Found New Hardware Wizard" appears
     - select 'No, not this time'
     - press  'Next'

     - select 'Install from a list or specific location (Advanced)'
     - press  'Next'

     - select 'Search for the best driver in these locations'
     - check  'include this location in the serach'
     - set to <project folder>
     - press 'Next'


     "Hardware Installation" appears
     "has not passed Windows Logo testing..."
     - press 'Continue Anyway'

     "Completing the Found New Hardware Wizard" appears
     - press 'Finish'


Note:
This has been tested on NGX LPC11U24 board