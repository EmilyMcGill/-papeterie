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
	eor	r