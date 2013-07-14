#include "blinky.h"
#include "LPC17xx.H"
#include "delay.h"

/**
  *******************************************************************
  	Funtion Name : Test_LED()

  	Description  :

  	Input        :

  	Output       : None

  	Note         :
  *****************************************************************
  */
  void test_led(void)
  {
	  int j;

	  for(j=0;j<5;j++)
	     {
	 	    LPC_GPIO0->FIOPIN |=  TEST_LED;
	 	    delay(80000);                         /* Delay 500ms                        */
	 	    LPC_GPIO0->FIOPIN &= ~TEST_LED;
	 	    delay(80000);
	     }
  }

  int main(void) {

  	// TODO: insert code here
	  LPC_GPIO0->FIODIR =TEST_LED;
  	// Enter an infinite loop, just incrementing a counter

  	while(1)
  	{
  		test_led();
  	}
  	
  }
