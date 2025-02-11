fib(int, int*):
        ble     a0,zero,.L1
        sw      zero,0(a1)
        li      a5,1
        beq     a0,a5,.L1
        sw      a5,4(a1)
        li      a2,2
        beq     a0,a2,.L1
        li      a4,4
        ble     a0,a4,.L7
        addi    a6,a0,-5
        andi    a6,a6,-2
        add     a6,a6,a4
        addi    a3,a1,8
        li      a4,0
.L5:
        add     a4,a4,a5
        add     a5,a5,a4
        sw      a5,4(a3)
        sw      a4,0(a3)
        addi    a2,a2,2
        addi    a3,a3,8
        bne     a6,a2,.L5
.L4:
        slli    a5,a6,2
        addi    a5,a5,-4
        add     a5,a1,a5
        lw      a3,-4(a5)
        lw      a2,0(a5)
        addi    a4,a6,1
        add     a3,a3,a2
        sw      a3,4(a5)
        ble     a0,a4,.L1
        slli    a6,a6,2
        add     a1,a1,a6
        lw      a5,0(a1)
        add     a5,a5,a2
        sw      a5,4(a1)
.L1:
        ret
.L7:
        mv      a6,a2
        j       .L4