  .intel_syntax noprefix

  .text
  .global my_memcpy

my_memcpy:

  push rbp
  mov rbp, rsp

  push rdi

  test edx, edx
  jz end_

loop_:

  cmp edx, 8
  jl rem_

  mov rax, [rsi]
  mov [rdi], rax
  add rdi, 8
  add rsi, 8
  sub edx, 8

  jmp loop_

rem_:

  test edx, edx
  jz end_

  mov al, [rsi]
  mov [rdi], al
  inc rsi
  inc rdi
  sub edx, 1
  jmp rem_


end_:
  pop rax
  mov rsp, rbp
  pop rbp
  ret
