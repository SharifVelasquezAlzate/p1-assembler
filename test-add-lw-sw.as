        lw    0    1    10    Load -3 into r1
        add   1    1    2     Store -3 + -3 = -6 in r2
        add   2    1    7     Store -6 + -3 = -9 in r7
        sw    0    2    10    Store -6 in mem addr 5
        lw    0    1    mem1  Load -6 in r1 (not -3 bc of prev instr)
        sw    0    2    mem1  Store -6 in mem addr 5 but with symbolic addr
        lw    0    3    mem2  Store 50 in r3
        lw    3    4    -40   Store -6 in r4 but with neg offset
        add   4    1    2     Store -9 in r2
        add   4    2    3     Store -15 in r3
        halt
mem1    .fill -3
mem2    .fill 50