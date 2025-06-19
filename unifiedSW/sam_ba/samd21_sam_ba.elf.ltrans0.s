	.cpu cortex-m0plus
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 4
	.eabi_attribute 34, 0
	.file	"<artificial>"
	.text
	.align	1
	.syntax unified
	.code	16
	.thumb_func
	.fpu softvfp
	.type	serial_is_rx_ready, %function
serial_is_rx_ready:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r3, .L2
	@ sp needed
	ldrb	r0, [r3, #24]
	lsls	r0, r0, #29
	lsrs	r0, r0, #31
	bx	lr
.L3:
	.align	2
.L2:
	.word	1107298304
	.size	serial_is_rx_ready, .-serial_is_rx_ready
	.align	1
	.syntax unified
	.code	16
	.thumb_func
	.fpu softvfp
	.type	serial_getc, %function
serial_getc:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, lr}
.L5:
	bl	serial_is_rx_ready
	cmp	r0, #0
	beq	.L5
	ldr	r3, .L12
	movs	r2, r3
.L6:
	ldrb	r1, [r3, #24]
	lsls	r1, r1, #29
	bpl	.L6
	ldrh	r1, [r3, #26]
	lsls	r1, r1, #31
	bmi	.L7
	ldrh	r1, [r3, #26]
	lsls	r1, r1, #30
	bmi	.L7
	ldrh	r3, [r3, #26]
.L7:
	ldrh	r0, [r2, #40]
	@ sp needed
	uxtb	r0, r0
	pop	{r4, pc}
.L13:
	.align	2
.L12:
	.word	1107298304
	.size	serial_getc, .-serial_getc
	.align	1
	.syntax unified
	.code	16
	.thumb_func
	.fpu softvfp
	.type	getbytes, %function
getbytes:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, r6, r7, lr}
	movs	r5, #0
	movs	r6, r0
	movs	r7, r1
	movs	r4, r5
.L18:
	bl	serial_getc
	ldr	r2, .L25
	ldrb	r3, [r2]
	cmp	r3, #0
	bne	.L19
	movs	r1, #255
	lsrs	r3, r4, #8
	ands	r1, r0
	eors	r3, r1
	lsls	r1, r4, #8
	ldr	r4, .L25+4
	lsls	r3, r3, #1
	ldrh	r4, [r3, r4]
	ldrh	r3, [r2, #2]
	eors	r4, r1
	uxth	r4, r4
	cmp	r3, #0
	bne	.L16
	ldrb	r2, [r2, #4]
	cmp	r2, #0
	beq	.L17
.L16:
	strb	r0, [r6]
	adds	r6, r6, #1
	cmp	r7, #128
	bne	.L17
	ldr	r2, .L25
	subs	r3, r3, #1
	strh	r3, [r2, #2]
.L17:
	adds	r5, r5, #1
	uxth	r5, r5
	cmp	r7, r5
	bne	.L18
.L15:
	movs	r0, r4
	@ sp needed
	pop	{r3, r4, r5, r6, r7, pc}
.L19:
	movs	r4, #1
	b	.L15
.L26:
	.align	2
.L25:
	.word	.LANCHOR0
	.word	crc16Table
	.size	getbytes, .-getbytes
	.align	1
	.syntax unified
	.code	16
	.thumb_func
	.fpu softvfp
	.type	serial_getdata, %function
serial_getdata:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, lr}
	movs	r4, r0
	bl	serial_getc
	@ sp needed
	strb	r0, [r4]
	movs	r0, #1
	pop	{r4, pc}
	.size	serial_getdata, .-serial_getdata
	.align	1
	.syntax unified
	.code	16
	.thumb_func
	.fpu softvfp
	.type	serial_putc, %function
serial_putc:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r3, .L32
	uxtb	r0, r0
.L29:
	ldrb	r2, [r3, #24]
	lsls	r2, r2, #31
	bpl	.L29
	uxth	r0, r0
	strh	r0, [r3, #40]
	@ sp needed
	movs	r0, #1
	bx	lr
.L33:
	.align	2
.L32:
	.word	1107298304
	.size	serial_putc, .-serial_putc
	.align	1
	.syntax unified
	.code	16
	.thumb_func
	.fpu softvfp
	.type	serial_getdata_xmd, %function
serial_getdata_xmd:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 0, uses_anonymous_args = 0
	movs	r3, #0
	push	{r4, r5, r6, r7, lr}
	ldr	r5, .L55
	sub	sp, sp, #20
	movs	r7, r0
	strb	r3, [r5]
	cmp	r1, r3
	bne	.L35
	adds	r3, r3, #1
.L53:
	movs	r4, #100
	strb	r3, [r5, #4]
.L43:
	movs	r0, #67
	bl	serial_putc
	ldr	r6, .L55+4
.L37:
	bl	serial_is_rx_ready
	cmp	r0, #0
	bne	.L38
	cmp	r6, #0
	bne	.L39
.L40:
	subs	r4, r4, #1
	cmp	r4, #0
	bne	.L43
	movs	r0, r4
	b	.L34
.L35:
	strh	r1, [r5, #2]
	b	.L53
.L39:
	subs	r6, r6, #1
	b	.L37
.L38:
	cmp	r6, #0
	beq	.L40
	movs	r6, #1
.L41:
	bl	serial_getc
	ldrb	r3, [r5]
	cmp	r3, #0
	beq	.L44
	movs	r0, #0
	strb	r0, [r5]
.L34:
	add	sp, sp, #20
	@ sp needed
	pop	{r4, r5, r6, r7, pc}
.L44:
	movs	r3, #255
	ands	r0, r3
	cmp	r0, #1
	beq	.L46
	cmp	r0, #4
	beq	.L47
.L45:
	movs	r3, #0
	movs	r0, #1
	strb	r3, [r5, #4]
	b	.L34
.L46:
	movs	r1, #2
	add	r0, sp, #12
	bl	getbytes
	movs	r1, #128
	movs	r0, r7
	bl	getbytes
	str	r0, [sp, #4]
	bl	serial_getc
	lsls	r0, r0, #8
	uxth	r4, r0
	bl	serial_getc
	ldr	r3, [sp, #4]
	adds	r4, r4, r0
	uxth	r4, r4
	cmp	r3, r4
	bne	.L48
	add	r3, sp, #8
	ldrb	r3, [r3, #4]
	cmp	r3, r6
	bne	.L48
	mvns	r3, r6
	add	r2, sp, #8
	ldrb	r2, [r2, #5]
	uxtb	r3, r3
	cmp	r2, r3
	beq	.L49
.L48:
	movs	r0, #24
.L54:
	bl	serial_putc
	b	.L45
.L49:
	movs	r0, #6
	adds	r6, r6, #1
	bl	serial_putc
	uxtb	r6, r6
	adds	r7, r7, #128
	b	.L41
.L47:
	movs	r0, #6
	b	.L54
.L56:
	.align	2
.L55:
	.word	.LANCHOR0
	.word	800000
	.size	serial_getdata_xmd, .-serial_getdata_xmd
	.align	1
	.syntax unified
	.code	16
	.thumb_func
	.fpu softvfp
	.type	serial_putdata_xmd, %function
serial_putdata_xmd:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 0, uses_anonymous_args = 0
	movs	r3, #0
	push	{r4, r5, r6, r7, lr}
	ldr	r5, .L92
	sub	sp, sp, #20
	str	r0, [sp, #12]
	movs	r4, r1
	strb	r3, [r5]
	cmp	r1, r3
	bne	.L58
	adds	r3, r3, #1
.L90:
	strb	r3, [r5, #4]
	movs	r3, #127
	tst	r4, r3
	beq	.L61
	adds	r4, r4, #128
	bics	r4, r3
.L61:
	bl	serial_getc
	ldrb	r3, [r5]
	cmp	r3, #0
	beq	.L91
	movs	r3, #0
	strb	r3, [r5]
	bl	serial_getc
.L91:
	uxtb	r0, r0
	cmp	r0, #67
	beq	.L74
	cmp	r0, #113
	beq	.L73
	cmp	r0, #21
	bne	.L61
.L74:
	movs	r3, #1
	str	r3, [sp, #4]
.L62:
	movs	r0, #1
	bl	serial_putc
	ldr	r0, [sp, #4]
	bl	serial_putc
	ldr	r3, [sp, #4]
	movs	r5, #128
	mvns	r0, r3
	movs	r7, #0
	uxtb	r0, r0
	bl	serial_putc
	ldr	r3, [sp, #12]
	str	r3, [sp, #8]
.L70:
	ldr	r2, .L92
	ldrh	r3, [r2, #2]
	cmp	r3, #0
	bne	.L68
	ldrb	r6, [r2, #4]
	cmp	r6, #0
	beq	.L69
.L68:
	ldr	r1, [sp, #8]
	subs	r3, r3, #1
	strh	r3, [r2, #2]
	movs	r3, r1
	ldrb	r6, [r1]
	adds	r3, r3, #1
	str	r3, [sp, #8]
.L69:
	movs	r0, r6
	bl	serial_putc
	lsrs	r3, r7, #8
	eors	r6, r3
	ldr	r3, .L92+4
	lsls	r6, r6, #1
	ldrh	r6, [r6, r3]
	lsls	r7, r7, #8
	eors	r6, r7
	sxth	r6, r6
	subs	r5, r5, #1
	uxth	r7, r6
	cmp	r5, #0
	bne	.L70
	lsrs	r0, r7, #8
	bl	serial_putc
	uxtb	r0, r6
	bl	serial_putc
	bl	serial_getc
	ldr	r6, .L92
	ldrb	r3, [r6]
	cmp	r3, #0
	beq	.L89
	strb	r5, [r6]
.L73:
	movs	r0, #0
	b	.L57
.L58:
	strh	r1, [r5, #2]
	b	.L90
.L89:
	movs	r3, #255
	ands	r0, r3
	cmp	r0, #6
	bne	.L66
	ldr	r3, [sp, #4]
	subs	r4, r4, #128
	adds	r3, r3, #1
	uxtb	r3, r3
	str	r3, [sp, #4]
	ldr	r3, [sp, #12]
	adds	r3, r3, #128
	str	r3, [sp, #12]
.L66:
	cmp	r4, #0
	bne	.L62
	movs	r0, #4
	bl	serial_putc
	bl	serial_getc
	movs	r0, #1
	strb	r4, [r6, #4]
.L57:
	add	sp, sp, #20
	@ sp needed
	pop	{r4, r5, r6, r7, pc}
.L93:
	.align	2
.L92:
	.word	.LANCHOR0
	.word	crc16Table
	.size	serial_putdata_xmd, .-serial_putdata_xmd
	.align	1
	.syntax unified
	.code	16
	.thumb_func
	.fpu softvfp
	.type	serial_putdata, %function
serial_putdata:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, lr}
	movs	r4, r0
	movs	r5, r1
	adds	r6, r0, r1
.L95:
	cmp	r4, r6
	bne	.L96
	movs	r0, r5
	@ sp needed
	pop	{r4, r5, r6, pc}
.L96:
	ldrb	r0, [r4]
	bl	serial_putc
	adds	r4, r4, #1
	b	.L95
	.size	serial_putdata, .-serial_putdata
	.align	1
	.syntax unified
	.code	16
	.thumb_func
	.fpu softvfp
	.type	sam_ba_putdata.isra.0, %function
sam_ba_putdata.isra.0:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	movs	r3, r0
	push	{r4, lr}
	movs	r0, r1
	movs	r1, r2
	blx	r3
	movs	r2, #128
	ldr	r3, .L98
	lsls	r2, r2, #20
	str	r2, [r3, #20]
	movs	r2, #100
	ldr	r3, .L98+4
	@ sp needed
	strh	r2, [r3, #6]
	pop	{r4, pc}
.L99:
	.align	2
.L98:
	.word	1090536448
	.word	.LANCHOR0
	.size	sam_ba_putdata.isra.0, .-sam_ba_putdata.isra.0
	.thumb_set sam_ba_putdata_xmd.isra.2,sam_ba_putdata.isra.0
	.align	1
	.syntax unified
	.code	16
	.thumb_func
	.fpu softvfp
	.type	eraseFlash, %function
eraseFlash:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r3, .L106
	push	{r4, r5, lr}
	ldr	r1, [r3, #12]
	str	r0, [r3, #8]
	ldr	r3, [r3, #16]
	ldr	r2, .L106+4
	ldr	r4, .L106+8
	lsls	r3, r3, #2
.L101:
	cmp	r1, r0
	bhi	.L103
	@ sp needed
	pop	{r4, r5, pc}
.L103:
	lsrs	r5, r0, #1
	str	r5, [r2, #28]
	strh	r4, [r2]
.L102:
	ldrb	r5, [r2, #20]
	lsls	r5, r5, #31
	bpl	.L102
	adds	r0, r0, r3
	b	.L101
.L107:
	.align	2
.L106:
	.word	.LANCHOR0
	.word	1090535424
	.word	-23294
	.size	eraseFlash, .-eraseFlash
	.align	1
	.syntax unified
	.code	16
	.thumb_func
	.fpu softvfp
	.type	sam_ba_putdata_term, %function
sam_ba_putdata_term:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, lr}
	ldr	r3, .L120
	movs	r2, r1
	ldrb	r1, [r3, #20]
	sub	sp, sp, #20
	ldr	r4, [r3, #24]
	cmp	r1, #0
	beq	.L109
	cmp	r2, #4
	bne	.L110
	ldr	r0, [r0]
.L111:
	movs	r6, #15
	movs	r7, #48
	lsls	r2, r2, #1
	add	r3, sp, #16
	adds	r3, r3, r2
	subs	r3, r3, #11
	subs	r5, r3, r2
.L115:
	movs	r1, r0
	ands	r1, r6
	cmp	r1, #9
	bhi	.L113
	orrs	r1, r7
.L118:
	strb	r1, [r3]
	subs	r3, r3, #1
	lsrs	r0, r0, #4
	cmp	r5, r3
	bne	.L115
	movs	r0, #10
	ldr	r3, .L120+4
	add	r1, sp, #4
	strh	r3, [r1]
	adds	r3, r1, r2
	strb	r0, [r3, #2]
	adds	r0, r0, #3
	strb	r0, [r3, #3]
	adds	r2, r2, #4
.L119:
	ldr	r0, [r4, #12]
	bl	sam_ba_putdata.isra.0
	add	sp, sp, #20
	@ sp needed
	pop	{r4, r5, r6, r7, pc}
.L110:
	cmp	r2, #2
	bne	.L112
	ldrh	r0, [r0]
	b	.L111
.L112:
	ldrb	r0, [r0]
	b	.L111
.L113:
	adds	r1, r1, #55
	b	.L118
.L109:
	movs	r1, r0
	b	.L119
.L121:
	.align	2
.L120:
	.word	.LANCHOR0
	.word	30768
	.size	sam_ba_putdata_term, .-sam_ba_putdata_term
	.align	1
	.syntax unified
	.code	16
	.thumb_func
	.fpu softvfp
	.type	SysTick_Handler, %function
SysTick_Handler:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r2, .L133
	ldr	r3, .L133+4
	ldrb	r1, [r2, #28]
	cmp	r1, #0
	bne	.L123
	ldrb	r1, [r3]
	ldrb	r0, [r3, #1]
	adds	r1, r1, r0
	movs	r0, #128
	uxtb	r1, r1
	strb	r1, [r3, #1]
	ldr	r1, .L133+8
	lsls	r0, r0, #10
	str	r0, [r1, #28]
.L123:
	ldrb	r1, [r2, #28]
	adds	r1, r1, #1
	uxtb	r1, r1
	strb	r1, [r2, #28]
	ldrb	r1, [r3, #1]
	cmp	r1, #240
	bhi	.L124
	ldrb	r1, [r3, #1]
	cmp	r1, #9
	bhi	.L125
.L124:
	ldrb	r1, [r3]
	rsbs	r1, r1, #0
	sxtb	r1, r1
	strb	r1, [r3]
	ldrb	r1, [r3]
	ldrb	r0, [r3, #1]
	adds	r1, r1, r0
	uxtb	r1, r1
	strb	r1, [r3, #1]
.L125:
	ldrb	r1, [r2, #28]
	ldrb	r3, [r3, #1]
	cmp	r1, r3
	bne	.L126
	movs	r1, #128
	ldr	r3, .L133+8
	lsls	r1, r1, #10
	str	r1, [r3, #28]
.L126:
	ldrh	r3, [r2, #6]
	cmp	r3, #0
	beq	.L128
	ldrh	r3, [r2, #6]
	subs	r3, r3, #1
	uxth	r3, r3
	strh	r3, [r2, #6]
	cmp	r3, #0
	bne	.L128
	movs	r1, #128
	ldr	r3, .L133+8
	lsls	r1, r1, #20
	str	r1, [r3, #24]
.L128:
	ldrh	r3, [r2, #30]
	cmp	r3, #0
	beq	.L122
	ldrh	r3, [r2, #30]
	subs	r3, r3, #1
	uxth	r3, r3
	strh	r3, [r2, #30]
	cmp	r3, #0
	bne	.L122
	movs	r2, #8
	ldr	r3, .L133+12
	str	r2, [r3]
.L122:
	@ sp needed
	bx	lr
.L134:
	.align	2
.L133:
	.word	.LANCHOR0
	.word	.LANCHOR1
	.word	1090536448
	.word	1090536600
	.size	SysTick_Handler, .-SysTick_Handler
	.align	1
	.syntax unified
	.code	16
	.thumb_func
	.fpu softvfp
	.type	PendSV_Handler, %function
PendSV_Handler:
	@ Volatile: function does not return.
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	.syntax divided
@ 139 "board_startup.c" 1
	bkpt 2
@ 0 "" 2
	.thumb
	.syntax unified
.L136:
	b	.L136
	.size	PendSV_Handler, .-PendSV_Handler
	.align	1
	.syntax unified
	.code	16
	.thumb_func
	.fpu softvfp
	.type	SVC_Handler, %function
SVC_Handler:
	@ Volatile: function does not return.
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	.syntax divided
@ 133 "board_startup.c" 1
	bkpt 5
@ 0 "" 2
	.thumb
	.syntax unified
.L138:
	b	.L138
	.size	SVC_Handler, .-SVC_Handler
	.align	1
	.syntax unified
	.code	16
	.thumb_func
	.fpu softvfp
	.type	HardFault_Handler, %function
HardFault_Handler:
	@ Volatile: function does not return.
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	.syntax divided
@ 127 "board_startup.c" 1
	bkpt 13
@ 0 "" 2
	.thumb
	.syntax unified
.L140:
	b	.L140
	.size	HardFault_Handler, .-HardFault_Handler
	.align	1
	.syntax unified
	.code	16
	.thumb_func
	.fpu softvfp
	.type	NMI_Handler, %function
NMI_Handler:
	@ Volatile: function does not return.
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	.syntax divided
@ 121 "board_startup.c" 1
	bkpt 14
@ 0 "" 2
	.thumb
	.syntax unified
.L142:
	b	.L142
	.size	NMI_Handler, .-NMI_Handler
	.align	1
	.syntax unified
	.code	16
	.thumb_func
	.fpu softvfp
	.type	dfll_sync, %function
dfll_sync:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	movs	r3, #16
	ldr	r1, .L147
.L144:
	ldr	r2, [r1, #12]
	tst	r2, r3
	beq	.L144
	@ sp needed
	bx	lr
.L148:
	.align	2
.L147:
	.word	1073743872
	.size	dfll_sync, .-dfll_sync
	.align	1
	.syntax unified
	.code	16
	.thumb_func
	.fpu softvfp
	.type	gclk_sync, %function
gclk_sync:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r2, .L152
.L150:
	ldrb	r3, [r2, #1]
	sxtb	r3, r3
	cmp	r3, #0
	blt	.L150
	@ sp needed
	bx	lr
.L153:
	.align	2
.L152:
	.word	1073744896
	.size	gclk_sync, .-gclk_sync
	.align	1
	.syntax unified
	.code	16
	.thumb_func
	.fpu softvfp
	.type	USB_SendZlp, %function
USB_SendZlp:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	ldr	r2, .L158
	adds	r0, r0, #255
	ldr	r3, [r2, #20]
	lsrs	r3, r3, #14
	lsls	r3, r3, #14
	str	r3, [r2, #20]
	movs	r3, #2
	strb	r3, [r0, #8]
	ldrb	r2, [r0, #6]
	subs	r3, r3, #130
	orrs	r3, r2
	strb	r3, [r0, #6]
.L155:
	ldrb	r3, [r0, #8]
	lsls	r3, r3, #30
	bpl	.L155
	@ sp needed
	bx	lr
.L159:
	.align	2
.L158:
	.word	usb_endpoint_table
	.size	USB_SendZlp, .-USB_SendZlp
	.align	1
	.syntax unified
	.code	16
	.thumb_func
	.fpu softvfp
	.type	sam_ba_monitor_run, %function
sam_ba_monitor_run:
	@ Volatile: function does not return.
	@ args = 0, pretend = 0, frame = 56
	@ frame_needed = 1, uses_anonymous_args = 0
	movs	r2, #16
	movs	r3, #8
	push	{r4, r5, r6, r7, lr}
	sub	sp, sp, #60
	add	r7, sp, #0
	adds	r2, r7, r2
	adds	r1, r2, r3
	movs	r3, r1
	ldr	r2, .L259
	ldmia	r2!, {r0, r4, r5}
	stmia	r3!, {r0, r4, r5}
	ldmia	r2!, {r0, r4, r5}
	stmia	r3!, {r0, r4, r5}
	ldmia	r2!, {r0, r4}
	stmia	r3!, {r0, r4}
	ldr	r2, .L259+4
	ldr	r4, .L259+8
	ldr	r3, [r2, #8]
	lsls	r3, r3, #13
	lsrs	r3, r3, #29
	lsls	r3, r3, #2
	ldr	r3, [r3, r1]
	str	r3, [r4, #16]
	ldr	r1, [r2, #8]
	uxth	r1, r1
	muls	r3, r1
	str	r3, [r4, #12]
	ldrh	r3, [r2, #24]
	adds	r1, r4, #1
	lsls	r3, r3, #23
	lsrs	r3, r3, #31
	strb	r3, [r1, #31]
	movs	r3, #0
	str	r3, [r4, #36]
	movs	r3, r4
	movs	r1, #122
	adds	r3, r3, #40
	strb	r1, [r3]
	str	r2, [r7, #12]
.L219:
	ldr	r3, [r4, #24]
	movs	r1, #64
	ldr	r3, [r3, #16]
	ldr	r0, .L259+12
	blx	r3
	cmp	r0, #0
	beq	.L161
	movs	r2, #8
	ldr	r3, .L259+16
	str	r2, [r3]
	movs	r3, #100
	strh	r3, [r4, #30]
.L161:
	ldr	r3, .L259+12
	str	r0, [r4, #108]
	str	r3, [r4, #112]
	movs	r3, #0
	str	r3, [r4, #116]
.L162:
	ldr	r3, [r4, #116]
	ldr	r2, [r4, #108]
	cmp	r3, r2
	bcs	.L219
	ldr	r3, [r4, #112]
	ldrb	r3, [r3]
	cmp	r3, #255
	beq	.L164
	cmp	r3, #35
	beq	.LCB1016
	b	.L165	@long jump
.LCB1016:
	ldrb	r3, [r4, #20]
	cmp	r3, #0
	beq	.L166
	ldr	r3, [r4, #24]
	movs	r2, #2
	ldr	r1, .L259+20
	ldr	r0, [r3, #12]
	bl	sam_ba_putdata.isra.0
.L166:
	movs	r3, r4
	adds	r3, r3, #40
	ldrb	r3, [r3]
	cmp	r3, #83
	bne	.L167
	ldr	r3, [r4, #108]
	ldr	r5, [r4, #116]
	cmp	r3, r5
	bls	.L168
	ldr	r1, [r4, #112]
	ldr	r2, [r4, #120]
	adds	r5, r5, #1
	adds	r6, r1, #1
	str	r6, [r4, #112]
	str	r5, [r4, #116]
	subs	r3, r3, r5
	cmp	r3, r2
	bcs	.L169
	str	r3, [r4, #124]
.L170:
	ldr	r3, [r4, #124]
	movs	r1, r6
	movs	r2, r3
	ldr	r0, [r4, #36]
	str	r3, [r7, #8]
	bl	memcpy
	ldr	r3, [r7, #8]
	ldrb	r2, [r7, #8]
	adds	r5, r5, r3
	adds	r6, r6, r3
	ldr	r3, .L259+24
	str	r5, [r4, #116]
	str	r6, [r4, #112]
	strb	r2, [r3]
.L168:
	ldr	r3, [r4, #116]
	ldr	r1, [r4, #120]
	subs	r3, r3, #1
	str	r3, [r4, #116]
	ldr	r3, [r4, #112]
	subs	r3, r3, #1
	str	r3, [r4, #112]
	ldr	r3, .L259+24
	ldrb	r3, [r3]
	cmp	r3, r1
	bcs	.L172
	subs	r1, r1, r3
	ldr	r3, [r4, #24]
	ldr	r0, [r4, #36]
	ldr	r3, [r3, #24]
	blx	r3
	cmp	r0, #0
	beq	.L172
	movs	r2, #8
	ldr	r3, .L259+16
	str	r2, [r3]
	movs	r3, #100
	strh	r3, [r4, #30]
.L172:
	.syntax divided
@ 347 "sam_ba_monitor.c" 1
	nop
@ 0 "" 2
	.thumb
	.syntax unified
.L174:
	movs	r3, r4
	movs	r2, #122
	adds	r3, r3, #40
	strb	r2, [r3]
	movs	r3, #0
	str	r3, [r4, #120]
	ldrb	r3, [r4, #20]
	cmp	r3, #0
	beq	.L164
	ldr	r3, [r4, #24]
	movs	r2, #1
	ldr	r1, .L259+28
	ldr	r0, [r3, #12]
	bl	sam_ba_putdata.isra.0
.L164:
	ldr	r3, [r4, #116]
	adds	r3, r3, #1
	str	r3, [r4, #116]
	ldr	r3, [r4, #112]
	adds	r3, r3, #1
	str	r3, [r4, #112]
	b	.L162
.L169:
	str	r2, [r4, #124]
	b	.L170
.L167:
	cmp	r3, #82
	bne	.L175
	adds	r3, r4, #1
	ldrb	r3, [r3, #31]
	cmp	r3, #0
	beq	.L176
	ldr	r3, .L259+32
	ldr	r2, [r4, #36]
	cmp	r2, r3
	bhi	.L176
	ldr	r3, .L259+36
	str	r3, [r4, #36]
.L176:
	ldr	r3, [r4, #24]
	ldr	r2, [r4, #120]
	ldr	r1, [r4, #36]
	ldr	r0, [r3, #20]
	bl	sam_ba_putdata_xmd.isra.2
	b	.L174
.L175:
	cmp	r3, #79
	bne	.L177
	ldr	r3, [r4, #120]
	ldr	r2, [r4, #36]
	strb	r3, [r2]
	b	.L174
.L177:
	cmp	r3, #72
	bne	.L178
	ldr	r3, [r4, #120]
	ldr	r2, [r4, #36]
	strh	r3, [r2]
	b	.L174
.L178:
	cmp	r3, #87
	bne	.L179
	ldr	r3, [r4, #36]
	ldr	r2, [r4, #120]
	str	r2, [r3]
	b	.L174
.L179:
	adds	r2, r4, #1
	ldrb	r2, [r2, #31]
	cmp	r3, #111
	bne	.L180
	cmp	r2, #0
	beq	.L181
	ldr	r3, [r4, #36]
	ldr	r2, .L259+40
	subs	r3, r3, #4
	cmp	r3, r2
	bhi	.L181
	movs	r3, r4
	adds	r3, r3, #120
	str	r3, [r4, #36]
.L181:
	movs	r1, #1
	ldr	r0, [r4, #36]
.L255:
	bl	sam_ba_putdata_term
	b	.L174
.L180:
	cmp	r3, #104
	bne	.L182
	ldr	r1, [r4, #36]
	cmp	r2, #0
	beq	.L183
	ldr	r2, .L259+40
	subs	r3, r1, #4
	cmp	r3, r2
	bhi	.L183
	movs	r3, #0
.L253:
	movs	r0, r4
	str	r3, [r4, #120]
	adds	r0, r0, #120
	movs	r1, #2
	b	.L255
.L183:
	ldrh	r3, [r1]
	b	.L253
.L182:
	cmp	r3, #119
	bne	.L185
	ldr	r1, [r4, #36]
	cmp	r2, #0
	beq	.L186
	ldr	r2, .L259+40
	subs	r3, r1, #4
	cmp	r3, r2
	bhi	.L186
	movs	r3, #0
.L254:
	movs	r0, r4
	str	r3, [r4, #120]
	adds	r0, r0, #120
	movs	r1, #4
	b	.L255
.L186:
	ldr	r3, [r1]
	b	.L254
.L185:
	cmp	r2, #0
	bne	.L188
	cmp	r3, #71
	bne	.L188
	ldr	r1, [r4, #120]
	.syntax divided
@ 71 "C:\Users\Stephan\AppData\Local/Arduino15/packages/arduino/tools/CMSIS/4.5.0/CMSIS/Include/cmsis_gcc.h" 1
	cpsid i
@ 0 "" 2
@ 177 "C:\Users\Stephan\AppData\Local/Arduino15/packages/arduino/tools/CMSIS/4.5.0/CMSIS/Include/cmsis_gcc.h" 1
	MRS r2, msp

@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r3, .L259+24
	str	r2, [r3, #4]
	ldr	r2, [r1]
	.syntax divided
@ 190 "C:\Users\Stephan\AppData\Local/Arduino15/packages/arduino/tools/CMSIS/4.5.0/CMSIS/Include/cmsis_gcc.h" 1
	MSR msp, r2

@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r2, [r1, #4]
	.syntax divided
@ 253 "sam_ba_monitor.c" 1
	bx r2
@ 0 "" 2
	.thumb
	.syntax unified
	ldr	r2, [r3, #4]
	.syntax divided
@ 190 "C:\Users\Stephan\AppData\Local/Arduino15/packages/arduino/tools/CMSIS/4.5.0/CMSIS/Include/cmsis_gcc.h" 1
	MSR msp, r2

@ 0 "" 2
@ 60 "C:\Users\Stephan\AppData\Local/Arduino15/packages/arduino/tools/CMSIS/4.5.0/CMSIS/Include/cmsis_gcc.h" 1
	cpsie i
@ 0 "" 2
	.thumb
	.syntax unified
	ldrb	r3, [r3, #8]
	cmp	r3, #0
	bne	.LCB1283
	b	.L174	@long jump
.LCB1283:
	ldr	r3, [r4, #24]
	movs	r0, #6
	ldr	r3, [r3]
	blx	r3
	b	.L174
.L188:
	cmp	r3, #84
	bne	.L190
	movs	r2, #2
	subs	r3, r3, #83
	strb	r3, [r4, #20]
	ldr	r1, .L259+20
	ldr	r3, [r4, #24]
.L256:
	ldr	r0, [r3, #12]
	bl	sam_ba_putdata.isra.0
	b	.L174
.L190:
	cmp	r3, #78
	bne	.L191
	ldrb	r3, [r4, #20]
	cmp	r3, #0
	bne	.L192
	ldr	r3, [r4, #24]
	movs	r2, #2
	ldr	r1, .L259+20
	ldr	r0, [r3, #12]
	bl	sam_ba_putdata.isra.0
.L192:
	movs	r3, #0
	strb	r3, [r4, #20]
	b	.L174
.L191:
	cmp	r3, #86
	bne	.L193
	ldr	r3, [r4, #24]
	movs	r2, #1
	ldr	r1, .L259+44
	ldr	r0, [r3, #12]
	bl	sam_ba_putdata.isra.0
	ldr	r3, [r4, #24]
	ldr	r1, .L259
	movs	r2, #3
	adds	r1, r1, #32
	ldr	r0, [r3, #12]
	bl	sam_ba_putdata.isra.0
	ldr	r3, [r4, #24]
	movs	r2, #1
	ldr	r1, .L259+48
	ldr	r0, [r3, #12]
	bl	sam_ba_putdata.isra.0
	ldr	r3, [r4, #24]
	ldr	r1, .L259
	movs	r2, #14
	adds	r1, r1, #36
	ldr	r0, [r3, #12]
	bl	sam_ba_putdata.isra.0
	ldr	r1, .L259+52
	ldr	r3, [r4, #24]
	str	r1, [r4, #112]
	movs	r2, #23
	b	.L256
.L193:
	cmp	r3, #88
	bne	.L194
	cmp	r2, #0
	bne	.L220
	ldr	r0, [r4, #120]
.L195:
	bl	eraseFlash
	ldr	r3, [r4, #24]
	movs	r2, #3
	ldr	r1, .L259+56
	b	.L256
.L220:
	movs	r0, #128
	lsls	r0, r0, #6
	b	.L195
.L194:
	cmp	r3, #89
	bne	.L196
	ldr	r3, [r4, #120]
	ldr	r5, [r4, #36]
	ldr	r6, .L259+24
	cmp	r3, #0
	bne	.L197
	str	r5, [r6, #12]
.L198:
	ldr	r3, [r4, #24]
	movs	r2, #3
	ldr	r1, .L259+60
	b	.L256
.L260:
	.align	2
.L259:
	.word	.LANCHOR2
	.word	1090535424
	.word	.LANCHOR0
	.word	.LANCHOR0+41
	.word	1090536596
	.word	.LC18
	.word	.LANCHOR3
	.word	.LC36
	.word	536870911
	.word	536891392
	.word	536870907
	.word	.LC21
	.word	.LC23
	.word	.LC25
	.word	.LC27
	.word	.LC29
.L197:
	cmp	r2, #0
	beq	.L199
	movs	r0, #128
	ldr	r3, [r4, #8]
	lsls	r0, r0, #6
	cmp	r3, r0
	beq	.L199
	bl	eraseFlash
.L199:
	movs	r1, #128
	ldr	r2, [r7, #12]
	ldr	r3, [r4, #120]
	ldr	r2, [r2, #4]
	ldr	r6, [r6, #12]
	bics	r2, r1
	ldr	r1, [r7, #12]
	lsrs	r3, r3, #2
	str	r2, [r1, #4]
	ldr	r2, [r4, #16]
	lsrs	r2, r2, #2
	str	r2, [r7, #4]
	ldr	r2, .L261
	mov	ip, r2
.L200:
	cmp	r3, #0
	beq	.L198
	ldr	r2, [r7, #12]
	ldr	r1, .L261+4
	strh	r1, [r2]
.L201:
	ldr	r2, [r7, #12]
	ldrb	r2, [r2, #20]
	lsls	r2, r2, #31
	bpl	.L201
	movs	r0, r6
	movs	r2, #0
	str	r5, [r7, #8]
.L202:
	ldr	r1, [r7, #4]
	ldr	r5, [r7, #8]
	movs	r6, r0
	cmp	r2, r1
	beq	.L203
	cmp	r2, r3
	bne	.L204
.L203:
	mov	r0, ip
	ldr	r1, [r7, #12]
	strh	r0, [r1]
.L205:
	ldr	r1, [r7, #12]
	ldrb	r1, [r1, #20]
	lsls	r1, r1, #31
	bpl	.L205
	subs	r3, r3, r2
	b	.L200
.L204:
	ldr	r1, [r7, #8]
	ldmia	r0!, {r5}
	adds	r2, r2, #1
	stmia	r1!, {r5}
	str	r1, [r7, #8]
	b	.L202
.L196:
	cmp	r3, #90
	beq	.LCB1502
	b	.L174	@long jump
.LCB1502:
	movs	r5, #0
	ldr	r3, [r4, #36]
	ldr	r2, [r4, #120]
	ldr	r0, .L261+8
	adds	r1, r3, r2
.L208:
	cmp	r3, r1
	bne	.L209
	ldr	r3, [r4, #24]
	movs	r2, #1
	ldr	r0, [r3, #12]
	ldr	r1, .L261+12
	bl	sam_ba_putdata.isra.0
	movs	r3, #7
.L212:
	movs	r2, #15
	ands	r2, r5
	uxtb	r0, r2
	movs	r1, r0
	lsrs	r5, r5, #4
	adds	r1, r1, #48
	cmp	r2, #9
	ble	.L211
	adds	r1, r1, #7
.L211:
	movs	r2, #16
	adds	r2, r7, r2
	strb	r1, [r2, r3]
	subs	r3, r3, #1
	bcs	.L212
	movs	r1, #16
	ldr	r3, [r4, #24]
	movs	r2, #8
	adds	r1, r7, r1
	ldr	r0, [r3, #12]
	bl	sam_ba_putdata.isra.0
	ldr	r3, [r4, #24]
	movs	r2, #3
	ldr	r1, .L261+16
	b	.L256
.L209:
	ldrb	r6, [r3]
	lsrs	r2, r5, #8
	eors	r2, r6
	lsls	r2, r2, #1
	ldrh	r2, [r2, r0]
	lsls	r5, r5, #8
	eors	r5, r2
	uxth	r5, r5
	adds	r3, r3, #1
	b	.L208
.L165:
	movs	r2, r3
	subs	r2, r2, #48
	uxtb	r1, r2
	cmp	r1, #9
	bhi	.L214
	ldr	r3, [r4, #120]
	lsls	r3, r3, #4
.L257:
	orrs	r3, r2
	str	r3, [r4, #120]
	b	.L164
.L214:
	movs	r2, r3
	subs	r2, r2, #65
	cmp	r2, #5
	bhi	.L215
	ldr	r2, [r4, #120]
	subs	r3, r3, #55
	lsls	r2, r2, #4
	b	.L257
.L215:
	movs	r2, r3
	subs	r2, r2, #97
	cmp	r2, #5
	bhi	.L216
	ldr	r2, [r4, #120]
	subs	r3, r3, #87
	lsls	r2, r2, #4
	b	.L257
.L216:
	movs	r2, #0
	cmp	r3, #44
	bne	.L217
	ldr	r3, [r4, #120]
	str	r3, [r4, #36]
.L258:
	str	r2, [r4, #120]
	b	.L164
.L217:
	movs	r1, r4
	adds	r1, r1, #40
	strb	r3, [r1]
	b	.L258
.L262:
	.align	2
.L261:
	.word	-23292
	.word	-23228
	.word	crc16Table
	.word	.LC32
	.word	.LC34
	.size	sam_ba_monitor_run, .-sam_ba_monitor_run
	.align	1
	.syntax unified
	.code	16
	.thumb_func
	.fpu softvfp
	.type	USB_Read, %function
USB_Read:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, r6, r7, lr}
	ldr	r7, .L268
	movs	r6, r0
	ldrb	r3, [r7, #16]
	movs	r0, r1
	movs	r4, r2
	cmp	r3, #0
	bne	.L264
	ldr	r3, .L268+4
	ldr	r2, .L268+8
	ldr	r1, .L268+12
	str	r2, [r3, #64]
	ldr	r2, [r3, #68]
	lsrs	r2, r2, #14
	lsls	r2, r2, #14
	str	r2, [r3, #68]
	ldr	r2, [r3, #68]
	ands	r2, r1
	str	r2, [r3, #68]
	movs	r2, #162
	movs	r3, #64
	lsls	r2, r2, #1
	ldrb	r1, [r6, r2]
	orrs	r3, r1
	strb	r3, [r6, r2]
	movs	r3, #1
	strb	r3, [r7, #16]
.L264:
	movs	r5, #72
	adds	r5, r5, #255
	ldrb	r3, [r6, r5]
	lsls	r3, r3, #31
	lsrs	r3, r3, #31
	beq	.L267
	ldr	r2, .L268+4
	ldr	r3, [r2, #68]
	lsls	r3, r3, #18
	lsrs	r3, r3, #18
	cmp	r4, r3
	bls	.L266
	ldr	r4, [r2, #68]
	lsls	r4, r4, #18
	lsrs	r4, r4, #18
.L266:
	movs	r2, r4
	ldr	r1, .L268+8
	bl	memcpy
	movs	r3, #1
	strb	r3, [r6, r5]
	movs	r3, #0
	strb	r3, [r7, #16]
.L265:
	movs	r0, r4
	@ sp needed
	pop	{r3, r4, r5, r6, r7, pc}
.L267:
	movs	r4, r3
	b	.L265
.L269:
	.align	2
.L268:
	.word	.LANCHOR3
	.word	usb_endpoint_table
	.word	udd_ep_out_cache_buffer+64
	.word	-268419073
	.size	USB_Read, .-USB_Read
	.align	1
	.syntax unified
	.code	16
	.thumb_func
	.fpu softvfp
	.type	cdc_getc, %function
cdc_getc:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r0, r1, r4, lr}
	mov	r3, sp
	adds	r4, r3, #7
	ldr	r3, .L271
	movs	r2, #1
	movs	r1, r4
	ldr	r0, [r3, #20]
	bl	USB_Read
	@ sp needed
	ldrb	r0, [r4]
	pop	{r1, r2, r4, pc}
.L272:
	.align	2
.L271:
	.word	.LANCHOR3
	.size	cdc_getc, .-cdc_getc
	.align	1
	.syntax unified
	.code	16
	.thumb_func
	.fpu softvfp
	.type	USB_Write, %function
USB_Write:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r0, r1, r2, r4, r5, r6, r7, lr}
	str	r0, [sp]
	movs	r0, #1
	ldr	r5, .L279
	movs	r4, r3
	lsls	r3, r3, #5
	movs	r6, r2
	str	r3, [sp, #4]
	adds	r2, r5, r3
	ldr	r3, [r2, #20]
	adds	r2, r2, #16
	lsls	r3, r3, #1
	lsrs	r3, r3, #29
	adds	r3, r3, #3
	lsls	r0, r0, r3
	cmp	r0, r6
	bhi	.L274
	movs	r3, #128
	ldr	r0, [r2, #4]
	lsls	r3, r3, #24
	orrs	r3, r0
	str	r3, [r2, #4]
.L275:
	lsls	r3, r4, #5
	adds	r5, r5, r3
	str	r1, [r5, #16]
	ldr	r3, [r5, #20]
	lsls	r1, r6, #18
	lsrs	r3, r3, #14
	lsrs	r1, r1, #18
	lsls	r3, r3, #14
	orrs	r3, r1
	str	r3, [r5, #20]
	ldr	r3, [r5, #20]
	ldr	r1, .L279+4
	adds	r4, r4, #8
	ands	r3, r1
	str	r3, [r5, #20]
	ldr	r3, [sp]
	lsls	r4, r4, #5
	adds	r4, r3, r4
	movs	r3, #2
	strb	r3, [r4, #7]
	ldrb	r2, [r4, #5]
	subs	r3, r3, #130
	orrs	r3, r2
	strb	r3, [r4, #5]
.L276:
	ldrb	r3, [r4, #7]
	lsls	r3, r3, #30
	bpl	.L276
	movs	r0, r6
	@ sp needed
	pop	{r1, r2, r3, r4, r5, r6, r7, pc}
.L274:
	ldr	r3, .L279+8
	lsls	r7, r4, #6
	adds	r7, r7, r3
	movs	r2, r6
	movs	r0, r7
	bl	memcpy
	movs	r1, r7
	b	.L275
.L280:
	.align	2
.L279:
	.word	usb_endpoint_table
	.word	-268419073
	.word	udd_ep_in_cache_buffer
	.size	USB_Write, .-USB_Write
	.align	1
	.syntax unified
	.code	16
	.thumb_func
	.fpu softvfp
	.type	cdc_write_buf, %function
cdc_write_buf:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, lr}
	movs	r4, r1
	movs	r2, r1
	movs	r1, r0
	ldr	r0, .L282
	movs	r3, #1
	ldr	r0, [r0, #20]
	bl	USB_Write
	@ sp needed
	movs	r0, r4
	pop	{r4, pc}
.L283:
	.align	2
.L282:
	.word	.LANCHOR3
	.size	cdc_write_buf, .-cdc_write_buf
	.align	1
	.syntax unified
	.code	16
	.thumb_func
	.fpu softvfp
	.type	cdc_putc, %function
cdc_putc:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	movs	r3, #1
	push	{r0, r1, r2, lr}
	str	r0, [sp, #4]
	ldr	r0, .L285
	movs	r2, r3
	add	r1, sp, #4
	ldr	r0, [r0, #20]
	bl	USB_Write
	@ sp needed
	movs	r0, #1
	pop	{r1, r2, r3, pc}
.L286:
	.align	2
.L285:
	.word	.LANCHOR3
	.size	cdc_putc, .-cdc_putc
	.align	1
	.syntax unified
	.code	16
	.thumb_func
	.fpu softvfp
	.type	USB_SendString, %function
USB_SendString:
	@ args = 0, pretend = 0, frame = 256
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, lr}
	movs	r5, r0
	sub	sp, sp, #256
	movs	r0, r1
	movs	r6, r2
	movs	r4, r1
	bl	strlen
	mov	r3, sp
	lsls	r0, r0, #1
	adds	r0, r0, #2
	strb	r0, [r3]
	mov	r2, sp
	movs	r3, #3
	lsrs	r6, r6, #1
	strb	r3, [r2, #1]
	subs	r3, r3, #2
.L288:
	subs	r2, r3, #1
	ldrb	r1, [r4, r2]
	lsls	r2, r3, #1
	cmp	r1, #0
	beq	.L291
	cmp	r6, r3
	bgt	.L290
.L291:
	movs	r3, #0
	mov	r1, sp
	movs	r0, r5
	bl	USB_Write
	add	sp, sp, #256
	@ sp needed
	pop	{r4, r5, r6, pc}
.L290:
	mov	r0, sp
	adds	r3, r3, #1
	strh	r1, [r0, r2]
	b	.L288
	.size	USB_SendString, .-USB_SendString
	.align	1
	.syntax unified
	.code	16
	.thumb_func
	.fpu softvfp
	.type	USB_IsConfigured, %function
USB_IsConfigured:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r0, r1, r2, r4, r5, r6, r7, lr}
	ldr	r5, [r0]
	movs	r2, #8
	movs	r1, r5
	ldrh	r3, [r5, #28]
	movs	r6, r0
	ands	r3, r2
	adds	r1, r1, #255
	cmp	r3, #0
	beq	.L294
	movs	r3, #128
	movs	r0, #64
	movs	r4, #192
	strh	r2, [r5, #28]
	adds	r2, r2, #9
	strb	r3, [r5, #10]
	strb	r2, [r1, #1]
	strb	r0, [r1, #6]
	strb	r3, [r1, #5]
	ldr	r3, .L369
	ldr	r5, .L369+4
	ldr	r2, [r3, #4]
	lsls	r4, r4, #22
	ands	r2, r5
	orrs	r2, r4
	str	r2, [r3, #4]
	ldr	r2, [r3, #20]
	ands	r2, r5
	orrs	r4, r2
	ldr	r2, .L369+8
	str	r4, [r3, #20]
	str	r2, [r3]
	ldr	r2, .L369+12
	ldr	r4, .L369+16
	str	r2, [r3, #16]
	ldr	r2, [r3, #4]
	ands	r4, r2
	movs	r2, #128
	lsls	r2, r2, #10
	orrs	r2, r4
	str	r2, [r3, #4]
	ldr	r2, [r3, #4]
	lsrs	r2, r2, #14
	lsls	r2, r2, #14
	str	r2, [r3, #4]
	movs	r3, #0
	strb	r0, [r1, #5]
	strb	r3, [r6, #4]
.L295:
	ldrb	r0, [r6, #4]
	@ sp needed
	pop	{r1, r2, r3, r4, r5, r6, r7, pc}
.L294:
	movs	r0, #16
	ldrb	r2, [r1, #8]
	tst	r2, r0
	beq	.L295
	ldr	r2, .L369+8
	strb	r0, [r1, #8]
	ldrb	r7, [r2]
	ldr	r0, .L369+20
	ldr	r4, .L369+24
	strb	r7, [r0, #31]
	ldrb	r7, [r2, #1]
	ldr	r0, .L369+28
	strb	r7, [r0, #31]
	ldrb	r7, [r2, #2]
	strh	r7, [r4, #34]
	ldrb	r7, [r2, #3]
	ldrh	r0, [r4, #34]
	lsls	r7, r7, #8
	orrs	r7, r0
	strh	r7, [r4, #34]
	ldrb	r7, [r2, #4]
	strh	r7, [r4, #36]
	ldrb	r7, [r2, #5]
	ldrh	r0, [r4, #36]
	lsls	r7, r7, #8
	orrs	r7, r0
	strh	r7, [r4, #36]
	ldrb	r7, [r2, #6]
	ldr	r0, .L369+20
	strh	r7, [r4, #38]
	ldrb	r2, [r2, #7]
	ldrh	r7, [r4, #38]
	lsls	r2, r2, #8
	orrs	r7, r2
	strh	r7, [r4, #38]
	movs	r7, #64
	ldr	r2, .L369+28
	strb	r7, [r1, #5]
	ldrb	r2, [r2, #31]
	ldrb	r0, [r0, #31]
	lsls	r2, r2, #8
	orrs	r2, r0
	ldr	r0, .L369+32
	cmp	r2, r0
	bne	.LCB2089
	b	.L297	@long jump
.LCB2089:
	bgt	.L298
	movs	r0, #128
	lsls	r0, r0, #1
	cmp	r2, r0
	beq	.L296
	bgt	.L300
	cmp	r2, #129
	beq	.L301
	cmp	r2, #130
	bne	.LCB2101
	b	.L302	@long jump
.LCB2101:
	cmp	r2, #128
	bne	.L296
.L301:
	movs	r1, r4
	strh	r3, [r4, #40]
	adds	r1, r1, #40
.L368:
	movs	r2, #2
	b	.L365
.L300:
	movs	r3, #129
	lsls	r3, r3, #1
	cmp	r2, r3
	bne	.LCB2118
	b	.L304	@long jump
.LCB2118:
	bge	.LCB2120
	b	.L367	@long jump
.LCB2120:
	adds	r3, r3, #255
	adds	r3, r3, #255
	cmp	r2, r3
	bne	.LCB2124
	b	.L306	@long jump
.LCB2124:
	ldr	r3, .L369+36
	cmp	r2, r3
	bne	.LCB2127
	b	.L367	@long jump
.LCB2127:
.L296:
	movs	r3, #32
	adds	r5, r5, #255
	ldrb	r2, [r5, #6]
	orrs	r3, r2
	strb	r3, [r5, #6]
	b	.L295
.L298:
	movs	r1, #144
	lsls	r1, r1, #4
	cmp	r2, r1
	beq	.L308
	bgt	.L309
	movs	r1, #208
	lsls	r1, r1, #3
	cmp	r2, r1
	beq	.L310
	movs	r1, #136
	lsls	r1, r1, #4
	cmp	r2, r1
	bne	.LCB2151
	b	.L311	@long jump
.LCB2151:
	movs	r3, #160
	lsls	r3, r3, #3
	cmp	r2, r3
	bne	.L296
	movs	r0, r5
	bl	USB_SendZlp
	movs	r3, #128
	ldrh	r2, [r4, #34]
	rsbs	r3, r3, #0
	orrs	r3, r2
	ldr	r2, [r6]
	uxtb	r3, r3
	strb	r3, [r2, #10]
	b	.L295
.L309:
	ldr	r3, .L369+40
	cmp	r2, r3
	bne	.LCB2173
	b	.L367	@long jump
.LCB2173:
	ble	.L296
	ldr	r3, .L369+44
	cmp	r2, r3
	bne	.LCB2178
	b	.L315	@long jump
.LCB2178:
	ldr	r3, .L369+48
	cmp	r2, r3
	bne	.L296
	ldrh	r3, [r4, #34]
	strb	r3, [r6, #5]
	b	.L367
.L310:
	ldrh	r3, [r4, #34]
	lsrs	r3, r3, #8
	cmp	r3, #1
	bne	.L317
	movs	r2, #18
	ldrh	r3, [r4, #38]
	cmp	r3, r2
	bhi	.L318
	ldrh	r2, [r4, #38]
	uxth	r2, r2
.L318:
	movs	r3, #0
	ldr	r1, .L369+52
	adds	r1, r1, #52
.L365:
	movs	r0, r5
	bl	USB_Write
	b	.L295
.L317:
	ldrh	r3, [r4, #34]
	lsrs	r3, r3, #8
	cmp	r3, #2
	bne	.L319
	movs	r2, #67
	ldrh	r3, [r4, #38]
	cmp	r3, r2
	bhi	.L320
	ldrh	r2, [r4, #38]
	uxth	r2, r2
.L320:
	ldr	r1, .L369+56
	movs	r3, #0
	adds	r1, r1, #4
	b	.L365
.L319:
	ldrh	r3, [r4, #34]
	lsrs	r3, r3, #8
	cmp	r3, #3
	bne	.L296
	ldrh	r3, [r4, #34]
	uxtb	r3, r3
	cmp	r3, #1
	beq	.L323
	cmp	r3, #0
	beq	.L324
	cmp	r3, #2
	bne	.L296
	ldrh	r2, [r4, #38]
	ldr	r1, .L369+60
	uxtb	r2, r2
	b	.L366
.L324:
	ldr	r1, .L369+52
	movs	r2, #4
	adds	r1, r1, #70
	add	r0, sp, #4
	bl	memcpy
	movs	r2, #4
	ldrh	r3, [r4, #38]
	cmp	r3, r2
	bhi	.L326
	ldrh	r2, [r4, #38]
	uxth	r2, r2
.L326:
	movs	r3, #0
	add	r1, sp, #4
	b	.L365
.L323:
	ldrh	r2, [r4, #38]
	ldr	r1, .L369+64
	uxtb	r2, r2
.L366:
	movs	r0, r5
	bl	USB_SendString
	b	.L295
.L308:
	ldrh	r3, [r4, #34]
	movs	r0, r5
	strb	r3, [r6, #4]
	bl	USB_SendZlp
	movs	r3, #160
	movs	r2, #3
	movs	r1, #192
	lsls	r3, r3, #1
	strb	r2, [r5, r3]
	ldr	r3, .L369
	ldr	r0, .L369+4
	ldr	r2, [r3, #68]
	lsls	r1, r1, #22
	ands	r2, r0
	orrs	r2, r1
	str	r2, [r3, #68]
	movs	r2, #70
	adds	r2, r2, #255
	strb	r7, [r5, r2]
	ldr	r2, .L369+68
	movs	r4, #48
	str	r2, [r3, #64]
	movs	r2, #144
	lsls	r2, r2, #1
	strb	r4, [r5, r2]
	ldr	r2, [r3, #52]
	ands	r2, r0
	orrs	r1, r2
	movs	r2, #146
	str	r1, [r3, #52]
	movs	r1, #128
	lsls	r2, r2, #1
	strb	r1, [r5, r2]
	ldr	r2, .L369+72
	str	r2, [r3, #48]
	movs	r2, #176
	lsls	r2, r2, #1
	strb	r7, [r5, r2]
	ldr	r2, [r3, #116]
	ands	r0, r2
	str	r0, [r3, #116]
	movs	r3, #178
	lsls	r3, r3, #1
	strb	r1, [r5, r3]
	b	.L295
.L311:
	adds	r1, r6, #4
	movs	r2, #1
	b	.L365
.L302:
	movs	r2, #80
	strh	r3, [r4, #40]
	ldrh	r3, [r4, #36]
	movs	r0, #15
	ands	r3, r2
	movs	r2, r4
	adds	r2, r2, #42
	strb	r3, [r2]
	ldrh	r3, [r4, #36]
	ands	r3, r0
	strh	r3, [r4, #36]
	ldrh	r3, [r4, #36]
	cmp	r3, #3
	bhi	.L306
	ldrb	r3, [r2]
	cmp	r3, #0
	beq	.L328
	ldrh	r3, [r4, #36]
	adds	r3, r3, #8
	lsls	r3, r3, #5
	adds	r3, r5, r3
	ldrb	r3, [r3, #6]
	lsls	r3, r3, #26
.L362:
	movs	r1, r4
	lsrs	r3, r3, #31
	strh	r3, [r4, #40]
	adds	r1, r1, #40
	movs	r3, #0
	b	.L368
.L328:
	ldrh	r3, [r4, #36]
	adds	r3, r3, #8
	lsls	r3, r3, #5
	adds	r3, r5, r3
	ldrb	r3, [r3, #6]
	lsls	r3, r3, #27
	b	.L362
.L306:
	movs	r3, #32
	ldrb	r2, [r1, #6]
	orrs	r3, r2
	strb	r3, [r1, #6]
	b	.L295
.L297:
	movs	r2, #128
	ldrh	r3, [r4, #36]
	movs	r1, #15
	ands	r3, r2
	movs	r2, r4
	adds	r2, r2, #42
	strb	r3, [r2]
	ldrh	r3, [r4, #36]
	ands	r3, r1
	strh	r3, [r4, #36]
	ldrh	r3, [r4, #34]
	cmp	r3, #0
	beq	.LCB2434
	b	.L296	@long jump
.LCB2434:
	ldrh	r3, [r4, #36]
	cmp	r3, #0
	bne	.LCB2438
	b	.L296	@long jump
.LCB2438:
	ldrh	r3, [r4, #36]
	cmp	r3, #3
	bls	.LCB2442
	b	.L296	@long jump
.LCB2442:
	ldrb	r3, [r2]
	cmp	r3, #0
	beq	.L331
	movs	r2, #32
	ldrh	r3, [r4, #36]
	adds	r3, r3, #8
	lsls	r3, r3, #5
	adds	r3, r5, r3
.L363:
	strb	r2, [r3, #5]
.L367:
	movs	r0, r5
	bl	USB_SendZlp
	b	.L295
.L331:
	ldrh	r3, [r4, #36]
	movs	r2, #16
	adds	r3, r3, #8
	lsls	r3, r3, #5
	adds	r3, r5, r3
	b	.L363
.L370:
	.align	2
.L369:
	.word	usb_endpoint_table
	.word	-1879048193
	.word	udd_ep_out_cache_buffer
	.word	udd_ep_in_cache_buffer
	.word	-268419073
	.word	.LANCHOR3+1
	.word	.LANCHOR3
	.word	.LANCHOR3+2
	.word	770
	.word	769
	.word	8225
	.word	8609
	.word	8737
	.word	.LANCHOR2
	.word	.LANCHOR1
	.word	.LC63
	.word	.LC61
	.word	udd_ep_out_cache_buffer+64
	.word	udd_ep_in_cache_buffer+64
.L304:
	movs	r2, #128
	ldrh	r3, [r4, #36]
	ands	r3, r2
	movs	r2, r4
	adds	r2, r2, #42
	strb	r3, [r2]
	movs	r2, #15
	ldrh	r3, [r4, #36]
	ands	r3, r2
	strh	r3, [r4, #36]
	ldrh	r3, [r4, #34]
	cmp	r3, #0
	beq	.LCB2515
	b	.L296	@long jump
.LCB2515:
	ldrh	r3, [r4, #36]
	cmp	r3, #0
	bne	.LCB2519
	b	.L296	@long jump
.LCB2519:
	ldrh	r3, [r4, #36]
	cmp	r3, #3
	bls	.LCB2523
	b	.L296	@long jump
.LCB2523:
	movs	r3, r4
	adds	r3, r3, #42
	ldrb	r3, [r3]
	cmp	r3, #0
	beq	.L334
	ldrh	r3, [r4, #36]
	adds	r3, r3, #8
	lsls	r3, r3, #5
	adds	r3, r5, r3
	ldrb	r3, [r3, #6]
	lsls	r3, r3, #26
	bpl	.L367
	ldrh	r3, [r4, #36]
	adds	r2, r2, #17
	adds	r3, r3, #8
	lsls	r3, r3, #5
	adds	r3, r5, r3
	strb	r2, [r3, #4]
	ldrh	r3, [r4, #36]
	adds	r3, r3, #8
	lsls	r3, r3, #5
	adds	r3, r5, r3
	ldrb	r3, [r3, #7]
	lsls	r3, r3, #25
	bpl	.L367
	ldrh	r3, [r4, #36]
	adds	r2, r2, #32
	adds	r3, r3, #8
	lsls	r3, r3, #5
	adds	r3, r5, r3
	strb	r2, [r3, #7]
	ldrh	r3, [r4, #36]
	subs	r2, r2, #62
	adds	r3, r3, #8
	lsls	r3, r3, #5
	adds	r3, r5, r3
.L364:
	strb	r2, [r3, #4]
	b	.L367
.L334:
	ldrh	r3, [r4, #36]
	adds	r3, r3, #8
	lsls	r3, r3, #5
	adds	r3, r5, r3
	ldrb	r3, [r3, #6]
	lsls	r3, r3, #27
	bmi	.LCB2589
	b	.L367	@long jump
.LCB2589:
	movs	r2, #16
	ldrh	r3, [r4, #36]
	adds	r3, r3, #8
	lsls	r3, r3, #5
	adds	r3, r5, r3
	strb	r2, [r3, #4]
	ldrh	r3, [r4, #36]
	adds	r3, r3, #8
	lsls	r3, r3, #5
	adds	r3, r5, r3
	ldrb	r3, [r3, #7]
	lsls	r3, r3, #26
	bmi	.LCB2607
	b	.L367	@long jump
.LCB2607:
	ldrh	r3, [r4, #36]
	adds	r2, r2, r2
	adds	r3, r3, #8
	lsls	r3, r3, #5
	adds	r3, r5, r3
	strb	r2, [r3, #7]
	ldrh	r3, [r4, #36]
	subs	r2, r2, #31
	adds	r3, r3, #8
	lsls	r3, r3, #5
	adds	r3, r5, r3
	b	.L364
.L315:
	movs	r2, #8
	ldrh	r3, [r4, #38]
	cmp	r3, r2
	bhi	.L338
	ldrh	r2, [r4, #38]
	uxth	r2, r2
.L338:
	ldr	r1, .L371
	movs	r3, #0
	adds	r1, r1, #72
	b	.L365
.L372:
	.align	2
.L371:
	.word	.LANCHOR1
	.size	USB_IsConfigured, .-USB_IsConfigured
	.align	1
	.syntax unified
	.code	16
	.thumb_func
	.fpu softvfp
	.type	cdc_read_buf_xmd, %function
cdc_read_buf_xmd:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r3, r4, r5, r6, r7, lr}
	ldr	r7, .L378
	movs	r6, r0
	movs	r0, r7
	adds	r0, r0, #20
	movs	r5, r1
	bl	USB_IsConfigured
	movs	r4, r5
	cmp	r0, #0
	bne	.L375
	movs	r5, r0
	b	.L374
.L376:
	movs	r2, r4
	movs	r1, r6
	ldr	r0, [r7, #20]
	bl	USB_Read
	subs	r4, r4, r0
	adds	r6, r6, r0
.L375:
	cmp	r4, #0
	bne	.L376
.L374:
	movs	r0, r5
	@ sp needed
	pop	{r3, r4, r5, r6, r7, pc}
.L379:
	.align	2
.L378:
	.word	.LANCHOR3
	.size	cdc_read_buf_xmd, .-cdc_read_buf_xmd
	.align	1
	.syntax unified
	.code	16
	.thumb_func
	.fpu softvfp
	.type	cdc_read_buf, %function
cdc_read_buf:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, lr}
	ldr	r4, .L384
	movs	r5, r0
	movs	r0, r4
	adds	r0, r0, #20
	movs	r6, r1
	bl	USB_IsConfigured
	movs	r3, r0
	movs	r0, #0
	cmp	r3, r0
	beq	.L380
	movs	r2, r6
	movs	r1, r5
	ldr	r0, [r4, #20]
	bl	USB_Read
.L380:
	@ sp needed
	pop	{r4, r5, r6, pc}
.L385:
	.align	2
.L384:
	.word	.LANCHOR3
	.size	cdc_read_buf, .-cdc_read_buf
	.align	1
	.syntax unified
	.code	16
	.thumb_func
	.fpu softvfp
	.type	cdc_is_rx_ready, %function
cdc_is_rx_ready:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, lr}
	ldr	r4, .L390
	movs	r0, r4
	adds	r0, r0, #20
	bl	USB_IsConfigured
	movs	r3, r0
	movs	r0, #0
	cmp	r3, r0
	beq	.L387
	movs	r3, #72
	ldr	r2, [r4, #20]
	adds	r3, r3, #255
	ldrb	r0, [r2, r3]
	lsls	r0, r0, #31
	lsrs	r0, r0, #31
.L387:
	@ sp needed
	pop	{r4, pc}
.L391:
	.align	2
.L390:
	.word	.LANCHOR3
	.size	cdc_is_rx_ready, .-cdc_is_rx_ready
	.section	.text.startup,"ax",%progbits
	.align	1
	.global	main
	.syntax unified
	.code	16
	.thumb_func
	.fpu softvfp
	.type	main, %function
main:
	@ Volatile: function does not return.
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, lr}
	ldr	r2, .L449
	sub	sp, sp, #20
	ldr	r3, [r2]
	add	r7, sp, #0
	adds	r3, r3, #1
	beq	.L394
	ldr	r3, .L449+4
	ldr	r1, .L449+8
	uxtb	r0, r2
	str	r1, [r3, #44]
	cmp	r0, #0
	bne	.L394
	ldr	r3, .L449+12
	ldr	r1, .L449+16
	ldrb	r3, [r3]
	lsls	r3, r3, #31
	lsrs	r3, r3, #31
	beq	.L396
	str	r0, [r1]
.L397:
	ldr	r3, [r2]
	.syntax divided
@ 190 "C:\Users\Stephan\AppData\Local/Arduino15/packages/arduino/tools/CMSIS/4.5.0/CMSIS/Include/cmsis_gcc.h" 1
	MSR msp, r3

@ 0 "" 2
	.thumb
	.syntax unified
	movs	r3, #255
	movs	r1, r2
	bics	r1, r3
	ldr	r3, .L449+20
	str	r1, [r3, #8]
	ldr	r3, [r2, #4]
	.syntax divided
@ 46 "main.c" 1
	bx r3
@ 0 "" 2
	.thumb
	.syntax unified
	b	.L394
.L396:
	ldr	r4, [r1]
	ldr	r0, .L449+24
	cmp	r4, r0
	beq	.LCB2810
	b	.L398	@long jump
.LCB2810:
	str	r3, [r1]
.L394:
	movs	r1, #30
	ldr	r2, .L449+28
	ldr	r5, .L449+32
	ldr	r3, [r2, #4]
	bics	r3, r1
	subs	r1, r1, #28
	orrs	r3, r1
	str	r3, [r2, #4]
	ldr	r3, .L449+36
	strh	r3, [r5, #20]
	ldrh	r3, [r5, #20]
	orrs	r3, r1
	strh	r3, [r5, #20]
	str	r1, [r7, #12]
.L400:
	ldr	r3, [r5, #12]
	ldr	r2, [r7, #12]
	tst	r3, r2
	beq	.L400
	movs	r3, #1
	ldr	r4, .L449+40
	movs	r6, #128
	str	r3, [r4, #8]
	bl	gclk_sync
	ldr	r3, .L449+44
	str	r3, [r4, #4]
	bl	gclk_sync
	movs	r3, #130
	lsls	r3, r3, #7
	strh	r3, [r4, #2]
	bl	gclk_sync
	ldrh	r3, [r5, #36]
	bics	r3, r6
	strh	r3, [r5, #36]
	bl	dfll_sync
	ldr	r3, .L449+48
	str	r3, [r5, #44]
	bl	dfll_sync
	ldrh	r2, [r5, #36]
	ldr	r3, .L449+52
	orrs	r3, r2
	strh	r3, [r5, #36]
	bl	dfll_sync
	ldrh	r3, [r5, #36]
	ldr	r2, [r7, #12]
	orrs	r2, r3
	strh	r2, [r5, #36]
	movs	r2, #64
	str	r4, [r7, #12]
.L401:
	ldr	r1, [r5, #12]
	tst	r1, r6
	beq	.L401
	ldr	r1, [r5, #12]
	tst	r1, r2
	beq	.L401
	movs	r5, #0
	bl	dfll_sync
	ldr	r3, [r7, #12]
	ldr	r6, .L449+56
	str	r5, [r3, #8]
	bl	gclk_sync
	ldr	r3, .L449+60
	ldr	r2, [r7, #12]
	str	r3, [r2, #4]
	bl	gclk_sync
	movs	r1, #192
	ldr	r3, .L449+64
	ldr	r0, .L449+20
	str	r3, [r6, #4]
	ldr	r3, [r0, #32]
	lsls	r1, r1, #24
	lsls	r3, r3, #8
	lsrs	r3, r3, #8
	orrs	r3, r1
	str	r3, [r0, #32]
	movs	r3, #7
	str	r5, [r6, #8]
	str	r3, [r6]
	.syntax divided
@ 60 "C:\Users\Stephan\AppData\Local/Arduino15/packages/arduino/tools/CMSIS/4.5.0/CMSIS/Include/cmsis_gcc.h" 1
	cpsie i
@ 0 "" 2
	.thumb
	.syntax unified
	movs	r2, #1
	ldr	r5, .L449+68
	movs	r3, #2
	ldrb	r1, [r5]
	orrs	r1, r2
	strb	r1, [r5]
	movs	r5, #15
	ldr	r1, .L449+72
	ldrb	r4, [r1]
	bics	r4, r5
	strb	r4, [r1]
	ldrb	r4, [r1]
	orrs	r4, r3
	strb	r4, [r1]
	ldr	r4, .L449+76
	ldrb	r3, [r4]
	orrs	r3, r2
	strb	r3, [r4]
	ldrb	r3, [r1]
	ldr	r2, [r7, #12]
	ands	r3, r5
	strb	r3, [r1]
	movs	r3, #32
	ldrb	r5, [r1]
	orrs	r3, r5
	strb	r3, [r1]
	movs	r3, #4
	ldr	r1, .L449+80
	ldr	r5, [r1, #32]
	orrs	r3, r5
	str	r3, [r1, #32]
	ldr	r3, .L449+84
	strh	r3, [r2, #2]
	ldr	r3, .L449+64
	str	r0, [r7, #8]
	str	r3, [r7, #4]
.L403:
	ldr	r3, [r7, #12]
	ldrb	r3, [r3, #1]
	sxtb	r3, r3
	cmp	r3, #0
	blt	.L403
	ldr	r3, .L449+88
	movs	r2, r3
.L404:
	ldr	r0, [r3, #28]
	lsls	r0, r0, #30
	bmi	.L404
	movs	r5, #2
	ldr	r0, [r3]
	bics	r0, r5
	str	r0, [r3]
.L405:
	ldr	r3, [r2, #28]
	lsls	r3, r3, #31
	bmi	.L405
	movs	r3, #1
	ldr	r0, [r2]
	orrs	r3, r0
	str	r3, [r2]
.L406:
	ldr	r3, [r2]
	lsls	r3, r3, #31
	bmi	.L406
.L407:
	ldr	r3, [r2, #28]
	lsls	r3, r3, #31
	bmi	.L407
	ldr	r3, [r2, #28]
	lsls	r3, r3, #30
	bmi	.L407
	ldr	r3, .L449+92
	str	r3, [r2]
.L409:
	ldr	r3, [r2, #28]
	lsls	r3, r3, #29
	bmi	.L409
	movs	r3, #192
	lsls	r3, r3, #10
	str	r3, [r2, #4]
	ldr	r3, .L449+96
	strh	r3, [r2, #12]
.L410:
	ldr	r3, [r2, #28]
	lsls	r3, r3, #30
	lsrs	r3, r3, #31
	bne	.L410
	movs	r0, #2
	ldr	r5, [r2]
	orrs	r0, r5
	ldr	r5, .L449+4
	str	r0, [r2]
	movs	r2, r5
	adds	r2, r2, #48
	strb	r3, [r2]
	movs	r2, r5
	adds	r2, r2, #49
	strb	r3, [r2]
	movs	r2, r5
	adds	r2, r2, #50
	strb	r3, [r2]
	movs	r2, r5
	adds	r2, r2, #51
	strb	r3, [r2]
	movs	r2, r5
	adds	r2, r2, #52
	strb	r3, [r2]
	ldr	r2, .L449+100
	strb	r3, [r2]
	ldr	r2, [r1, #28]
	adds	r3, r3, #32
	orrs	r3, r2
	str	r3, [r1, #28]
	movs	r3, #1
	ldr	r1, .L449+104
	ldrb	r2, [r1]
	orrs	r2, r3
	strb	r2, [r1]
	movs	r1, #15
	ldr	r2, .L449+108
	ldrb	r0, [r2]
	bics	r0, r1
	strb	r0, [r2]
	ldrb	r0, [r2]
	mov	ip, r0
	movs	r0, #6
	mov	r4, ip
	orrs	r0, r4
	strb	r0, [r2]
	ldr	r0, .L449+112
	ldrb	r4, [r0]
	orrs	r3, r4
	strb	r3, [r0]
	ldrb	r3, [r2]
	ands	r3, r1
	strb	r3, [r2]
	movs	r3, #96
	ldrb	r1, [r2]
	orrs	r3, r1
	strb	r3, [r2]
	ldr	r3, .L449+116
	ldr	r2, [r7, #12]
	strh	r3, [r2, #2]
.L411:
	ldr	r3, [r7, #12]
	ldrb	r3, [r3, #1]
	sxtb	r3, r3
	cmp	r3, #0
	blt	.L411
	movs	r3, #1
	ldr	r4, .L449+120
	ldrb	r2, [r4]
	orrs	r3, r2
	strb	r3, [r4]
.L412:
	ldrb	r3, [r4, #2]
	lsls	r3, r3, #31
	bmi	.L412
	movs	r1, #31
	ldr	r2, .L449+124
	ldr	r3, [r2]
	str	r2, [r7, #12]
	lsrs	r0, r3, #13
	ands	r0, r1
	cmp	r0, r1
	bne	.L413
	movs	r0, #5
.L413:
	ldrh	r3, [r4, #40]
	ands	r0, r1
	mov	ip, r3
	mov	r2, ip
	ldr	r3, .L449+128
	lsls	r0, r0, #6
	ands	r3, r2
	orrs	r3, r0
	strh	r3, [r4, #40]
	ldr	r3, [r7, #12]
	ldr	r3, [r3]
	lsrs	r3, r3, #18
	ands	r3, r1
	cmp	r3, #31
	bne	.L414
	subs	r3, r3, #2
.L414:
	movs	r0, #31
	movs	r2, #7
	ldrh	r1, [r4, #40]
	ands	r3, r0
	bics	r1, r0
	orrs	r3, r1
	strh	r3, [r4, #40]
	ldr	r3, [r7, #12]
	ldr	r3, [r3]
	str	r2, [r7, #12]
	lsrs	r3, r3, #23
	ands	r3, r2
	cmp	r3, r2
	bne	.L415
	movs	r3, #3
.L415:
	ldr	r1, [r7, #12]
	ldrh	r2, [r4, #40]
	ands	r3, r1
	ldr	r1, .L449+132
	lsls	r3, r3, #12
	ands	r2, r1
	orrs	r3, r2
	movs	r2, #127
	strh	r3, [r4, #40]
	ldrb	r3, [r4]
	ldr	r0, .L449+136
	ands	r3, r2
	strb	r3, [r4]
	movs	r3, #4
	ldrb	r2, [r4]
	movs	r1, #0
	orrs	r3, r2
	movs	r2, #12
	strb	r3, [r4]
	str	r0, [r4, #36]
	ldrh	r3, [r4, #8]
	bics	r3, r2
	strh	r3, [r4, #8]
	ldrh	r3, [r4, #8]
	subs	r2, r2, #11
	bics	r3, r2
	strh	r3, [r4, #8]
	adds	r2, r2, #127
	bl	memset
	movs	r2, #0
	ldr	r3, .L449+140
	str	r4, [r5, #20]
	str	r3, [r5, #28]
	movs	r3, #2
	strh	r2, [r5, #24]
	ldrb	r1, [r4]
	ldr	r0, .L449+144
	orrs	r3, r1
	movs	r1, #128
	strb	r3, [r4]
	ldr	r3, .L449+148
	lsls	r1, r1, #10
	str	r1, [r3, #8]
	movs	r1, #8
	str	r1, [r0]
	ldr	r0, .L449+152
	str	r1, [r0]
	movs	r1, #128
	lsls	r1, r1, #20
	str	r1, [r3, #8]
	str	r1, [r3, #24]
	movs	r1, #192
	ldr	r3, [r7, #4]
	lsls	r1, r1, #24
	str	r3, [r6, #4]
	ldr	r3, [r7, #8]
	ldr	r3, [r3, #32]
	lsls	r3, r3, #8
	lsrs	r3, r3, #8
	orrs	r3, r1
	ldr	r1, [r7, #8]
	str	r3, [r1, #32]
	ldr	r3, [r7, #12]
	str	r2, [r6, #8]
	str	r3, [r6]
	movs	r6, #1
.L419:
	movs	r4, r5
	adds	r4, r4, #53
.L418:
	movs	r0, r5
	ldr	r3, [r5, #28]
	adds	r0, r0, #20
	blx	r3
	cmp	r0, #0
	beq	.L416
	strb	r6, [r4]
.L416:
	ldrb	r3, [r4]
	cmp	r3, #0
	beq	.L417
	ldr	r3, .L449+156
	ldr	r2, .L449+100
	adds	r3, r3, #76
	str	r3, [r2, #24]
.L448:
	bl	sam_ba_monitor_run
.L398:
	ldr	r3, .L449+160
	str	r0, [r1]
.L399:
	subs	r3, r3, #1
	cmp	r3, #0
	bne	.L399
	str	r3, [r1]
	b	.L397
.L417:
	ldrb	r3, [r4]
	cmp	r3, #0
	bne	.L418
	bl	serial_is_rx_ready
	cmp	r0, #0
	beq	.L419
	bl	serial_getc
	cmp	r0, #35
	bne	.L419
	ldr	r3, .L449+156
	ldr	r2, .L449+100
	adds	r3, r3, #104
	str	r3, [r2, #24]
	movs	r3, #1
	strb	r3, [r5, #8]
	b	.L448
.L450:
	.align	2
.L449:
	.word	__sketch_vectors_ptr
	.word	.LANCHOR3
	.word	__sketch_vectors_ptr+4
	.word	1073742904
	.word	536903676
	.word	-536810240
	.word	125010229
	.word	1090535424
	.word	1073743872
	.word	1548
	.word	1073744896
	.word	66817
	.word	2113865144
	.word	2564
	.word	-536813552
	.word	198400
	.word	999
	.word	1090536522
	.word	1090536501
	.word	1090536523
	.word	1073742848
	.word	16404
	.word	1107298304
	.word	1076953092
	.word	-2518
	.word	.LANCHOR0
	.word	1090536536
	.word	1090536508
	.word	1090536537
	.word	16390
	.word	1090539520
	.word	8413220
	.word	-1985
	.word	-28673
	.word	usb_endpoint_table
	.word	USB_IsConfigured
	.word	1090536584
	.word	1090536448
	.word	1090536600
	.word	.LANCHOR2
	.word	125000
	.size	main, .-main
	.text
	.align	1
	.global	Reset_Handler
	.syntax unified
	.code	16
	.thumb_func
	.fpu softvfp
	.type	Reset_Handler, %function
Reset_Handler:
	@ Volatile: function does not return.
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	ldr	r2, .L463
	ldr	r0, .L463+4
	push	{r4, r5, r6, lr}
	cmp	r2, r0
	bne	.L452
.L456:
	ldr	r2, .L463+8
	ldr	r3, .L463+12
	movs	r1, #0
	cmp	r3, r2
	bne	.L453
.L454:
	bl	main
.L452:
	ldr	r4, .L463+16
	movs	r3, #0
	cmp	r4, r2
	beq	.L456
.L455:
	adds	r1, r2, r3
	cmp	r0, r1
	bls	.L456
	ldr	r5, [r4, r3]
	adds	r3, r3, #4
	str	r5, [r1]
	b	.L455
.L458:
	stmia	r3!, {r1}
.L453:
	cmp	r3, r2
	bcc	.L458
	b	.L454
.L464:
	.align	2
.L463:
	.word	__data_start__
	.word	__data_end__
	.word	__bss_end__
	.word	__bss_start__
	.word	__etext
	.size	Reset_Handler, .-Reset_Handler
	.global	exception_table
	.section	.rodata
	.align	2
	.set	.LANCHOR2,. + 0
.LC15:
	.word	8
	.word	16
	.word	32
	.word	64
	.word	128
	.word	256
	.word	512
	.word	1024
	.type	RomBOOT_Version, %object
	.size	RomBOOT_Version, 4
RomBOOT_Version:
	.ascii	"2.0\000"
	.type	RomBOOT_ExtendedCapabilities, %object
	.size	RomBOOT_ExtendedCapabilities, 15
RomBOOT_ExtendedCapabilities:
	.ascii	"[Arduino:XYZP]\000"
	.space	1
	.type	devDescriptor, %object
	.size	devDescriptor, 18
devDescriptor:
	.byte	18
	.byte	1
	.byte	0
	.byte	2
	.byte	2
	.byte	0
	.byte	0
	.byte	64
	.byte	65
	.byte	35
	.byte	77
	.byte	0
	.byte	0
	.byte	2
	.byte	1
	.byte	2
	.byte	0
	.byte	1
.LC60:
	.short	772
	.short	1033
	.space	2
	.type	usbcdc_if, %object
	.size	usbcdc_if, 28
usbcdc_if:
	.word	cdc_putc
	.word	cdc_getc
	.word	cdc_is_rx_ready
	.word	cdc_write_buf
	.word	cdc_read_buf
	.word	cdc_write_buf
	.word	cdc_read_buf_xmd
	.type	uart_if, %object
	.size	uart_if, 28
uart_if:
	.word	serial_putc
	.word	serial_getc
	.word	serial_is_rx_ready
	.word	serial_putdata
	.word	serial_getdata
	.word	serial_putdata_xmd
	.word	serial_getdata_xmd
	.type	crc16Table, %object
	.size	crc16Table, 512
crc16Table:
	.short	0
	.short	4129
	.short	8258
	.short	12387
	.short	16516
	.short	20645
	.short	24774
	.short	28903
	.short	-32504
	.short	-28375
	.short	-24246
	.short	-20117
	.short	-15988
	.short	-11859
	.short	-7730
	.short	-3601
	.short	4657
	.short	528
	.short	12915
	.short	8786
	.short	21173
	.short	17044
	.short	29431
	.short	25302
	.short	-27847
	.short	-31976
	.short	-19589
	.short	-23718
	.short	-11331
	.short	-15460
	.short	-3073
	.short	-7202
	.short	9314
	.short	13379
	.short	1056
	.short	5121
	.short	25830
	.short	29895
	.short	17572
	.short	21637
	.short	-23190
	.short	-19125
	.short	-31448
	.short	-27383
	.short	-6674
	.short	-2609
	.short	-14932
	.short	-10867
	.short	13907
	.short	9842
	.short	5649
	.short	1584
	.short	30423
	.short	26358
	.short	22165
	.short	18100
	.short	-18597
	.short	-22662
	.short	-26855
	.short	-30920
	.short	-2081
	.short	-6146
	.short	-10339
	.short	-14404
	.short	18628
	.short	22757
	.short	26758
	.short	30887
	.short	2112
	.short	6241
	.short	10242
	.short	14371
	.short	-13876
	.short	-9747
	.short	-5746
	.short	-1617
	.short	-30392
	.short	-26263
	.short	-22262
	.short	-18133
	.short	23285
	.short	19156
	.short	31415
	.short	27286
	.short	6769
	.short	2640
	.short	14899
	.short	10770
	.short	-9219
	.short	-13348
	.short	-1089
	.short	-5218
	.short	-25735
	.short	-29864
	.short	-17605
	.short	-21734
	.short	27814
	.short	31879
	.short	19684
	.short	23749
	.short	11298
	.short	15363
	.short	3168
	.short	7233
	.short	-4690
	.short	-625
	.short	-12820
	.short	-8755
	.short	-21206
	.short	-17141
	.short	-29336
	.short	-25271
	.short	32407
	.short	28342
	.short	24277
	.short	20212
	.short	15891
	.short	11826
	.short	7761
	.short	3696
	.short	-97
	.short	-4162
	.short	-8227
	.short	-12292
	.short	-16613
	.short	-20678
	.short	-24743
	.short	-28808
	.short	-28280
	.short	-32343
	.short	-20022
	.short	-24085
	.short	-12020
	.short	-16083
	.short	-3762
	.short	-7825
	.short	4224
	.short	161
	.short	12482
	.short	8419
	.short	20484
	.short	16421
	.short	28742
	.short	24679
	.short	-31815
	.short	-27752
	.short	-23557
	.short	-19494
	.short	-15555
	.short	-11492
	.short	-7297
	.short	-3234
	.short	689
	.short	4752
	.short	8947
	.short	13010
	.short	16949
	.short	21012
	.short	25207
	.short	29270
	.short	-18966
	.short	-23093
	.short	-27224
	.short	-31351
	.short	-2706
	.short	-6833
	.short	-10964
	.short	-15091
	.short	13538
	.short	9411
	.short	5280
	.short	1153
	.short	29798
	.short	25671
	.short	21540
	.short	17413
	.short	-22565
	.short	-18438
	.short	-30823
	.short	-26696
	.short	-6305
	.short	-2178
	.short	-14563
	.short	-10436
	.short	9939
	.short	14066
	.short	1681
	.short	5808
	.short	26199
	.short	30326
	.short	17941
	.short	22068
	.short	-9908
	.short	-13971
	.short	-1778
	.short	-5841
	.short	-26168
	.short	-30231
	.short	-18038
	.short	-22101
	.short	22596
	.short	18533
	.short	30726
	.short	26663
	.short	6336
	.short	2273
	.short	14466
	.short	10403
	.short	-13443
	.short	-9380
	.short	-5313
	.short	-1250
	.short	-29703
	.short	-25640
	.short	-21573
	.short	-17510
	.short	19061
	.short	23124
	.short	27191
	.short	31254
	.short	2801
	.short	6864
	.short	10931
	.short	14994
	.short	-722
	.short	-4849
	.short	-8852
	.short	-12979
	.short	-16982
	.short	-21109
	.short	-25112
	.short	-29239
	.short	31782
	.short	27655
	.short	23652
	.short	19525
	.short	15522
	.short	11395
	.short	7392
	.short	3265
	.short	-4321
	.short	-194
	.short	-12451
	.short	-8324
	.short	-20581
	.short	-16454
	.short	-28711
	.short	-24584
	.short	28183
	.short	32310
	.short	20053
	.short	24180
	.short	11923
	.short	16050
	.short	3793
	.short	7920
	.data
	.align	2
	.set	.LANCHOR1,. + 0
	.type	ledDirection, %object
	.size	ledDirection, 1
ledDirection:
	.byte	1
	.type	ledTargetValue, %object
	.size	ledTargetValue, 1
ledTargetValue:
	.byte	20
	.space	2
	.type	cfgDescriptor, %object
	.size	cfgDescriptor, 67
cfgDescriptor:
	.byte	9
	.byte	2
	.byte	67
	.byte	0
	.byte	2
	.byte	1
	.byte	0
	.byte	-128
	.byte	50
	.byte	9
	.byte	4
	.byte	0
	.byte	0
	.byte	1
	.byte	2
	.byte	2
	.byte	0
	.byte	0
	.byte	5
	.byte	36
	.byte	0
	.byte	16
	.byte	1
	.byte	4
	.byte	36
	.byte	2
	.byte	0
	.byte	5
	.byte	36
	.byte	6
	.byte	0
	.byte	1
	.byte	5
	.byte	36
	.byte	1
	.byte	0
	.byte	1
	.byte	7
	.byte	5
	.byte	-125
	.byte	3
	.byte	8
	.byte	0
	.byte	-1
	.byte	9
	.byte	4
	.byte	1
	.byte	0
	.byte	2
	.byte	10
	.byte	0
	.byte	0
	.byte	0
	.byte	7
	.byte	5
	.byte	-127
	.byte	2
	.byte	64
	.byte	0
	.byte	0
	.byte	7
	.byte	5
	.byte	2
	.byte	2
	.byte	64
	.byte	0
	.byte	0
	.space	1
	.type	line_coding, %object
	.size	line_coding, 8
line_coding:
	.word	115200
	.byte	0
	.byte	0
	.byte	8
	.space	1
	.type	g_interrupt_enabled, %object
	.size	g_interrupt_enabled, 1
g_interrupt_enabled:
	.byte	1
	.bss
	.align	2
	.set	.LANCHOR0,. + 0
	.set	.LANCHOR3,. + 128
	.type	error_timeout, %object
	.size	error_timeout, 1
error_timeout:
	.space	1
	.space	1
	.type	size_of_data, %object
	.size	size_of_data, 2
size_of_data:
	.space	2
	.type	mode_of_transfer, %object
	.size	mode_of_transfer, 1
mode_of_transfer:
	.space	1
	.space	1
	.type	txLEDPulse, %object
	.size	txLEDPulse, 2
txLEDPulse:
	.space	2
	.type	erased_from, %object
	.size	erased_from, 4
erased_from:
	.space	4
	.type	MAX_FLASH, %object
	.size	MAX_FLASH, 4
MAX_FLASH:
	.space	4
	.type	PAGE_SIZE, %object
	.size	PAGE_SIZE, 4
PAGE_SIZE:
	.space	4
	.type	b_terminal_mode, %object
	.size	b_terminal_mode, 1
b_terminal_mode:
	.space	1
	.space	3
	.type	ptr_monitor_if, %object
	.size	ptr_monitor_if, 4
ptr_monitor_if:
	.space	4
	.type	ledKeepValue, %object
	.size	ledKeepValue, 1
ledKeepValue:
	.space	1
	.space	1
	.type	rxLEDPulse, %object
	.size	rxLEDPulse, 2
rxLEDPulse:
	.space	2
	.type	b_security_enabled, %object
	.size	b_security_enabled, 1
b_security_enabled:
	.space	1
	.space	3
	.type	ptr_data, %object
	.size	ptr_data, 4
ptr_data:
	.space	4
	.type	command, %object
	.size	command, 1
command:
	.space	1
	.type	data, %object
	.size	data, 64
data:
	.space	64
	.space	3
	.type	length, %object
	.size	length, 4
length:
	.space	4
	.type	ptr, %object
	.size	ptr, 4
ptr:
	.space	4
	.type	i, %object
	.size	i, 4
i:
	.space	4
	.type	current_number, %object
	.size	current_number, 4
current_number:
	.space	4
	.type	u32tmp, %object
	.size	u32tmp, 4
u32tmp:
	.space	4
	.type	j, %object
	.size	j, 1
j:
	.space	1
	.space	3
	.type	sp, %object
	.size	sp, 4
sp:
	.space	4
	.type	b_sam_ba_interface_usart, %object
	.size	b_sam_ba_interface_usart, 1
b_sam_ba_interface_usart:
	.space	1
	.space	3
	.type	src_buff_addr.11922, %object
	.size	src_buff_addr.11922, 4
src_buff_addr.11922:
	.space	4
	.type	read_job, %object
	.size	read_job, 1
read_job:
	.space	1
	.space	3
	.type	sam_ba_cdc, %object
	.size	sam_ba_cdc, 12
sam_ba_cdc:
	.space	12
	.type	bmRequestType.10449, %object
	.size	bmRequestType.10449, 1
bmRequestType.10449:
	.space	1
	.type	bRequest.10450, %object
	.size	bRequest.10450, 1
bRequest.10450:
	.space	1
	.type	wValue.10452, %object
	.size	wValue.10452, 2
wValue.10452:
	.space	2
	.type	wIndex.10453, %object
	.size	wIndex.10453, 2
wIndex.10453:
	.space	2
	.type	wLength.10454, %object
	.size	wLength.10454, 2
wLength.10454:
	.space	2
	.type	wStatus.10455, %object
	.size	wStatus.10455, 2
wStatus.10455:
	.space	2
	.type	dir.10451, %object
	.size	dir.10451, 1
dir.10451:
	.space	1
	.space	1
	.type	pulSketch_Start_Address, %object
	.size	pulSketch_Start_Address, 4
pulSketch_Start_Address:
	.space	4
	.type	b_sharp_received, %object
	.size	b_sharp_received, 1
b_sharp_received:
	.space	1
	.type	idx_rx_read, %object
	.size	idx_rx_read, 1
idx_rx_read:
	.space	1
	.type	idx_rx_write, %object
	.size	idx_rx_write, 1
idx_rx_write:
	.space	1
	.type	idx_tx_read, %object
	.size	idx_tx_read, 1
idx_tx_read:
	.space	1
	.type	idx_tx_write, %object
	.size	idx_tx_write, 1
idx_tx_write:
	.space	1
	.type	main_b_cdc_enable, %object
	.size	main_b_cdc_enable, 1
main_b_cdc_enable:
	.space	1
	.type	buffer_rx_usart, %object
	.size	buffer_rx_usart, 128
buffer_rx_usart:
	.space	128
	.type	buffer_tx_usart, %object
	.size	buffer_tx_usart, 128
buffer_tx_usart:
	.space	128
	.space	2
	.type	udd_ep_in_cache_buffer, %object
	.size	udd_ep_in_cache_buffer, 128
udd_ep_in_cache_buffer:
	.space	128
	.type	udd_ep_out_cache_buffer, %object
	.size	udd_ep_out_cache_buffer, 128
udd_ep_out_cache_buffer:
	.space	128
	.type	usb_endpoint_table, %object
	.size	usb_endpoint_table, 128
usb_endpoint_table:
	.space	128
	.section	.isr_vector,"a",%progbits
	.align	2
	.type	exception_table, %object
	.size	exception_table, 64
exception_table:
	.word	__StackTop
	.word	Reset_Handler
	.word	NMI_Handler
	.word	HardFault_Handler
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	SVC_Handler
	.word	0
	.word	0
	.word	PendSV_Handler
	.word	SysTick_Handler
	.section	.rodata.str1.1,"aMS",%progbits,1
.LC18:
	.ascii	"\012\015\000"
.LC21:
	.ascii	"v\000"
.LC23:
	.ascii	" \000"
.LC25:
	.ascii	" Apr  9 2025 19:20:51\012\015\000"
.LC27:
	.ascii	"X\012\015\000"
.LC29:
	.ascii	"Y\012\015\000"
.LC32:
	.ascii	"Z\000"
.LC34:
	.ascii	"#\012\015\000"
.LC36:
	.ascii	">\000"
.LC61:
	.ascii	"Arduino LLC\000"
.LC63:
	.ascii	"Arduino Zero\000"
	.ident	"GCC: (GNU Tools for Arm Embedded Processors 7-2017-q4-major) 7.2.1 20170904 (release) [ARM/embedded-7-branch revision 255204]"
