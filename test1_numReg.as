        add     0       1       five    test error checking on numeric registers
        add     0       0       0       test 0 register
        lw      0       1       five
        beq     0       five    five    test numeric registers
start   add     1       2       1       decrement reg1
        beq     0       1       five    goto end of program when reg1==0
        beq     0       0       start   go back to the beginning of the loop
        noop
done    halt                            end of program
five    .fill   5
neg1    .fill   -1
stAddr  .fill   start                   will contain the address of start (2)
