    .intel_syntax noprefix

    .text
    .global dot_product

dot_product:

    vxorps ymm0, ymm0, ymm0
    push rdi              
    shr rdi, 3                     

loop_:

    jz rem_

    vmovups ymm1, [rsi]
    vmovups ymm2, [rdx]

    vmulps ymm1, ymm1, ymm2
    vaddps ymm0, ymm0, ymm1

    add rsi, 32
    add rdx, 32

    dec rdi
    jmp loop_

rem_:

    pop rdi
    and rdi, 7

    jz ans_

loop_2_ :

    movss xmm1, [rsi]
    movss xmm2, [rdx]
    mulss xmm1, xmm2                
    addss xmm0, xmm1               

    
    add rsi, 4                     
    add rdx, 4

    dec rdi
    jnz loop_2_

ans_:

    vhaddps ymm0, ymm0, ymm0       
    vhaddps ymm0, ymm0, ymm0
    vextractf128 xmm1, ymm0, 1     
    addss xmm0, xmm1                 

    ret
