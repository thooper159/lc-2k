    lw 0 1 neg1 iterating var   r1 = -1
    lw 0 2 four    factor 1    r2 = 4
    lw 0 3 six      factor 2   r3 = 6
lb1 beq 2 0 done
    add 4 3 4       r4 += r3
    add 2 1 2       r2 -= 1
    beq 0 0 lb1
    noop
done halt       product of r2 and r3 will be in r4
neg1 .fill -1
four .fill 4
six .fill 6
