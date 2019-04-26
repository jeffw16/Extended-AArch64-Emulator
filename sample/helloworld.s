
helloworld:     file format elf64-littleaarch64


Disassembly of section .init:

00000000004003b0 <_init>:
  4003b0:	a9bf7bfd 	stp	x29, x30, [sp,#-16]!
  4003b4:	910003fd 	mov	x29, sp
  4003b8:	94000038 	bl	400498 <call_weak_fn>
  4003bc:	a8c17bfd 	ldp	x29, x30, [sp],#16
  4003c0:	d65f03c0 	ret

Disassembly of section .plt:

00000000004003d0 <__libc_start_main@plt-0x20>:
  4003d0:	a9bf7bf0 	stp	x16, x30, [sp,#-16]!
  4003d4:	90000090 	adrp	x16, 410000 <__FRAME_END__+0xf9b8>
  4003d8:	f9442e11 	ldr	x17, [x16,#2136]
  4003dc:	91216210 	add	x16, x16, #0x858
  4003e0:	d61f0220 	br	x17
  4003e4:	d503201f 	nop
  4003e8:	d503201f 	nop
  4003ec:	d503201f 	nop

00000000004003f0 <__libc_start_main@plt>:
  4003f0:	90000090 	adrp	x16, 410000 <__FRAME_END__+0xf9b8>
  4003f4:	f9443211 	ldr	x17, [x16,#2144]
  4003f8:	91218210 	add	x16, x16, #0x860
  4003fc:	d61f0220 	br	x17

0000000000400400 <__gmon_start__@plt>:
  400400:	90000090 	adrp	x16, 410000 <__FRAME_END__+0xf9b8>
  400404:	f9443611 	ldr	x17, [x16,#2152]
  400408:	9121a210 	add	x16, x16, #0x868
  40040c:	d61f0220 	br	x17

0000000000400410 <abort@plt>:
  400410:	90000090 	adrp	x16, 410000 <__FRAME_END__+0xf9b8>
  400414:	f9443a11 	ldr	x17, [x16,#2160]
  400418:	9121c210 	add	x16, x16, #0x870
  40041c:	d61f0220 	br	x17

0000000000400420 <printf@plt>:
  400420:	90000090 	adrp	x16, 410000 <__FRAME_END__+0xf9b8>
  400424:	f9443e11 	ldr	x17, [x16,#2168]
  400428:	9121e210 	add	x16, x16, #0x878
  40042c:	d61f0220 	br	x17

Disassembly of section .text:

0000000000400430 <main>:
  400430:	a9bf7bfd 	stp	x29, x30, [sp,#-16]!
  400434:	90000000 	adrp	x0, 400000 <_init-0x3b0>
  400438:	9118e000 	add	x0, x0, #0x638
  40043c:	910003fd 	mov	x29, sp
  400440:	97fffff8 	bl	400420 <printf@plt>
  400444:	52800000 	mov	w0, #0x0                   	// #0
  400448:	a8c17bfd 	ldp	x29, x30, [sp],#16
  40044c:	d65f03c0 	ret

0000000000400450 <_start>:
  400450:	d280001d 	mov	x29, #0x0                   	// #0
  400454:	d280001e 	mov	x30, #0x0                   	// #0
  400458:	aa0003e5 	mov	x5, x0
  40045c:	f94003e1 	ldr	x1, [sp]
  400460:	910023e2 	add	x2, sp, #0x8
  400464:	910003e6 	mov	x6, sp
  400468:	580000c0 	ldr	x0, 400480 <_start+0x30>
  40046c:	580000e3 	ldr	x3, 400488 <_start+0x38>
  400470:	58000104 	ldr	x4, 400490 <_start+0x40>
  400474:	97ffffdf 	bl	4003f0 <__libc_start_main@plt>
  400478:	97ffffe6 	bl	400410 <abort@plt>
  40047c:	00000000 	.inst	0x00000000 ; undefined
  400480:	00400430 	.word	0x00400430
  400484:	00000000 	.word	0x00000000
  400488:	004005a0 	.word	0x004005a0
  40048c:	00000000 	.word	0x00000000
  400490:	00400618 	.word	0x00400618
  400494:	00000000 	.word	0x00000000

0000000000400498 <call_weak_fn>:
  400498:	90000080 	adrp	x0, 410000 <__FRAME_END__+0xf9b8>
  40049c:	f9442000 	ldr	x0, [x0,#2112]
  4004a0:	b4000040 	cbz	x0, 4004a8 <call_weak_fn+0x10>
  4004a4:	17ffffd7 	b	400400 <__gmon_start__@plt>
  4004a8:	d65f03c0 	ret
  4004ac:	00000000 	.inst	0x00000000 ; undefined

00000000004004b0 <deregister_tm_clones>:
  4004b0:	90000081 	adrp	x1, 410000 <__FRAME_END__+0xf9b8>
  4004b4:	90000080 	adrp	x0, 410000 <__FRAME_END__+0xf9b8>
  4004b8:	91224021 	add	x1, x1, #0x890
  4004bc:	91224000 	add	x0, x0, #0x890
  4004c0:	91001c21 	add	x1, x1, #0x7
  4004c4:	cb000021 	sub	x1, x1, x0
  4004c8:	f100383f 	cmp	x1, #0xe
  4004cc:	540000a9 	b.ls	4004e0 <deregister_tm_clones+0x30>
  4004d0:	580000c1 	ldr	x1, 4004e8 <deregister_tm_clones+0x38>
  4004d4:	b4000061 	cbz	x1, 4004e0 <deregister_tm_clones+0x30>
  4004d8:	d61f0020 	br	x1
  4004dc:	d503201f 	nop
  4004e0:	d65f03c0 	ret
  4004e4:	d503201f 	nop
	...

00000000004004f0 <register_tm_clones>:
  4004f0:	90000081 	adrp	x1, 410000 <__FRAME_END__+0xf9b8>
  4004f4:	90000080 	adrp	x0, 410000 <__FRAME_END__+0xf9b8>
  4004f8:	91224021 	add	x1, x1, #0x890
  4004fc:	91224000 	add	x0, x0, #0x890
  400500:	cb000021 	sub	x1, x1, x0
  400504:	9343fc21 	asr	x1, x1, #3
  400508:	8b41fc21 	add	x1, x1, x1, lsr #63
  40050c:	9341fc21 	asr	x1, x1, #1
  400510:	b4000081 	cbz	x1, 400520 <register_tm_clones+0x30>
  400514:	580000a2 	ldr	x2, 400528 <register_tm_clones+0x38>
  400518:	b4000042 	cbz	x2, 400520 <register_tm_clones+0x30>
  40051c:	d61f0040 	br	x2
  400520:	d65f03c0 	ret
  400524:	d503201f 	nop
	...

0000000000400530 <__do_global_dtors_aux>:
  400530:	a9be7bfd 	stp	x29, x30, [sp,#-32]!
  400534:	910003fd 	mov	x29, sp
  400538:	f9000bf3 	str	x19, [sp,#16]
  40053c:	90000093 	adrp	x19, 410000 <__FRAME_END__+0xf9b8>
  400540:	39624260 	ldrb	w0, [x19,#2192]
  400544:	35000080 	cbnz	w0, 400554 <__do_global_dtors_aux+0x24>
  400548:	97ffffda 	bl	4004b0 <deregister_tm_clones>
  40054c:	52800020 	mov	w0, #0x1                   	// #1
  400550:	39224260 	strb	w0, [x19,#2192]
  400554:	f9400bf3 	ldr	x19, [sp,#16]
  400558:	a8c27bfd 	ldp	x29, x30, [sp],#32
  40055c:	d65f03c0 	ret

0000000000400560 <frame_dummy>:
  400560:	90000080 	adrp	x0, 410000 <__FRAME_END__+0xf9b8>
  400564:	91198000 	add	x0, x0, #0x660
  400568:	f9400001 	ldr	x1, [x0]
  40056c:	b5000061 	cbnz	x1, 400578 <frame_dummy+0x18>
  400570:	17ffffe0 	b	4004f0 <register_tm_clones>
  400574:	d503201f 	nop
  400578:	58000101 	ldr	x1, 400598 <frame_dummy+0x38>
  40057c:	b4ffffa1 	cbz	x1, 400570 <frame_dummy+0x10>
  400580:	a9bf7bfd 	stp	x29, x30, [sp,#-16]!
  400584:	910003fd 	mov	x29, sp
  400588:	d63f0020 	blr	x1
  40058c:	a8c17bfd 	ldp	x29, x30, [sp],#16
  400590:	17ffffd8 	b	4004f0 <register_tm_clones>
  400594:	d503201f 	nop
	...

00000000004005a0 <__libc_csu_init>:
  4005a0:	a9bc7bfd 	stp	x29, x30, [sp,#-64]!
  4005a4:	910003fd 	mov	x29, sp
  4005a8:	a9025bf5 	stp	x21, x22, [sp,#32]
  4005ac:	90000095 	adrp	x21, 410000 <__FRAME_END__+0xf9b8>
  4005b0:	a90153f3 	stp	x19, x20, [sp,#16]
  4005b4:	90000094 	adrp	x20, 410000 <__FRAME_END__+0xf9b8>
  4005b8:	911942b5 	add	x21, x21, #0x650
  4005bc:	91196294 	add	x20, x20, #0x658
  4005c0:	cb150294 	sub	x20, x20, x21
  4005c4:	a90363f7 	stp	x23, x24, [sp,#48]
  4005c8:	aa0203f6 	mov	x22, x2
  4005cc:	2a0003f8 	mov	w24, w0
  4005d0:	aa0103f7 	mov	x23, x1
  4005d4:	97ffff77 	bl	4003b0 <_init>
  4005d8:	9343fe94 	asr	x20, x20, #3
  4005dc:	b4000154 	cbz	x20, 400604 <__libc_csu_init+0x64>
  4005e0:	d2800013 	mov	x19, #0x0                   	// #0
  4005e4:	f8737aa3 	ldr	x3, [x21,x19,lsl #3]
  4005e8:	aa1603e2 	mov	x2, x22
  4005ec:	aa1703e1 	mov	x1, x23
  4005f0:	2a1803e0 	mov	w0, w24
  4005f4:	91000673 	add	x19, x19, #0x1
  4005f8:	d63f0060 	blr	x3
  4005fc:	eb14027f 	cmp	x19, x20
  400600:	54ffff21 	b.ne	4005e4 <__libc_csu_init+0x44>
  400604:	a94153f3 	ldp	x19, x20, [sp,#16]
  400608:	a9425bf5 	ldp	x21, x22, [sp,#32]
  40060c:	a94363f7 	ldp	x23, x24, [sp,#48]
  400610:	a8c47bfd 	ldp	x29, x30, [sp],#64
  400614:	d65f03c0 	ret

0000000000400618 <__libc_csu_fini>:
  400618:	d65f03c0 	ret

Disassembly of section .fini:

000000000040061c <_fini>:
  40061c:	a9bf7bfd 	stp	x29, x30, [sp,#-16]!
  400620:	910003fd 	mov	x29, sp
  400624:	a8c17bfd 	ldp	x29, x30, [sp],#16
  400628:	d65f03c0 	ret
