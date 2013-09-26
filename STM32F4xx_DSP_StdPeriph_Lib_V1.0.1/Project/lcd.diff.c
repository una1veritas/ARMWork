21,22c21,22
< #define LCD_REG              (*((volatile unsigned short *) 0x6F000000)) /* RS = 0 */
< #define LCD_RAM              (*((volatile unsigned short *) 0x6F010000)) /* RS = 1 */
---
> //#define LCD_REG              (*((volatile unsigned short *) 0x6F000000)) /* RS = 0 */
> //#define LCD_RAM              (*((volatile unsigned short *) 0x6F010000)) /* RS = 1 */
44,46c44,45
< * Function Name  : LCD_CtrlLinesConfig
< * Description    : Configures LCD Control lines (FSMC Pins) in alternate function
<                    Push-Pull mode.
---
> * Function Name  : LCD_Configuration
> * Description    : Configure the LCD Control pins and FSMC Parallel interface
52c51
< static void LCD_CtrlLinesConfig(void)
---
> static void LCD_Configuration(void)
56,66c55,60
< 	/* Enable FSMC, GPIOD, GPIOE, GPIOF, GPIOG and AFIO clocks */
< 	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
< 	
< 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF | 
< 	                       RCC_APB2Periph_GPIOG | RCC_APB2Periph_AFIO, ENABLE);
< 	
< 	/* Set PD.00(D2), PD.01(D3), PD.04(NOE), PD.05(NWE), PD.08(D13), PD.09(D14),
< 	 PD.10(D15), PD.14(D0), PD.15(D1) as alternate function push pull */
< 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 |
< 	                            GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 | 
< 	                            GPIO_Pin_15;
---
> 	/* Enable GPIOC and GPIOE clocks */
> 	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE);  
> 	                    
> 	/* PE.00(D0), PE.01(D1), PE.02(D2), PE.03(D3), PE.04(D4), PE.05(D5), PE.06(D6), PE.07(D7), PE.08(D8)
> 	 PE.09(D9), PE.10(D10), PE.11(D11), PE.12(D12), PE.13(D13), PE.14(D14), PE.15(D15)   */
> 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
68,79c62
< 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
< 	GPIO_Init(GPIOD, &GPIO_InitStructure);
< 	
< 	/* Set PE.07(D4), PE.08(D5), PE.09(D6), PE.10(D7), PE.11(D8), PE.12(D9), PE.13(D10),
< 	 PE.14(D11), PE.15(D12) as alternate function push pull */
< 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | 
< 	                            GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
< 	                            GPIO_Pin_15;
< 	GPIO_Init(GPIOE, &GPIO_InitStructure);
< 	
< 	/* Set PE.02(A23), PE.05(A21), PE.06(A22) as alternate function push pull */
< 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_5 | GPIO_Pin_6;
---
> 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
80a64,69
> 
> 	/* PD.11(RST), PD.12(WR), PD.13(RD) , PD.14(CS), PD.15(RS)*/
> 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
> 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
> 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
> 	GPIO_Init(GPIOD, &GPIO_InitStructure);
82c71
< 	/* Set PF.00(A0 (RS)) as alternate function push pull */
---
> 	/* PA.00(BL_PWM)*/
84,88c73,75
< 	GPIO_Init(GPIOF, &GPIO_InitStructure);
< 	
< 	/* Set PG.05(A15), PG.12(NE4), PG.13(A24) as alternate function push pull - CE3(LCD /CS) */
< 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_12 | GPIO_Pin_13;
< 	GPIO_Init(GPIOG, &GPIO_InitStructure);
---
> 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
> 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
> 	GPIO_Init(GPIOA, &GPIO_InitStructure);
90a78
> 
92,94c80,82
< * Function Name  : LCD_FSMCConfig
< * Description    : Configures the Parallel interface (FSMC) for LCD(Parallel mode)
< * Input          : None
---
> * Function Name  : LCD_WriteReg
> * Description    : 
> * Input          : - index:
99c87
< static void LCD_FSMCConfig(void)
---
> __inline void LCD_WriteIndex(uint16_t index)
101,110c89,90
< 	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
< 	FSMC_NORSRAMTimingInitTypeDef FSMC_NORSRAMTimingInitStructure;
< 	/* FSMC Read/Write Timing */
< 	FSMC_NORSRAMTimingInitStructure.FSMC_AddressSetupTime = 5;  /* Address Setup Time  */
< 	FSMC_NORSRAMTimingInitStructure.FSMC_AddressHoldTime = 0;	   
< 	FSMC_NORSRAMTimingInitStructure.FSMC_DataSetupTime = 5;     /* Data Setup Time */
< 	FSMC_NORSRAMTimingInitStructure.FSMC_BusTurnAroundDuration = 0x00;
< 	FSMC_NORSRAMTimingInitStructure.FSMC_CLKDivision = 0x00;
< 	FSMC_NORSRAMTimingInitStructure.FSMC_DataLatency = 0x00;
< 	FSMC_NORSRAMTimingInitStructure.FSMC_AccessMode = FSMC_AccessMode_A;	/* FSMC Access Mode */
---
> 	Clr_Rs;
> 	Set_nRd;
112,135c92
< 	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;
< 	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
< 	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
< 	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
< 	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
< 	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
< 	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
< 	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
< 	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
< 	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
< 	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
< 	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
< 	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
< 	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &FSMC_NORSRAMTimingInitStructure;
< 	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 
< 	/* FSMC Write Timing */
< 	FSMC_NORSRAMTimingInitStructure.FSMC_AddressSetupTime = 15;//2;//1;   /* Address Setup Time */
< 	FSMC_NORSRAMTimingInitStructure.FSMC_AddressHoldTime = 0;	   
< 	FSMC_NORSRAMTimingInitStructure.FSMC_DataSetupTime = 15;//2;//1;	     /* Data Setup Time */
< 	FSMC_NORSRAMTimingInitStructure.FSMC_BusTurnAroundDuration = 0x00;
< 	FSMC_NORSRAMTimingInitStructure.FSMC_CLKDivision = 0x00;
< 	FSMC_NORSRAMTimingInitStructure.FSMC_DataLatency = 0x00;
< 	FSMC_NORSRAMTimingInitStructure.FSMC_AccessMode = FSMC_AccessMode_A;	/* FSMC Access Mode */
< 	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &FSMC_NORSRAMTimingInitStructure;	  
---
> 	GPIOE->ODR = index;	 /* GPIO_Write(GPIOE,index); */
137c94,108
< 	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 
---
> 	Clr_nWr;
> 	Set_nWr;
> }
> 
> /*******************************************************************************
> * Function Name  : LCD_WriteReg
> * Description    : 
> * Input          : - index:
> * Output         : None
> * Return         : None
> * Attention		 : None
> *******************************************************************************/
> __inline void LCD_WriteData(uint16_t data)
> {
> 	Set_Rs;
139,140c110,113
< 	/* Enable FSMC Bank4_SRAM Bank */
< 	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);  
---
> 	GPIOE->ODR = data;	 /* GPIO_Write(GPIOE,data); */
> 	
> 	Clr_nWr;
> 	Set_nWr;
144,145c117,118
< * Function Name  : LCD_Configuration
< * Description    : Configure the LCD Control pins and FSMC Parallel interface
---
> * Function Name  : LCD_ReadData
> * Description    : 
147a121,173
> * Return         : 
> * Attention		 : None
> *******************************************************************************/
> __inline uint16_t LCD_ReadData(void)
> {
> 	uint16_t value;
> 
> 	Set_Rs;
> 	Set_nWr;
> 	Clr_nRd;
> 
> /* 
>     PE.00(D0), PE.01(D1), PE.02(D2), PE.03(D3), PE.04(D4), PE.05(D5), PE.06(D6), PE.07(D7), PE.08(D8)
>     PE.09(D9), PE.10(D10), PE.11(D11), PE.12(D12), PE.13(D13), PE.14(D14), PE.15(D15)   */
> /* 
>     GPIO_InitTypeDef GPIO_InitStructure; 
>     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
>     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
>     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
>     GPIO_Init(GPIOE, &GPIO_InitStructure);
> */
> 	GPIOE->CRH = 0x44444444;
> 	GPIOE->CRL = 0x44444444;
> /*
>     value = GPIO_ReadInputData(GPIOE);
>     value = GPIO_ReadInputData(GPIOE);
> */
>     value = GPIOE->IDR;
>     value = GPIOE->IDR;
> 
> /*  
>     PE.00(D0), PE.01(D1), PE.02(D2), PE.03(D3), PE.04(D4), PE.05(D5), PE.06(D6), PE.07(D7), PE.08(D8)
>     PE.09(D9), PE.10(D10), PE.11(D11), PE.12(D12), PE.13(D13), PE.14(D14), PE.15(D15)   */
> /*
>     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
>     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
>     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
>     GPIO_Init(GPIOE, &GPIO_InitStructure);
> */
>     GPIOE->CRH = 0x33333333;
>     GPIOE->CRL = 0x33333333;
> 
>     Set_nRd;
> 
>     return value;
> }
> 
> /*******************************************************************************
> * Function Name  : LCD_WriteReg
> * Description    : Writes to the selected LCD register.
> * Input          : - LCD_Reg: address of the selected register.
> *                  - LCD_RegValue: value to write to the selected register.
> * Output         : None
151c177,195
< static void LCD_Configuration(void)
---
> __inline void LCD_WriteReg(uint16_t LCD_Reg,uint16_t LCD_RegValue)
> { 
> 	/* Write 16-bit Index, then Write Reg */  
> 	Clr_Cs;
> 	LCD_WriteIndex(LCD_Reg);         
> 	/* Write 16-bit Reg */
> 	LCD_WriteData(LCD_RegValue);  
> 	Set_Cs; 
> }
> 
> /*******************************************************************************
> * Function Name  : LCD_WriteReg
> * Description    : Reads the selected LCD Register.
> * Input          : None
> * Output         : None
> * Return         : LCD Register Value.
> * Attention		 : None
> *******************************************************************************/
> __inline uint16_t LCD_ReadReg(uint16_t LCD_Reg)
153,154c197
< 	/* Configure the LCD Control pins --------------------------------------------*/
< 	LCD_CtrlLinesConfig();
---
> 	uint16_t LCD_RAM;
156,157c199,245
< 	/* Configure the FSMC Parallel interface -------------------------------------*/
< 	LCD_FSMCConfig();
---
> 	/* Write 16-bit Index (then Read Reg) */
> 	Clr_Cs;
> 	LCD_WriteIndex(LCD_Reg);
> 	/* Read 16-bit Reg */
> 	LCD_RAM = LCD_ReadData();      	
> 	Set_Cs;
> 	return LCD_RAM;
> }
> 
> static void LCD_SetCursor( uint16_t Xpos, uint16_t Ypos )
> {
>     #if  ( DISP_ORIENTATION == 90 ) || ( DISP_ORIENTATION == 270 )
> 	
>  	uint16_t temp = Xpos;
> 
> 			 Xpos = Ypos;
> 			 Ypos = ( MAX_X - 1 ) - temp;  
> 
> 	#elif  ( DISP_ORIENTATION == 0 ) || ( DISP_ORIENTATION == 180 )
> 		
> 	#endif
> 
>   switch( LCD_Code )
>   {
>      default:		 /* 0x9320 0x9325 0x9328 0x9331 0x5408 0x1505 0x0505 0x7783 0x4531 0x4535 */
>           LCD_WriteReg(0x0020, Xpos );     
>           LCD_WriteReg(0x0021, Ypos );     
> 	      break; 
> 
>      case SSD1298: 	 /* 0x8999 */
>      case SSD1289:   /* 0x8989 */
> 	      LCD_WriteReg(0x004e, Xpos );      
>         LCD_WriteReg(0x004f, Ypos );          
> 	      break;  
> 
>      case HX8347A: 	 /* 0x0047 */
>      case HX8347D: 	 /* 0x0047 */
> 	      LCD_WriteReg(0x02, Xpos>>8 );                                                  
> 	      LCD_WriteReg(0x03, Xpos );  
> 
> 	      LCD_WriteReg(0x06, Ypos>>8 );                           
> 	      LCD_WriteReg(0x07, Ypos );    
> 	
> 	      break;     
>      case SSD2119:	 /* 3.5 LCD 0x9919 */
> 	      break; 
>   }
186,188d273
< 	uint16_t DeviceCode;
< 	uint16_t i;
< 
191c276
< 	GPIO_InitTypeDef GPIO_InitStructure;
---
> 	uint16_t DeviceCode;
193,200c278,279
< 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
<   /*Back Light Control (PB.00), LCD Reset (PB.01)*/
< 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
< 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
<   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
< 	GPIO_Init(GPIOB, &GPIO_InitStructure);
<   GPIO_SetBits(GPIOB, GPIO_Pin_0);      /* Enable LCD back light */
<   GPIO_ResetBits(GPIOB, GPIO_Pin_1);		/* Enable LCD_RESET */
---
> 	LCD_Configuration();
>   GPIO_ResetBits(GPIOD, GPIO_Pin_11);		/* LCD_RESET */
202c281,282
<   GPIO_SetBits(GPIOB, GPIO_Pin_1);		  /* Disable LCD_RESET */
---
>   GPIO_SetBits(GPIOD, GPIO_Pin_11);
>   GPIO_SetBits(GPIOA, GPIO_Pin_0);  /* Enable back light */
204d283
< 	LCD_Configuration();
209d287
< 	DeviceCode = 0x0123;
857d934
< 		DeviceCode = 0x0123;
974c1051
< 
---
> 	Clr_Cs; 
980,1020c1057
< 
< }
< 
< static void LCD_SetCursor( uint16_t Xpos, uint16_t Ypos )
< {
<     #if  ( DISP_ORIENTATION == 90 ) || ( DISP_ORIENTATION == 270 )
< 	
<  	uint16_t temp = Xpos;
< 
< 			 Xpos = Ypos;
< 			 Ypos = ( MAX_X - 1 ) - temp;  
< 
< 	#elif  ( DISP_ORIENTATION == 0 ) || ( DISP_ORIENTATION == 180 )
< 		
< 	#endif
< 
<   switch( LCD_Code )
<   {
<      default:		 /* 0x9320 0x9325 0x9328 0x9331 0x5408 0x1505 0x0505 0x7783 0x4531 0x4535 */
<           LCD_WriteReg(0x0020, Xpos );     
<           LCD_WriteReg(0x0021, Ypos );     
< 	      break; 
< 
<      case SSD1298: 	 /* 0x8999 */
<      case SSD1289:   /* 0x8989 */
< 	      LCD_WriteReg(0x004e, Xpos );      
<         LCD_WriteReg(0x004f, Ypos );          
< 	      break;  
< 
<      case HX8347A: 	 /* 0x0047 */
<      case HX8347D: 	 /* 0x0047 */
< 	      LCD_WriteReg(0x02, Xpos>>8 );                                                  
< 	      LCD_WriteReg(0x03, Xpos );  
< 
< 	      LCD_WriteReg(0x06, Ypos>>8 );                           
< 	      LCD_WriteReg(0x07, Ypos );    
< 	
< 	      break;     
<      case SSD2119:	 /* 3.5 LCD 0x9919 */
< 	      break; 
<   }
---
> 	Set_Cs;
1058c1095
< 
---
> 	Clr_Cs;
1070c1107
< 
---
>       Set_Cs;	
1080a1118
>       Set_Cs;	
1084a1123
>       Set_Cs;	
1149c1188,1189
<     }while( y1 >= y0 ); 
---
>     }
>     while( y1 >= y0 ); 
1273,1346d1312
< }
< 
< /*******************************************************************************
< * Function Name  : LCD_WriteReg
< * Description    : 
< * Input          : - index:
< * Output         : None
< * Return         : None
< * Attention		 : None
< *******************************************************************************/
< __inline void LCD_WriteIndex(uint16_t index)
< {
< 	LCD_REG	= index;
< }
< 
< /*******************************************************************************
< * Function Name  : LCD_WriteReg
< * Description    : 
< * Input          : - index:
< * Output         : None
< * Return         : None
< * Attention		 : None
< *******************************************************************************/
< __inline void LCD_WriteData(uint16_t data)
< {
< 	LCD_RAM = data;
< }
< 
< /*******************************************************************************
< * Function Name  : LCD_ReadData
< * Description    : 
< * Input          : None
< * Output         : None
< * Return         : 
< * Attention		 : None
< *******************************************************************************/
< __inline uint16_t LCD_ReadData(void)
< { 
<   return LCD_RAM;
< }
< 
< 
< /*******************************************************************************
< * Function Name  : LCD_WriteReg
< * Description    : Writes to the selected LCD register.
< * Input          : - LCD_Reg: address of the selected register.
< *                  - LCD_RegValue: value to write to the selected register.
< * Output         : None
< * Return         : None
< * Attention		 : None
< *******************************************************************************/
< __inline void LCD_WriteReg(uint16_t LCD_Reg,uint16_t LCD_RegValue)
< { 
< 	/* Write 16-bit Index, then Write Reg */  
< 	LCD_WriteIndex(LCD_Reg);         
< 	/* Write 16-bit Reg */
< 	LCD_WriteData(LCD_RegValue);  
< }
< 
< /*******************************************************************************
< * Function Name  : LCD_WriteReg
< * Description    : Reads the selected LCD Register.
< * Input          : None
< * Output         : None
< * Return         : LCD Register Value.
< * Attention		 : None
< *******************************************************************************/
< __inline uint16_t LCD_ReadReg(uint16_t LCD_Reg)
< {
< 	/* Write 16-bit Index (then Read Reg) */
< 	LCD_WriteIndex(LCD_Reg);
< 
< 	/* Read 16-bit Reg */
< 	return LCD_ReadData();
