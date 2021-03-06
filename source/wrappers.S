#include <asm.h>

ENTRY(write)
	push	%ebp
        push    %ebx
	movl	%esp, %ebp

	movl	8(%ebp), %ebx /*Exploits SAVE_ALL system stack structure.*/
	movl	12(%ebp), %ecx /*Parameters passing from usermode to sys mode: through the registers.*/
        movl	16(%ebp), %edx

	movl	$0x04, %eax
        int	$0x80

        pop     %ebx
	pop	%ebp
	ret
