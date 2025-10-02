start   lw    0    1    mem1    Load 3 into r1
        nor   0    1    2       00 NOR 11 = 1b111111111111100
        nor   1    1    3       11 NOR 11 = 1b111111111111100
        nor   3    2    4       r4 = 0b11
        add   1    4    5       r5 = 6
        sw    0    2    mem2
        beq   1    4    -3
        beq   2    4    start
        beq   4    1    start
        sw    0    4    mem1
        lw    0    5    mem1
        lw    0    7    mem1
        beq   5    7    skip
        nor   1    1    2       
skip    nor   1    2    3       
        nor   3    1    4       
        add   1    4    5
        noop
        noop
        jalr  1    6    -10
        jalr  1    6    mem3
        halt
mem1    .fill 3
mem2    .fill 0
mem3    .fill -4