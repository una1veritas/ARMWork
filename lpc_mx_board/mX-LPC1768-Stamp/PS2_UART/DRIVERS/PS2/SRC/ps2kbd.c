#include "lpc17xx.h"
#include "type.h"
#include "extint.h"
#include "ps2kbd.h"
#include "scancodes.h"


uint8_t	keyBuf[KBUFFSIZE];	 //PS2 Keyboard buffer, the register to store characters key in
uint8_t	keyIn;			     //Index into PS/2 key buf where next scan code will be inserted
uint8_t	keyOut;			     //Index into PS/2 key buf where next scan code will be removed
uint8_t	keyRead;		     //Number of keys read from the PS/2 keyboard
uint8_t bitcount=0;             // 0 = neg.  1 = pos.

#define kbDAT (LPC_GPIO2->FIOPIN & (KB_DAT))
#define kbCLK (LPC_GPIO2->FIOPIN & (KB_CLK))
#define K_STARTBIT    1			
#define K_PARITYBIT  10			
#define K_STOPBIT    11			


/*****************************************************************************
** Function name:		EINT3_Handler
**
** Descriptions:		external INT handler
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void EINT3_IRQHandler (void) 
{

  static uint8_t key_data;          // Holds the received scan code
  uint32_t clkstat;
  uint32_t datstat;
  volatile uint32_t paritystat;
 
  clkstat = kbCLK;             //check CLK pin state;
  datstat = kbDAT;             //check DAT pin state;

  bitcount++;
  
  if (bitcount==K_STARTBIT)
  {
      if (datstat || clkstat)	bitcount=0;
      key_data=0;
      paritystat=0; 
  }
  else if (bitcount==K_PARITYBIT)
  {
      paritystat = datstat;
  }
  else if (bitcount==K_STOPBIT)
  {   
     decode (key_data);
      bitcount=0;         
  }
  else
  {
      // For all bits from 2, 3...9 (total 8-bit)
      key_data= key_data >> 1;
      if (datstat)
      {
  	    key_data = key_data | 0x80;
      }
   }

  LPC_SC->EXTINT = EINT3;		/* clear interrupt */  		
}

/**
*****************************************************************************
	Function Name : init_keyboard()

	Description   :

	Input         :  

	Output        : 

	Note          :
******************************************************************************
*/
uint8_t init_keyboard(void)
{
	bitcount = 0;
  	keyIn =0;
  	keyOut=0;
  	keyRead=0;
#if 0
	  //PINSEL0 = (PINSEL0 & ~(3U << 30)) | (1U << 31);
	LPC_PINCON->PINSEL3 &= ~(0x00000003);
	LPC_GPIO2->FIOCLR = KB_CLK;             //hold keyboard clock line untill ready
	LPC_GPIO1->FIODIR &= ~(KB_DAT);

	LPC_PINCON->PINSEL4 &= ~(0x0C000000);
	LPC_PINCON->PINSEL4 |= (0x04000000);	/* set P2.13 as EINT1 and  */  
	LPC_GPIOINT->IO2IntEnF |= 0x2000;	/* Port2.13 is falling edge. */
	LPC_SC->EXTMODE = EINT3_EDGE;		/* INT3 edge trigger */
	LPC_SC->EXTPOLAR |= 0;				/* INT3 is falling edge by default */
	LPC_SC->EXTINT = EINT3;		/* clear interrupt */
	NVIC_EnableIRQ(EINT3_IRQn);
#endif
	LPC_PINCON->PINSEL4 &= ~(0x0C000000);  // KB_CLK p2.13
	LPC_PINCON->PINSEL4 &= ~(0x00000300);  // KB_DAT p2.4

	LPC_GPIO2->FIOCLR = KB_CLK;             //hold keyboard clock line untill ready
	LPC_GPIO2->FIODIR &= ~(KB_DAT);

	//LPC_PINCON->PINSEL4 &= ~(0x0C000000);	 dee
	//LPC_PINCON->PINSEL4 |= (0x04000000);	/* set P2.13 as EINT3 and  */  	 dee
	LPC_PINCON->PINSEL4 = (LPC_PINCON->PINSEL4 & ~(3U << 27)) | (1U << 26);
	//LPC_GPIOINT->IO2IntEnF |= 0x00002000;	/* Port2.13 is falling edge. */	 dee
	LPC_SC->EXTMODE = EINT3_EDGE;		/* INT3 edge trigger */
	LPC_SC->EXTPOLAR |= 0;				/* INT3 is falling edge by default */
	LPC_SC->EXTINT = EINT3;		/* clear interrupt */

	NVIC_EnableIRQ(EINT3_IRQn);
	
	return(TRUE);	  
}

/**
*****************************************************************************
	Function Name : decode()

	Description   :

	Input         :  

	Output        : 

	Note          :
******************************************************************************
*/
void decode(uint8_t sc)
{
  static unsigned char is_up=0, shift = 0;    //, mode = 0;
  unsigned char i;

  if (!is_up)                                 /* Last data received was the up-key identifier */
    {
      switch (sc)
        {
        case BREAKCHAR :                      /*  The up-key identifier */
          is_up = 1;
          break;
        case NONUMCODE :
          //is_up = 1;
          break;
        case LEFTSHIFT :
          shift = 1;
          break;
        case RIGHTSHIFT:
          shift = 1;
          break;
  //      case 0x58: //caps lock
  //          break;
        default:
          if (!shift)                          /* If shift not pressed */
            {
              // do a table look-up
              for (i = 0; unshifted[i][0]!=sc && unshifted[i][0]; i++);
              if (unshifted[i][0] == sc)
                {
                  putkeybuf((unshifted[i][1]));
                }
            }
          else
            {
              /* look-up */
              for (i = 0; shifted[i][0]!=sc && shifted[i][0]; i++);
              if (shifted[i][0] == sc)
                {
                  putkeybuf((shifted[i][1]));
                }
            }
          break;
        }
    }
  else
    {
      is_up = 0;
      switch (sc)
        {
        case LEFTSHIFT :
        case RIGHTSHIFT:
          shift = 0;
          break;
        }
    }
}

/**
*****************************************************************************
	Function Name : putkeybuf()

	Description   :

	Input         :  

	Output        : 

	Note          :
******************************************************************************
*/
void putkeybuf (uint8_t key)
{
  if (keyRead <KBUFFSIZE)  		    //make sure that we don't overflow the buffer
    {
      keyRead++;					    //Increment the number of keys read
      keyBuf[keyIn++] = key;		    //Store the scan code into the buffer
      if (keyIn >= KBUFFSIZE)  	        //Adjust index to the next scan code to put in buffer
      {
          keyIn = 0;
      }
    }
}

/**
*****************************************************************************
	Function Name : keyhit()

	Description   :

	Input         :  

	Output        : 

	Note          :
******************************************************************************
*/
uint8_t keyhit(void)
{
  unsigned char hit;

  hit = (unsigned char)(keyRead > 0)? TRUE : FALSE;

  return(hit);
}

/**
*****************************************************************************
	Function Name : xgetkey()

	Description   :

	Input         :  

	Output        : 

	Note          :
******************************************************************************
*/
uint8_t xgetkey(void)
{
  unsigned char key=0;

  if (keyRead > 0)
  {
      keyRead--;			            /* Decrement the number of keys in the buffer */
      key = keyBuf[keyOut];         	/* Get scan code from the buffer */
      keyOut++;
      if (keyOut >= KBUFFSIZE)
      {
          keyOut = 0;
      }
  }
	// else 
	 //{
      //return (0xFF);					     /* No scan codes in buffer, return -1 */
     //}
  return (key);						   /* return the scan code of the key pressed */
}

