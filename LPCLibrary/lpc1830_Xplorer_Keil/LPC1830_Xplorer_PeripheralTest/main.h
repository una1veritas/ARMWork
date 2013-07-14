#ifndef __MAIN_H
#define __MAIN_H

void Test_led (void);
void Test_ethernet(void);
void Test_audio(void);
void Test_sdcard(void);

void Emac_init(void);
void Audio_init(void);
void Hid_init (void);
void Sdio_init(void);
void User_switch_init(void);
void print_menu(void);
void GPIO0_IRQHandler(void);

#endif //#ifndef __MAIN_H
