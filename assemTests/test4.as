    lw 0 3 five iterating  r3 = 5
    lw 0 1 neg1             r1 = -1
lb1 beq 3 0 done            if r3 == 0 done
    add 2 3 2               r2 += r3
    add 3 1 3              r3 += -1
    beq 0 0 lb1
    noop
done halt          this function calculates the sum of 5 + 4 + ... + 1 + 0 to r2
five .fill 5
neg1 .fill -1
