
#include "i2c.h"
#include "lcd.h"

unsigned char i2c_test_buf[255];
/**
**************************************************************************

****1111
  Function Name : wait()

  Description :This function suspends the tasks for specified ticks.

  Input :  ticks:no of ticks in multiple of 1 usec
            task: task to be suspended

  Output : void

  Note :
*******************************************************************************
*/

void wait(int count)
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
*******************************************************************
	Funtion Name : test_i2c_at24c256_flash()

	Description  :

	Input        :

	Output       : None

	Note         :
*****************************************************************
*/

void test_i2c_at24c256_flash(void)
{
  unsigned char i=0;
  unsigned char status=1;


  for(i=0;i<sizeof(i2c_test_buf);i++)
    i2c_test_buf[i] = i;

  m24xx_write(EEPROM_24XX256, 0 , 0 , (char *)i2c_test_buf , sizeof(i2c_test_buf));

  for(i=0;i<sizeof(i2c_test_buf);i++)
    i2c_test_buf[i] = 0;

  m24xx_read(EEPROM_24XX256, 0 , 0 , (char *)i2c_test_buf , sizeof(i2c_test_buf));

  for(i=0;i<sizeof(i2c_test_buf);i++)
  {
    if(i2c_test_buf[i] != i)
    {
      status = 0;
      break;
    }
  }

  if(status == 1)
  {
	  lcd_putstring(LINE2,"I2C - PASS");
  }
  else
  {
	  lcd_putstring(LINE2,"I2c - FAIL");

  }
}

/**
********************************************************************************************
	Function Name :	main()

	Description :

	Input :	Void

	Output :

	Note :
**********************************************************************************************
*/
int main (void)
{
  init_lcd();						// Initialize LCD
  lcd_putstring(LINE1,"NGX TECHNOLOGIES");
  i2c_lpc_init(I2C_SPEED_100);		// Initialize I2C

  wait(1000);
//  lcd_clear();
  test_i2c_at24c256_flash();	   // Test I2C EEPROM by wrinting some data at some address and reading the same from same address

  while(1)
  {
  }
}
