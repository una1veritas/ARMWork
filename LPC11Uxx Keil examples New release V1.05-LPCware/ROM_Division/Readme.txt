Purpose:
To implement ROM division on LPC11U2x

Running mode:
* Make, Download and Debug
* Setup UART for 9600, 8 data bits, no parity, 1 stop bit

Note:
Tested on NGX LPC11U24 board.

Output:
[1] Signed Division
Cycles (Std C):0, [0 Avg]
Cycles (Lib  ):0, [0 Avg] [0% Increase]
[2] Signed Division w Mod.
Cycles (Std C):0, [0 Avg]
Cycles (Lib  ):0, [0 Avg] [0% Increase]
[3] Unsigned Division
Cycles (Std C):0, [0 Avg]
Cycles (Lib  ):0, [0 Avg] [0% Increase]
[4] Unsigned Division w Mod.
Cycles (Std C):0, [0 Avg]
Cycles (Lib  ):0, [0 Avg] [0% Increase]
Testing Complete

**********************************************************************************************************************
*** These are the relevant sections taken from DASM.txt (generated during project build).                        *****
*** They show that tool chain's EABI libraries have been replaced with wrapper functions which use ROM routines. *****
**********************************************************************************************************************

    __aeabi_idiv
        0x00000260:    b570        p.      PUSH     {r4-r6,lr}
        0x00000262:    4604        .F      MOV      r4,r0
        0x00000264:    460e        .F      MOV      r6,r1
        0x00000266:    4819        .H      LDR      r0,[pc,#100] ; [0x2cc] = 0x10000004
        0x00000268:    6800        .h      LDR      r0,[r0,#0]
        0x0000026a:    6800        .h      LDR      r0,[r0,#0]
        0x0000026c:    6905        .i      LDR      r5,[r0,#0x10]
        0x0000026e:    4631        1F      MOV      r1,r6
        0x00000270:    4620         F      MOV      r0,r4
-------------------------------------------------------------------------------------
        0x00000272:    682a        *h      LDR      r2,[r5,#0]		<<<<Table entry 0
-------------------------------------------------------------------------------------
        0x00000274:    4790        .G      BLX      r2
        0x00000276:    bd70        p.      POP      {r4-r6,pc}



    __aeabi_uidiv
        0x00000278:    b570        p.      PUSH     {r4-r6,lr}
        0x0000027a:    4604        .F      MOV      r4,r0
        0x0000027c:    460e        .F      MOV      r6,r1
        0x0000027e:    4813        .H      LDR      r0,[pc,#76] ; [0x2cc] = 0x10000004
        0x00000280:    6800        .h      LDR      r0,[r0,#0]
        0x00000282:    6800        .h      LDR      r0,[r0,#0]
        0x00000284:    6905        .i      LDR      r5,[r0,#0x10]
        0x00000286:    4631        1F      MOV      r1,r6
        0x00000288:    4620         F      MOV      r0,r4
-------------------------------------------------------------------------------------
        0x0000028a:    686a        jh      LDR      r2,[r5,#4]		<<<<Table entry 1
-------------------------------------------------------------------------------------
        0x0000028c:    4790        .G      BLX      r2
        0x0000028e:    bd70        p.      POP      {r4-r6,pc}

    __aeabi_idivmod
        0x000002ae:    b57c        |.      PUSH     {r2-r6,lr}
        0x000002b0:    4604        .F      MOV      r4,r0
        0x000002b2:    460d        .F      MOV      r5,r1
        0x000002b4:    4805        .H      LDR      r0,[pc,#20] ; [0x2cc] = 0x10000004
        0x000002b6:    6800        .h      LDR      r0,[r0,#0]
        0x000002b8:    6800        .h      LDR      r0,[r0,#0]
        0x000002ba:    6906        .i      LDR      r6,[r0,#0x10]
        0x000002bc:    462a        *F      MOV      r2,r5
        0x000002be:    4621        !F      MOV      r1,r4
        0x000002c0:    4668        hF      MOV      r0,sp
-------------------------------------------------------------------------------------
        0x000002c2:    68b3        .h      LDR      r3,[r6,#8]		<<<<Table entry 2
-------------------------------------------------------------------------------------
        0x000002c4:    4798        .G      BLX      r3
        0x000002c6:    9901        ..      LDR      r1,[sp,#4]
        0x000002c8:    9800        ..      LDR      r0,[sp,#0]
        0x000002ca:    bd7c        |.      POP      {r2-r6,pc}

    __aeabi_uidivmod
        0x00000290:    b57c        |.      PUSH     {r2-r6,lr}
        0x00000292:    4604        .F      MOV      r4,r0
        0x00000294:    460d        .F      MOV      r5,r1
        0x00000296:    480d        .H      LDR      r0,[pc,#52] ; [0x2cc] = 0x10000004
        0x00000298:    6800        .h      LDR      r0,[r0,#0]
        0x0000029a:    6800        .h      LDR      r0,[r0,#0]
        0x0000029c:    6906        .i      LDR      r6,[r0,#0x10]
        0x0000029e:    462a        *F      MOV      r2,r5
        0x000002a0:    4621        !F      MOV      r1,r4
        0x000002a2:    4668        hF      MOV      r0,sp
-------------------------------------------------------------------------------------
        0x000002a4:    68f3        .h      LDR      r3,[r6,#0xc]	<<<<Table entry 3
-------------------------------------------------------------------------------------
        0x000002a6:    4798        .G      BLX      r3
        0x000002a8:    9901        ..      LDR      r1,[sp,#4]
        0x000002aa:    9800        ..      LDR      r0,[sp,#0]
        0x000002ac:    bd7c        |.      POP      {r2-r6,pc}