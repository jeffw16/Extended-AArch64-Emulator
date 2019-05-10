.global write_char

write_char:
mov x8, #0x40
mov x1, x0
mov x0, #1
mov x2, #1
svc #0x0
ret
