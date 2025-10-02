        lw      0       1       mcand       load reg1 with first number
        lw      0       2       mplier      load reg2 with second number
        lw      0       4       n1          sync with bit counter (reg5 = 0) 
bitmul  lw      0       6       n16         reg6 = 16
        beq     5       6       done        if reg5 == 16 (out of bounds bit), finish
        nor     2       2       2           Flip bits of reg2
        nor     4       4       4           Flip bits of reg4
        nor     2       4       6           Compute OGreg2 && OGreg4
        nor     2       2       2           Flip back bits of reg2
        nor     4       4       4           Flip back bits of reg4
ifbit   beq     6       0       eifbit      If bit of secnum at pos reg5 is 0, skip mult by 2^reg5
forst   lw      0       7       n0          reg7 = 0
        lw      0       6       n1          reg6 = 1
nextit  beq     7       5       efor        If reg7 == reg5, end for loop
        add     1       1       1           reg1 = 2*reg1
        add     7       6       7           ++reg7
        beq     0       0       nextit      go back to beginnning of loop
efor    add     3       1       3           reg3 += 2^{reg5}*firstnum
        lw      0       1       mcand       Reset reg1 to the original number
eifbit  lw      0       6       n1          reg6 = 1
        add     5       6       5           ++reg5
        add     4       4       4           reg4 = 2*reg4 -- sync with reg5
        beq     0       0       bitmul      Continue with next bit of secnum
done    halt
nn1     .fill -1
n0      .fill 0
n1      .fill 1
n16     .fill 16
mcand   .fill 2563
mplier  .fill 5984