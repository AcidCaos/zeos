#include <asm.h>

ENTRY(addAsm)
	push	%ebp;
	mov	%esp, %ebp;
	mov	8(%ebp), %ecx;
	mov	12(%ebp), %eax;
	add	%ecx, %eax;
	pop	%ebp;
	ret;
