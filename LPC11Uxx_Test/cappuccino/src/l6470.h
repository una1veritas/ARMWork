


extern void l6470_reset();
extern void l6470_kval(unsigned char hold, unsigned char run,unsigned char acc,unsigned char dec);
extern void l6470_slope(unsigned short spd, unsigned char start, unsigned char acc, unsigned char dec);
extern void l6470_run(unsigned char dir, unsigned long a);
extern void l6470_move(unsigned char dir, unsigned long a);

extern unsigned long l6470_getspeed();
