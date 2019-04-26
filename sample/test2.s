.global start

.section .text

start:
 sub x20, x20, #1
 mov x16, #71
 sub x15, x15, #10
 strb w16, [x15]
 ldrb w17, [x15]
 strb w17, [x20]
 mov x16, #72
 sub x15, x15, #0x100000
 strb w16, [x15]
 ldrb w17, [x15]
 strb w17, [x20]
 mov x16, #69
 sub x15, x15, #0x200000
 strb w16, [x15]
 ldrb w17, [x15]
 strb w17, [x20]
 add x16, x16, #4
 sub x15, x15, #0x300000
 strb w16, [x15]
 ldrb w17, [x15]
 strb w17, [x20]
 mov x16, #84
 sub x15, x15, #0x400000
 strb w16, [x15]
 ldrb w17, [x15]
 strb w17, [x20]
 mov x16, #72
 sub x15, x15, #0x500000
 strb w16, [x15]
 ldrb w17, [x15]
 strb w17, [x20]
 mov x16, #10
 sub x15, x15, #0xb00000
 strb w16, [x15]
 ldrb w17, [x15]
 strb w17, [x20]
