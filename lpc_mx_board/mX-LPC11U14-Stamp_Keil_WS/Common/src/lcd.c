#include "LPC11Uxx.h"
#include "lcd.h"
/**
**************************************************************************

****1111
  Function Name : delay()

  Description :This function suspends the tasks for specified ticks.

  Input :  ticks:no of ticks in multiple of 1 usec
            task: task to be suspended

  Output : void

  Note :
*******************************************************************************
*/
void delay(int count)
{
  int j=0,i=0;

  for(j=0;j<count;j++)
  {
    /* At 60Mhz, the below loop introduces
    delay of 10 us */
    for(i=0;i<35;i++);
  }
}

/**
********************************************************************************************
  Function Name :	wait_lcd()

  Description   :

  Input         :

  Output        :	Void

  Note          :
********************************************************************************************
*/
void wait_lcd( void )
{
  LCD_DATA_DIR &= ~LCD_DATA_MASK;
  LCD_CTRL_CLR &=  ~(LCDRS);
  LCD_CTRL_SET |=  LCDRW |LCDEN;

  WAIT_LCD_BUSY_FLAG();		/* wait for busy flag to become low */

  LCD_CTRL_CLR &= ~(LCDEN | LCDRW);
  LCD_DATA_DIR |= LCD_DATA_MASK;

  delay(100);
}
/**
********************************************************************************************
  Function Name :	lcd_command_write()

  Description   :

  Input         :

  Output        :	Void

  Note          :
********************************************************************************************
*/

void lcd_command_write( unsigned char command )
{
  unsigned int temp=0;
  unsigned int temp1=0;

  temp=command;
  temp=(temp>>4)&0x0F;
  temp1=(temp<<DATA_POS)&LCD_DATA_MASK;

  LCD_CTRL_CLR &= ~(LCDRS);
  LCD_CTRL_SET |= LCDEN;
  LCD_DATA_CLR &= ~(LCD_DATA_MASK);
  LCD_DATA_SET |= temp1;
  delay(DELAY_COUNT);
  LCD_CTRL_CLR &= ~(LCDEN);

  temp=command;
  temp&=0x0F;
  temp1=(temp<<DATA_POS)&LCD_DATA_MASK;
  delay(DELAY_COUNT);

  LCD_CTRL_CLR &= ~(LCDRS);
  LCD_CTRL_SET |= LCDEN;
  LCD_DATA_CLR &= ~(LCD_DATA_MASK);
  LCD_DATA_SET |= temp1;
  delay(DELAY_COUNT);
  LCD_CTRL_CLR &= ~(LCDEN);
  wait_lcd();
}
/**
********************************************************************************************
  Function Name :	set_lcd_port_output()

  Description   :

  Input         :

  Output        :	Void

  Note          :
********************************************************************************************
*/
void set_lcd_port_output( void )
{
  LCD_CTRL_DIR |= ( LCDEN | LCDRS | LCDRW );
  LCD_CTRL_CLR &= ~( LCDEN | LCDRS | LCDRW );
  LCD_DATA_DIR |= LCD_DATA_MASK;
}
/* *
********************************************************************************************
  Function Name :	lcd_clear()

  Description   :

  Input         :

  Output        :	Void

  Note          :
********************************************************************************************
*/
void lcd_clear( void)
{
  lcd_command_write( 0x01 );
}
/**
********************************************************************************************
  Function Name :	lcd_gotoxy()

  Description   :

  Input         :

  Output        :	Void

  Note          :
********************************************************************************************
*/
int lcd_gotoxy( unsigned int x, unsigned int y)
{
  int retval = 0;

  if( (x > 1) && (y > 15) )
  {
    retval = -1;
  } else {
  if( x == 0 )
  {
    lcd_command_write( 0x80 + y );		/* command - position cursor at 0x00 (0x80 + 0x00 ) */
  } else if( x==1 ){
    lcd_command_write( 0xC0 + y );		/* command - position cursor at 0x40 (0x80 + 0x00 ) */
    }
   }
   return retval;
}

/**
********************************************************************************************
  Function Name :	lcd_data_write()

  Description   :

  Input         :

  Output        :	Void

  Note          :
********************************************************************************************
*/
void lcd_data_write( unsigned char data )
{
  unsigned char temp=0;
  unsigned int temp1=0;

  temp=data;
  temp=(temp>>4)& 0x0F;
  temp1=(temp<<DATA_POS) & LCD_DATA_MASK;

  LCD_CTRL_SET |= LCDEN|LCDRS;
  LCD_DATA_CLR &= ~(LCD_DATA_MASK);
  LCD_DATA_SET |= temp1;
  LCD_CTRL_CLR &= ~(LCDEN);

  temp=data;
  temp&=0x0F;
  temp1=(temp<<DATA_POS)&LCD_DATA_MASK;

  LCD_CTRL_SET |= LCDEN|LCDRS;
  LCD_DATA_CLR &= ~(LCD_DATA_MASK);
  LCD_DATA_SET |= temp1;
  LCD_CTRL_CLR &= ~(LCDEN);
  wait_lcd();
}
/**
********************************************************************************************
  Function Name :	lcd_putchar()

  Description   :

  Input         :

  Output        :	Void

  Note          :
********************************************************************************************
*/
void lcd_putchar( int c )
{
  lcd_data_write( c );
}

/**
********************************************************************************************
  Function Name :	lcd_putstring()

  Description   :

  Input         :

  Output        :	Void

  Note          :
********************************************************************************************
*/
void lcd_putstring( unsigned char line, char *string )
{
  unsigned char len = MAX_CHAR_IN_ONE_LINE;

  lcd_gotoxy( line, 0 );
  while(*string != '\0' && len--)
  {
    lcd_putchar( *string );
    string++;
  }
}

//#define TEST_GPIOS
/**
********************************************************************************************
  Function Name :	init_lcd()

  Description   :

  Input         :

  Output        :	Void

  Note          :
********************************************************************************************
*/
void init_lcd( void )
{
  set_lcd_port_output();

#ifdef TEST_GPIOS
  while(1)
  {
	  LCD_DATA_SET |= LCD_DATA_MASK;
	  LCD_CTRL_SET |=  LCD_CONTROL_MASK;
	  delay(10000);
	  LCD_DATA_CLR &= ~LCD_DATA_MASK;
	  LCD_CTRL_CLR &= ~LCD_CONTROL_MASK;
	  delay(10000);
  }
#endif

  lcd_command_write(0x28);     /*   4-bit interface, two line, 5X7 dots.        */
  lcd_command_write(0x02);     /*   cursor home                                 */
  lcd_command_write(0x06);     /*   cursor move direction                       */
  lcd_command_write(0x0C) ;    /*   display on      */
  lcd_gotoxy(0, 0);
  lcd_clear();
}
