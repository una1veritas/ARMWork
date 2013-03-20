
static const uint8 Consolas7x12[] = {
    0x0, 0x0, // size of zero indicates fixed width font, actual length is width * height
    0x06, // width
    0x0f, // height
    0x20, // first char
    0x60, // char count
    
    // Fixed width; char width table not used !!!!
    
    // font data
        0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char  
        0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7C, 0x03, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00,  // Code for char !
        0x05, 0x00, 0x00, 0x00, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char "
        0x06, 0x90, 0x00, 0xD0, 0x03, 0xBC, 0x00, 0xD0, 0x03, 0xBC, 0x00, 0x90, 0x00, 0x00, 0x00,  // Code for char #
        0x06, 0x00, 0x00, 0x18, 0x02, 0x24, 0x07, 0xFE, 0x02, 0x44, 0x02, 0x80, 0x01, 0x00, 0x00,  // Code for char $
        0x07, 0x3C, 0x02, 0x24, 0x01, 0xBC, 0x00, 0xE0, 0x03, 0x50, 0x02, 0xC8, 0x03, 0x04, 0x00,  // Code for char %
        0x07, 0x80, 0x01, 0x58, 0x02, 0x24, 0x02, 0xE4, 0x02, 0x9C, 0x03, 0xC0, 0x03, 0x00, 0x02,  // Code for char &
        0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char '
        0x05, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x01, 0x0C, 0x06, 0x02, 0x08, 0x00, 0x00, 0x00, 0x00,  // Code for char (
        0x05, 0x00, 0x00, 0x00, 0x00, 0x02, 0x08, 0x0C, 0x06, 0xF0, 0x01, 0x00, 0x00, 0x00, 0x00,  // Code for char )
        0x06, 0x00, 0x00, 0x28, 0x00, 0x10, 0x00, 0x7C, 0x00, 0x10, 0x00, 0x28, 0x00, 0x00, 0x00,  // Code for char *
        0x06, 0x00, 0x00, 0x40, 0x00, 0x40, 0x00, 0xF0, 0x01, 0x40, 0x00, 0x40, 0x00, 0x00, 0x00,  // Code for char +
        0x04, 0x00, 0x00, 0x00, 0x08, 0x00, 0x0F, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char ,
        0x06, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0x00, 0x00, 0x00,  // Code for char -
        0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char .
        0x06, 0x00, 0x00, 0x00, 0x04, 0x00, 0x03, 0xE0, 0x00, 0x18, 0x00, 0x04, 0x00, 0x00, 0x00,  // Code for char /
        0x07, 0x00, 0x00, 0xF0, 0x01, 0x8C, 0x03, 0x44, 0x02, 0x24, 0x02, 0x1C, 0x03, 0xF8, 0x00,  // Code for char 0
        0x06, 0x00, 0x00, 0x08, 0x02, 0x04, 0x02, 0xFC, 0x03, 0x00, 0x02, 0x00, 0x02, 0x00, 0x00,  // Code for char 1
        0x06, 0x00, 0x00, 0x08, 0x02, 0x04, 0x03, 0x84, 0x02, 0x44, 0x02, 0x38, 0x02, 0x00, 0x00,  // Code for char 2
        0x06, 0x00, 0x00, 0x04, 0x02, 0x24, 0x02, 0x24, 0x02, 0x24, 0x02, 0xD8, 0x01, 0x00, 0x00,  // Code for char 3
        0x06, 0xC0, 0x00, 0xA0, 0x00, 0x90, 0x00, 0x8C, 0x00, 0xFC, 0x03, 0x80, 0x00, 0x00, 0x00,  // Code for char 4
        0x06, 0x00, 0x00, 0x3C, 0x02, 0x24, 0x02, 0x24, 0x02, 0x24, 0x03, 0xC4, 0x01, 0x00, 0x00,  // Code for char 5
        0x06, 0x00, 0x00, 0xF0, 0x01, 0x2C, 0x02, 0x24, 0x02, 0x24, 0x02, 0xC0, 0x01, 0x00, 0x00,  // Code for char 6
        0x06, 0x00, 0x00, 0x04, 0x00, 0x04, 0x03, 0xC4, 0x01, 0x34, 0x00, 0x0C, 0x00, 0x00, 0x00,  // Code for char 7
        0x06, 0x00, 0x00, 0xD8, 0x01, 0x74, 0x02, 0x24, 0x02, 0x64, 0x02, 0xD8, 0x01, 0x00, 0x00,  // Code for char 8
        0x06, 0x00, 0x00, 0x38, 0x02, 0x44, 0x02, 0x44, 0x02, 0x44, 0x01, 0xF8, 0x00, 0x00, 0x00,  // Code for char 9
        0x04, 0x00, 0x00, 0x00, 0x00, 0x30, 0x03, 0x30, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char :
        0x04, 0x00, 0x00, 0x00, 0x08, 0x30, 0x0F, 0x30, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char ;
        0x06, 0x00, 0x00, 0x40, 0x00, 0xA0, 0x00, 0x10, 0x01, 0x10, 0x01, 0x08, 0x02, 0x00, 0x00,  // Code for char <
        0x06, 0x00, 0x00, 0xA0, 0x00, 0xA0, 0x00, 0xA0, 0x00, 0xA0, 0x00, 0xA0, 0x00, 0x00, 0x00,  // Code for char =
        0x06, 0x00, 0x00, 0x08, 0x02, 0x10, 0x01, 0x10, 0x01, 0xA0, 0x00, 0x40, 0x00, 0x00, 0x00,  // Code for char >
        0x06, 0x00, 0x00, 0x00, 0x00, 0x44, 0x03, 0x44, 0x03, 0x48, 0x00, 0x38, 0x00, 0x00, 0x00,  // Code for char ?
        0x07, 0xE0, 0x03, 0x18, 0x0C, 0xCC, 0x09, 0x24, 0x09, 0xE4, 0x09, 0x0C, 0x01, 0xF8, 0x00,  // Code for char @
        0x06, 0x00, 0x00, 0xC0, 0x03, 0xB8, 0x00, 0x84, 0x00, 0xB8, 0x00, 0xC0, 0x03, 0x00, 0x00,  // Code for char A
        0x06, 0x00, 0x00, 0xFC, 0x03, 0x24, 0x02, 0x24, 0x02, 0x24, 0x02, 0xD8, 0x01, 0x00, 0x00,  // Code for char B
        0x06, 0x00, 0x00, 0xF0, 0x01, 0x08, 0x03, 0x04, 0x02, 0x04, 0x02, 0x04, 0x02, 0x00, 0x00,  // Code for char C
        0x06, 0x00, 0x00, 0xFC, 0x03, 0x04, 0x02, 0x04, 0x02, 0x0C, 0x03, 0xF8, 0x00, 0x00, 0x00,  // Code for char D
        0x05, 0x00, 0x00, 0xFC, 0x03, 0x24, 0x02, 0x24, 0x02, 0x24, 0x02, 0x00, 0x00, 0x00, 0x00,  // Code for char E
        0x05, 0x00, 0x00, 0xFC, 0x03, 0x24, 0x00, 0x24, 0x00, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char F
        0x06, 0xF0, 0x00, 0x08, 0x01, 0x04, 0x02, 0x24, 0x02, 0x24, 0x02, 0xE4, 0x03, 0x00, 0x00,  // Code for char G
        0x06, 0x00, 0x00, 0xFC, 0x03, 0x20, 0x00, 0x20, 0x00, 0x20, 0x00, 0xFC, 0x03, 0x00, 0x00,  // Code for char H
        0x06, 0x00, 0x00, 0x04, 0x02, 0x04, 0x02, 0xFC, 0x03, 0x04, 0x02, 0x04, 0x02, 0x00, 0x00,  // Code for char I
        0x05, 0x00, 0x00, 0x04, 0x02, 0x04, 0x02, 0x04, 0x02, 0xFC, 0x01, 0x00, 0x00, 0x00, 0x00,  // Code for char J
        0x06, 0x00, 0x00, 0xFC, 0x03, 0x60, 0x00, 0x90, 0x00, 0x08, 0x01, 0x04, 0x02, 0x00, 0x00,  // Code for char K
        0x05, 0x00, 0x00, 0xFC, 0x03, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,  // Code for char L
        0x07, 0x00, 0x00, 0xFC, 0x03, 0x30, 0x00, 0x40, 0x00, 0x30, 0x00, 0xFC, 0x03, 0x00, 0x02,  // Code for char M
        0x06, 0x00, 0x00, 0xFC, 0x03, 0x18, 0x00, 0x60, 0x00, 0x80, 0x01, 0xFC, 0x03, 0x00, 0x00,  // Code for char N
        0x06, 0xF0, 0x01, 0x0C, 0x03, 0x04, 0x02, 0x04, 0x02, 0x0C, 0x03, 0xF8, 0x00, 0x00, 0x00,  // Code for char O
        0x06, 0x00, 0x00, 0xFC, 0x03, 0x44, 0x00, 0x44, 0x00, 0x44, 0x00, 0x38, 0x00, 0x00, 0x00,  // Code for char P
        0x06, 0xF0, 0x01, 0x0C, 0x03, 0x04, 0x06, 0x04, 0x0A, 0x0C, 0x0B, 0xF8, 0x08, 0x00, 0x00,  // Code for char Q
        0x07, 0x00, 0x00, 0xFC, 0x03, 0x44, 0x00, 0x44, 0x00, 0xC4, 0x00, 0xB8, 0x03, 0x00, 0x02,  // Code for char R
        0x06, 0x00, 0x00, 0x18, 0x02, 0x24, 0x02, 0x24, 0x02, 0x44, 0x02, 0x84, 0x01, 0x00, 0x00,  // Code for char S
        0x06, 0x00, 0x00, 0x04, 0x00, 0x04, 0x00, 0xFC, 0x03, 0x04, 0x00, 0x04, 0x00, 0x00, 0x00,  // Code for char T
        0x06, 0x00, 0x00, 0xFC, 0x01, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0xFC, 0x01, 0x00, 0x00,  // Code for char U
        0x06, 0x00, 0x00, 0x3C, 0x00, 0xE0, 0x01, 0x00, 0x02, 0xE0, 0x01, 0x1C, 0x00, 0x00, 0x00,  // Code for char V
        0x06, 0x00, 0x00, 0xFC, 0x03, 0x80, 0x01, 0x60, 0x00, 0x80, 0x01, 0xFC, 0x03, 0x00, 0x00,  // Code for char W
        0x07, 0x04, 0x02, 0x0C, 0x03, 0xF0, 0x00, 0x70, 0x00, 0x9C, 0x00, 0x04, 0x03, 0x00, 0x02,  // Code for char X
        0x07, 0x04, 0x00, 0x18, 0x00, 0x30, 0x00, 0xC0, 0x03, 0x30, 0x00, 0x18, 0x00, 0x04, 0x00,  // Code for char Y
        0x06, 0x00, 0x00, 0x04, 0x02, 0x84, 0x03, 0x64, 0x02, 0x1C, 0x02, 0x04, 0x02, 0x00, 0x00,  // Code for char Z
        0x05, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x0F, 0x04, 0x08, 0x04, 0x08, 0x00, 0x00, 0x00, 0x00,  // Code for char [
        0x06, 0x00, 0x00, 0x04, 0x00, 0x18, 0x00, 0xE0, 0x00, 0x00, 0x03, 0x00, 0x04, 0x00, 0x00,  // Code for char BackSlash
        0x05, 0x00, 0x00, 0x00, 0x00, 0x04, 0x08, 0x04, 0x08, 0xFC, 0x0F, 0x00, 0x00, 0x00, 0x00,  // Code for char ]
        0x06, 0x00, 0x00, 0x30, 0x00, 0x08, 0x00, 0x04, 0x00, 0x08, 0x00, 0x30, 0x00, 0x00, 0x00,  // Code for char ^
        0x07, 0x00, 0x08, 0x00, 0x08, 0x00, 0x08, 0x00, 0x08, 0x00, 0x08, 0x00, 0x08, 0x00, 0x08,  // Code for char _
        0x04, 0x00, 0x00, 0x02, 0x00, 0x06, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char `
        0x06, 0x00, 0x00, 0x80, 0x01, 0x50, 0x02, 0x50, 0x02, 0x50, 0x03, 0xE0, 0x03, 0x00, 0x00,  // Code for char a
        0x06, 0x00, 0x00, 0xFC, 0x03, 0x30, 0x02, 0x10, 0x02, 0x10, 0x02, 0xE0, 0x01, 0x00, 0x00,  // Code for char b
        0x06, 0x00, 0x00, 0xE0, 0x01, 0x30, 0x03, 0x10, 0x02, 0x10, 0x02, 0x10, 0x02, 0x00, 0x00,  // Code for char c
        0x06, 0x00, 0x00, 0xE0, 0x01, 0x10, 0x02, 0x10, 0x02, 0x10, 0x01, 0xFC, 0x03, 0x00, 0x00,  // Code for char d
        0x06, 0x00, 0x00, 0xE0, 0x01, 0x50, 0x03, 0x50, 0x02, 0x50, 0x02, 0x60, 0x02, 0x00, 0x00,  // Code for char e
        0x07, 0x20, 0x00, 0x20, 0x00, 0xF8, 0x03, 0x24, 0x00, 0x24, 0x00, 0x24, 0x00, 0x04, 0x00,  // Code for char f
        0x07, 0x00, 0x00, 0xE0, 0x0F, 0x90, 0x0A, 0x90, 0x0A, 0x90, 0x0A, 0x70, 0x06, 0x10, 0x00,  // Code for char g
        0x06, 0x00, 0x00, 0xFC, 0x03, 0x20, 0x00, 0x10, 0x00, 0x10, 0x00, 0xE0, 0x03, 0x00, 0x00,  // Code for char h
        0x06, 0x00, 0x00, 0x10, 0x02, 0x10, 0x02, 0xF6, 0x03, 0x06, 0x02, 0x00, 0x02, 0x00, 0x00,  // Code for char i
        0x05, 0x00, 0x00, 0x10, 0x08, 0x10, 0x08, 0x16, 0x08, 0xF6, 0x07, 0x00, 0x00, 0x00, 0x00,  // Code for char j
        0x06, 0x00, 0x00, 0xFC, 0x03, 0x40, 0x00, 0xA0, 0x00, 0x10, 0x01, 0x00, 0x02, 0x00, 0x00,  // Code for char k
        0x06, 0x00, 0x00, 0x04, 0x02, 0x04, 0x02, 0xFC, 0x03, 0x00, 0x02, 0x00, 0x02, 0x00, 0x00,  // Code for char l
        0x06, 0x00, 0x00, 0xF0, 0x03, 0x10, 0x00, 0xF0, 0x03, 0x10, 0x00, 0xF0, 0x03, 0x00, 0x00,  // Code for char m
        0x06, 0x00, 0x00, 0xF0, 0x03, 0x30, 0x00, 0x10, 0x00, 0x10, 0x00, 0xE0, 0x03, 0x00, 0x00,  // Code for char n
        0x06, 0x00, 0x00, 0xE0, 0x01, 0x10, 0x02, 0x10, 0x02, 0x10, 0x02, 0xE0, 0x01, 0x00, 0x00,  // Code for char o
        0x06, 0x00, 0x00, 0xF0, 0x0F, 0x20, 0x02, 0x10, 0x02, 0x10, 0x02, 0xE0, 0x01, 0x00, 0x00,  // Code for char p
        0x06, 0x00, 0x00, 0xE0, 0x01, 0x10, 0x02, 0x10, 0x02, 0x10, 0x01, 0xF0, 0x0F, 0x00, 0x00,  // Code for char q
        0x06, 0x00, 0x00, 0xF0, 0x03, 0x20, 0x00, 0x10, 0x00, 0x10, 0x00, 0x30, 0x00, 0x00, 0x00,  // Code for char r
        0x05, 0x00, 0x00, 0x60, 0x02, 0x50, 0x02, 0x90, 0x02, 0x90, 0x01, 0x00, 0x00, 0x00, 0x00,  // Code for char s
        0x05, 0x10, 0x00, 0x10, 0x00, 0xFC, 0x03, 0x10, 0x02, 0x10, 0x02, 0x00, 0x00, 0x00, 0x00,  // Code for char t
        0x06, 0x00, 0x00, 0xF0, 0x01, 0x00, 0x02, 0x00, 0x02, 0x00, 0x03, 0xF0, 0x03, 0x00, 0x00,  // Code for char u
        0x06, 0x00, 0x00, 0x70, 0x00, 0x80, 0x01, 0x00, 0x02, 0x80, 0x01, 0x70, 0x00, 0x00, 0x00,  // Code for char v
        0x06, 0x00, 0x00, 0xF0, 0x01, 0x00, 0x03, 0xE0, 0x00, 0x80, 0x01, 0xF0, 0x03, 0x00, 0x00,  // Code for char w
        0x07, 0x00, 0x02, 0x10, 0x03, 0xB0, 0x00, 0x40, 0x00, 0xB0, 0x00, 0x10, 0x03, 0x00, 0x02,  // Code for char x
        0x06, 0x00, 0x08, 0x30, 0x08, 0xC0, 0x0D, 0x00, 0x03, 0xC0, 0x00, 0x30, 0x00, 0x00, 0x00,  // Code for char y
        0x06, 0x00, 0x00, 0x10, 0x02, 0x10, 0x03, 0xD0, 0x02, 0x30, 0x02, 0x10, 0x02, 0x00, 0x00,  // Code for char z
        0x06, 0x00, 0x00, 0x40, 0x00, 0x40, 0x00, 0xB8, 0x07, 0x04, 0x08, 0x04, 0x08, 0x00, 0x00,  // Code for char {
        0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Code for char |
        0x06, 0x00, 0x00, 0x04, 0x08, 0x04, 0x08, 0xB8, 0x07, 0x40, 0x00, 0x40, 0x00, 0x00, 0x00,  // Code for char }
        0x06, 0xC0, 0x00, 0x40, 0x00, 0x40, 0x00, 0x80, 0x00, 0x80, 0x00, 0xC0, 0x00, 0x00, 0x00,  // Code for char ~
        0x03, 0xFC, 0x03, 0x04, 0x02, 0xFC, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00   // Code for char 
        };

