        lw    0    1    mem1  Load -3 into r1
        add   1    1    2     Store -3 + -3 = -6 in r2
        add   2    1    7     Store -6 + -3 = -9 in r7
        sw    7    2    mem1    Store -6 in mem1
        lw    0    1    mem1  Load -6 in r1 (not -3 bc of prev instr)
        sw    0    2    mem1  Store -6 in mem1 but with symbolic addr
        lw    0    3    mem2  Store 50 in r3
        lw    3    4    mem1   Store -6 in r4 but with neg offset
        add   4    1    2     Store -9 in r2
        add   4    2    3     Store -15 in r3
        beq   6    7    endif If reg6 == reg7, end if
        lw    0    6    n14   Setup for jalr test
        lw    0    4    n10
        jalr  4    7
        jalr  4    4          Just PC+1
        noop                  Test of noop
endif   halt
mem1    .fill 3
mem2    .fill 50
n14     .fill 14
n10     .fill 10