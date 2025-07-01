  .text
  .global longest_inc_subseq

longest_inc_subseq:
  stp x29, x30, [sp, -16]!
  mov x29, sp

  mov x3, 0
  cmp x3, x2
  b.eq end_of_external_loop_

  loop_external_:
  mov x5, 1
  lsl x3, x3, 3
  add x1, x1, x3
  str x5, [x1]
  sub x1, x1, x3
  lsr x3, x3, 3

  mov x4, 0
  cmp x4, x3
  b.eq end_of_external_loop_

  loop_:
  lsl x3, x3, 3
  lsl x4, x4, 3

  add x0, x0, x3
  ldr x5, [x0]
  sub x0, x0, x3
  add x0, x0, x4
  ldr x6, [x0]
  sub x0, x0, x4
  cmp x5, x6
  b.le end_of_loop_

  add x1, x1, x3
  ldr x5, [x1]
  sub x1, x1, x3
  add x1, x1, x4
  ldr x6, [x1]
  sub x1, x1, x4
  cmp x5, x6
  b.gt end_of_loop_

  add x6, x6, 1
  add x1, x1, x3
  str x6, [x1]
  sub x1, x1, x3

  end_of_loop_:
  lsr x3, x3, 3
  lsr x4, x4, 3
  add x4, x4, 1
  cmp x4, x3
  b.lt loop_

  end_of_external_loop_:
  add x3, x3, 1
  cmp x3, x2
  b.lt loop_external_

  mov x3, 0
  mov x0, 0
  cmp x3, x2
  b.eq end_

  loop_max_:

  lsl x3, x3, 3
  add x1, x1, x3
  ldr x5, [x1]
  cmp x0, x5           
  csel x0, x5, x0, lt
  sub x1, x1, x3
  lsr x3, x3, 3
  add x3, x3, 1
  cmp x3, x2
  b.lt loop_max_

  end_:
  ldp x29, x30, [sp], 16
  ret
