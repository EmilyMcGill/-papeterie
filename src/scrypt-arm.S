/*
 * Copyright 2012 pooler@litecoinpool.org
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.  See COPYING for more details.
 */


#if defined(OPTIMIZED_SALSA) &&  defined(__arm__) && defined(__APCS_32__)

#if defined(__linux__) && defined(__ELF__)
.section .note.GNU-stack,"",%progbits
#endif

#if defined(__ARM_ARCH_5E__) || defined(__ARM_ARCH_5TE__) || \
	defined(__ARM_ARCH_5TEJ__) || defined(__ARM_ARCH_6__) || \
	defined(__ARM_ARCH_6J__) || defined(__ARM_ARCH_6K__) || \
	defined(__ARM_ARCH_6M__) || defined(__ARM_ARCH_6T2__) || \
	defined(__ARM_ARCH_6Z__) || defined(__ARM_ARCH_6ZK__)
#define __ARM_ARCH_5E_OR_6__
#endif

#if defined(__ARM_ARCH_5E_OR_6__) || defined(__ARM_ARCH_7__) || \
	defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || \
	defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)
#define __ARM_ARCH_5E_OR_6_OR_7__
#endif

#ifdef __ARM_ARCH_5E_OR_6__

#define scrypt_shuffle() \
	add	lr, r0, #9*4; \
	ldmia	r0, {r2-r7}; \
	ldmia	lr, {r2, r8-r12, lr}; \
	str	r3, [r0, #5*4]; \
	str	r5, [r0, #15*4]; \
	str	r6, [r0, #12*4]; \
	str	r7, [r0, #1*4]; \
	ldr r5, [r0, #7*4]; \
	str	r2, [r0, #13*4]; \
	str	r8, [r0, #2*4]; \
	strd	r4, [r0, #10*4]; \
	str	r9, [r0, #7*4]; \
	str	r10, [r0, #4*4]; \
	str	r11, [r0, #9*4]; \
	str	lr, [r0, #3*4]; \
	add	r2, r0, #64+0*4; \
	add	lr, r0, #64+9*4; \
	ldmia	r2, {r2-r7}; \
	ldmia	lr, {r2, r8-r12, lr}; \
	str	r3, [r0, #64+5*4]; \
	str	r5, [r0, #64+15*4]; \
	str	r6, [r0, #64+12*4]; \
	str	r7, [r0, #64+1*4]; \
	ldr r5, [r0, #64+7*4]; \
	str	r2, [r0, #64+13*4]; \
	str	r8, [r0, #64+2*4]; \
	strd	r4, [r0, #64+10*4]; \
	str	r9, [r0, #64+7*4]; \
	str	r10, [r0, #64+4*4]; \
	str	r11, [r0, #64+9*4]; \
	str	lr, [r0, #64+3*4]; \


#define salsa8_core_doubleround_body() \
	add	r6, r2, r6; \
	add	r7, r3, r7; \
	eor	r10, r10, r6, ror #25; \
	add	r6, r0, r4; \
	eor	r11, r11, r7, ror #25; \
	add	r7, r1, r5; \
	strd	r10, [sp, #14*4]; \
	eor	r12, r12, r6, ror #25; \
	eor	lr, lr, r7, ror #25; \
	ldrd	r6, [sp, #10*4]; \
	add	r2, r10, r2; \
	add	r3, r11, r3; \
	eor	r6, r6, r2, ror #23; \
	add	r2, r12, r0; \
	eor	r7, r7, r3, ror #23; \
	add	r3, lr, r1; \
	strd	r6, [sp, #10*4]; \
	eor	r8, r8, r2, ror #23; \
	eor	r9, r9, r3, ror #23; \
	ldrd	r2, [sp, #6*4]; \
	add	r10, r6, r10; \
	add	r11, r7, r11; \
	eor	r2, r2, r10, ror #19; \
	add	r10, r8, r12; \
	eor	r3, r3, r11, ror #19; \
	add	r11, r9, lr; \
	eor	r4, r4, r10, ror #19; \
	eor	r5, r5, r11, ror #19; \
	ldrd	r10, [sp, #2*4]; \
	add	r6, r2, r6; \
	add	r7, r3, r7; \
	eor	r10, r10, r6, ror #14; \
	add	r6, r4, r8; \
	eor	r11, r11, r7, ror #14; \
	add	r7, r5, r9; \
	eor	r0, r0, r6, ror #14; \
	eor	r1, r1, r7, ror #14; \
	ldrd	r6, [sp, #14*4]; \
	strd	r2, [sp, #6*4]; \
	strd	r10, [sp, #2*4]; \
	add	r6, r11, r6; \
	add	r7, r0, r7; \
	eor	r4, r4, r6, ror #25; \
	add	r6, r1, r12; \
	eor	r5, r5, r7, ror #25; \
	add	r7, r10, lr; \
	eor	r2, r2, r6, ror #25; \
	eor	r3, r3, r7, ror #25; \
	strd	r2, [sp, #6*4]; \
	add	r10, r3, r10; \
	ldrd	r6, [sp, #10*4]; \
	add	r11, r4, r11; \
	eor	r8, r8, r10, ror #23; \
	add	r10, r5, r0; \
	eor	r9, r9, r11, ror #23; \
	add	r11, r2, r1; \
	eor	r6, r6, r10, ror #23; \
	eor	r7, r7, r11, ror #23; \
	strd	r6, [sp, #10*4]; \
	add	r2, r7, r2; \
	ldrd	r10, [sp, #14*4]; \
	add	r3, r8, r3; \
	eor	r12, r12, r2, ror #19; \
	add	r2, r9, r4; \
	eor	lr, lr, r3, ror #19; \
	add	r3, r6, r5; \
	eor	r10, r10, r2, ror #19; \
	eor	r11, r11, r3, ror #19; \
	ldrd	r2, [sp, #2*4]; \
	add	r6, r11, r6; \
	add	r7, r12, r7; \
	eor	r0, r0, r6, ror #14; \
	add	r6, lr, r8; \
	eor	r1, r1, r7, ror #14; \
	add	r7, r10, r9; \
	eor	r2, r2, r6, ror #14; \
	eor	r3, r3, r7, ror #14; \


#define salsa8_core() \
	ldmia	sp, {r0-r12, lr}; \
	ldrd	r10, [sp, #14*4]; \
	salsa8_core_doubleround_body(); \
	ldrd	r6, [sp, #6*4]; \
	strd	r2, [sp, #2*4]; \
	strd	r10, [sp, #14*4]; \
	salsa8_core_doubleround_body(); \
	ldrd	r6, [sp, #6*4]; \
	strd	r2, [sp, #2*4]; \
	strd	r10, [sp, #14*4]; \
	salsa8_core_doubleround_body(); \
	ldrd	r6, [sp, #6*4]; \
	strd	r2, [sp, #2*4]; \
	strd	r10, [sp, #14*4]; \
	salsa8_core_doubleround_body(); \
	stmia	sp, {r0-r5}; \
	strd	r8, [sp, #8*4]; \
	str	r12, [sp, #12*4]; \
	str	lr, [sp, #13*4]; \
	strd	r10, [sp, #14*4]; \


#else

#define scrypt_shuffle() \


#define salsa8_core_doubleround_body() \
	ldr	r8, [sp, #8*4]; \
	add	r11, r11, r10; \
	ldr	lr, [sp, #13*4]; \
	add	r12, r12, r3; \
	eor	r2, r2, r11, ror #23; \
	add	r11, r4, r0; \
	eor	r7, r7, r12, ror #23; \
	add	r12, r9, r5; \
	str	r9, [sp, #9*4]; \
	eor	r8, r8, r11, ror #23; \
	str	r10, [sp, #14*4]; \
	eor	lr, lr, r12, ror #23; \
	ldr	r11, [sp, #11*4]; \
	add	r9, lr, r9; \
	ldr	r12, [sp, #12*4]; \
	add	r10, r2, r10; \
	eor	r1, r1, r9, ror #19; \
	add	r9, r7, r3; \
	eor	r6, r6, r10, ror #19; \
	add	r10, r8, r4; \
	str	r8, [sp, #8*4]; \
	eor	r11, r11, r9, ror #19; \
	str	lr, [sp, #13*4]; \
	eor	r12, r12, r10, ror #19; \
	ldr	r9, [sp, #10*4]; \
	add	r8, r12, r8; \
	ldr	r10, [sp, #15*4]; \
	add	lr, r1, lr; \
	eor	r0, r0, r8, ror #14; \
	add	r8, r6, r2; \
	eor	r5, r5, lr, ror #14; \
	add	lr, r11, r7; \
	eor	r9, r9, r8, ror #14; \
	ldr	r8, [sp, #9*4]; \
	eor	r10, r10, lr, ror #14; \
	ldr	lr, [sp, #14*4]; \
	add	r8, r9, r8; \
	str	r9, [sp, #10*4]; \
	add	lr, r10, lr; \
	str	r10, [sp, #15*4]; \
	eor	r11, r11, r8, ror #25; \
	add	r8, r0, r3; \
	eor	r12, r12, lr, ror #25; \
	add	lr, r5, r4; \
	eor	r1, r1, r8, ror #25; \
	ldr	r8, [sp, #8*4]; \
	eor	r6, r6, lr, ror #25; \
	add	r9, r11, r9; \
	ldr	lr, [sp, #13*4]; \
	add	r10, r12, r10; \
	eor	r8, r8, r9, ror #23; \
	add	r9, r1, r0; \
	eor	lr, lr, r10, ror #23; \
	add	r10, r6, r5; \
	str	r11, [sp, #11*4]; \
	eor	r2, r2, r9, ror #23; \
	str	r12, [sp, #12*4]; \
	eor	r7, r7, r10, ror #23; \
	ldr	r9, [sp, #9*4]; \
	add	r11, r8, r11; \
	ldr	r10, [sp, #14*4]; \
	add	r12, lr, r12; \
	eor	r9, r9, r11, ror #19; \
	add	r11, r2, r1; \
	eor	r10, r10, r12, ror #19; \
	add	r12, r7, r6; \
	str	r8, [sp, #8*4]; \
	eor	r3, r3, r11, ror #19; \
	str	lr, [sp, #13*4]; \
	eor	r4, r4, r12, ror #19; \


#define salsa8_core() \
	ldmia	sp, {r0-r7}; \
	ldr	r12, [sp, #15*4]; \
	ldr	r8, [sp, #11*4]; \
	ldr	lr, [sp, #12*4]; \
	ldr	r9, [sp, #9*4]; \
	add	r8, r8, r12; \
	ldr	r11, [sp, #10*4]; \
	add	lr, lr, r0; \
	eor	r3, r3, r8, ror #25; \
	add	r8, r5, r1; \
	ldr	r10, [sp, #14*4]; \
	eor	r4, r4, lr, ror #25; \
	add	lr, r11, r6; \
	eor	r9, r9, r8, ror #25; \
	eor	r10, r10, lr, ror #25; \
	salsa8_core_doubleround_body(); \
	ldr	r11, [sp, #10*4]; \
	add	r8, r9, r8; \
	ldr	r12, [sp, #15*4]; \
	add	lr, r10, lr; \
	eor	r11, r11, r8, ror #14; \
	add	r8, r3, r2; \
	eor	r12, r12, lr, ror #14; \
	add	lr, r4, r7; \
	eor	r0, r0, r8, ror #14; \
	ldr	r8, [sp, #11*4]; \
	eor	r5, r5, lr, ror #14; \
	ldr	lr, [sp, #12*4]; \
	add	r8, r8, r12; \
	str	r11, [sp, #10*4]; \
	add	lr, lr, r0; \
	str	r12, [sp, #15*4]; \
	eor	r3, r3, r8, ror #25; \
	add	r8, r5, r1; \
	eor	r4, r4, lr, ror #25; \
	add	lr, r11, r6; \
	str	r9, [sp, #9*4]; \
	eor	r9, r9, r8, ror #25; \
	str	r10, [sp, #14*4]; \
	eor	r10, r10, lr, ror #25; \
	salsa8_core_doubleround_body(); \
	ldr	r11, [sp, #10*4]; \
	add	r8, r9, r8; \
	ldr	r12, [sp, #15*4]; \
	add	lr, r10, lr; \
	eor	r11, r11, r8, ror #14; \
	add	r8, r3, r2; \
	eor	r12, r12, lr, ror #14; \
	add	lr, r4, r7; \
	eor	r0, r0, r8, ror #14; \
	ldr	r8, [sp, #11*4]; \
	eor	r5, r5, lr, ror #14; \
	ldr	lr, [sp, #12*4]; \
	add	r8, r8, r12; \
	str	r11, [sp, #10*4]; \
	add	lr, lr, r0; \
	str	r12, [sp, #15*4]; \
	eor	r3, r3, r8, ror #25; \
	add	r8, r5, r1; \
	eor	r4, r4, lr, ror #25; \
	add	lr, r11, r6; \
	str	r9, [sp, #9*4]; \
	eor	r9, r9, r8, ror #25; \
	str	r10, [sp, #14*4]; \
	eor	r10, r10, lr, ror #25; \
	salsa8_core_doubleround_body(); \
	ldr	r11, [sp, #10*4]; \
	add	r8, r9, r8; \
	ldr	r12, [sp, #15*4]; \
	add	lr, r10, lr; \
	eor	r11, r11, r8, ror #14; \
	add	r8, r3, r2; \
	eor	r12, r12, lr, ror #14; \
	add	lr, r4, r7; \
	eor	r0, r0, r8, ror #14; \
	ldr	r8, [sp, #11*4]; \
	eor	r5, r5, lr, ror #14; \
	ldr	lr, [sp, #12*4]; \
	add	r8, r8, r12; \
	str	r11, [s