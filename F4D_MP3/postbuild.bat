arm-none-eabi-objcopy -O ihex F4D_MP3.elf F4D_MP3.hex
"C:\Program Files (x86)\STMicroelectronics\STM32 ST-LINK Utility\ST-LINK Utility\ST-LINK_CLI.exe" -c SWD -p F4D_MP3.hex -Rst -Run
