#define MAX_CHAR_IN_ONE_LINE 16

#define LCD_DATA_DIR	   LPC_GPIO->DIR[1]
#define LCD_DATA_SET	   LPC_GPIO->PIN[1]
#define LCD_DATA_CLR	   LPC_GPIO->PIN[1]
#define LCD_DATA_PIN	   LPC_GPIO->PIN[1]

#define LCD_CTRL_DIR	   LPC_GPIO->DIR[1]
#define LCD_CTRL_SET       LPC_GPIO->PIN[1]
#define LCD_CTRL_CLR       LPC_GPIO->PIN[1]
#define LCD_CTRL_PIN	   LPC_GPIO->PIN[1]

#define LCDRS_PIN	           (15)
#define LCDRW_PIN	           (14)
#define LCDEN_PIN	           (13)

#define LCD_D4_PIN 				(25)
#define LCD_D5_PIN 				(26)
#define LCD_D6_PIN 				(27)
#define LCD_D7_PIN 				(28)

/* When porting to other boards, do not modify anything below this line */

#define LCDRS	           (1 << LCDRS_PIN)
#define LCDRW	           (1 << LCDRW_PIN)
#define LCDEN	           (1 << LCDEN_PIN)

#define LCD_D4 (1 << LCD_D4_PIN)
#define LCD_D5 (1 << LCD_D5_PIN)
#define LCD_D6 (1 << LCD_D6_PIN)
#define LCD_D7 (1 << LCD_D7_PIN)

#define LCD_DATA_MASK           (LCD_D4 | LCD_D5 | LCD_D6 | LCD_D7)
#define LCD_BUSY_FLAG           LCD_D7

#define LCD_CONTROL_MASK        (LCDRS | LCDRW | LCDEN )

#define WAIT_LCD_BUSY_FLAG()	while(LCD_DATA_PIN & LCD_D7)

#define DATA_POS LCD_D4_PIN
#define DELAY_COUNT 100

enum ROW_NUMBERS
{
  LINE1,
  LINE2
};

void init_lcd(void);
void lcd_putstring(unsigned char line, char *string);
void lcd_clear(void);
void lcd_backlight_on(void);
void lcd_backlight_off(void);
int lcd_gotoxy(unsigned int x, unsigned int y);
void lcd_putchar(int c);
