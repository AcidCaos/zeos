#include <asm.h>

ENTRY(write) // uses SYSENTER
	push	%ebp
	movl	%esp, %ebp
	push    %ebx

	movl	8(%ebp), %ebx /*Exploits SAVE_ALL system stack structure.*/
	movl	12(%ebp), %ecx /*Parameters passing from usermode to sys mode: through the registers.*/
        movl	16(%ebp), %edx

	movl	$0x04, %eax // syscall identifier

	pushl 	%ecx // they are modified by sysenter!!
	pushl 	%edx

	pushl $next_instr // save return direction to user (instruction after sysenter)

	// fake dynamic link: used by the handler in system mode
	pushl	%ebp
	movl	%esp, %ebp
        sysenter

next_instr:
	popl 	%ebp //(popl) pops (the fake ebp)
	addl 	$4, %esp // pops (@ instruction after sysenter)

	popl 	%edx // recover saved values
	popl 	%ecx

	cmp $0, %eax
	jl	write_error
end:
        pop     %ebx
	pop	%ebp
	ret

write_error:
	negl	%eax
	movl	%eax, errno
	movl	$-1, %eax
	jmp end


ENTRY(write_deprecated) // uses INT. Raw write: no errno support
	push	%ebp
	movl	%esp, %ebp
	push    %ebx

	movl	8(%ebp), %ebx /*Exploits SAVE_ALL system stack structure.*/
	movl	12(%ebp), %ecx /*Parameters passing from usermode to sys mode: through the registers.*/
        movl	16(%ebp), %edx

	movl	$0x04, %eax
        int	$0x80

        pop     %ebx
	pop	%ebp
	ret


//int gettime();
ENTRY(gettime) // SYSENTER
	push	%ebp
	movl	%esp, %ebp
	movl	$0x0A, %eax // syscall identifier
	pushl 	%ecx // they are modified by sysenter!!
	pushl 	%edx
	pushl 	$gettime_next_instr // save return direction to user (instruction after sysenter)
	// fake dynamic link: used by the handler in system mode
	pushl	%ebp
	movl	%esp, %ebp
        sysenter
gettime_next_instr:
	popl 	%ebp //(popl) pops (the fake ebp)
	addl 	$4, %esp // pops (@ instruction after sysenter)
	popl 	%edx // recover saved values
	popl 	%ecx
	pop	%ebp
	ret


//int getpid();
ENTRY(getpid) // SYSENTER
	push	%ebp
	movl	%esp, %ebp
	movl	$0x14, %eax // syscall identifier
	pushl 	%ecx // they are modified by sysenter!!
	pushl 	%edx
	pushl 	$getpid_next_instr // save return direction to user (instruction after sysenter)
	// fake dynamic link: used by the handler in system mode
	pushl	%ebp
	movl	%esp, %ebp
        sysenter
getpid_next_instr:
	popl 	%ebp //(popl) pops (the fake ebp)
	addl 	$4, %esp // pops (@ instruction after sysenter)
	popl 	%edx // recover saved values
	popl 	%ecx
	pop	%ebp
	ret


//int fork();
ENTRY(fork) // SYSENTER
	push	%ebp
	movl	%esp, %ebp
	movl	$0x2, %eax // syscall identifier
	pushl 	%ecx // they are modified by sysenter!!
	pushl 	%edx
	pushl 	$fork_next_instr // save return direction to user (instruction after sysenter)
	// fake dynamic link: used by the handler in system mode
	pushl	%ebp
	movl	%esp, %ebp
        sysenter
fork_next_instr:
	popl 	%ebp //(popl) pops (the fake ebp)
	addl 	$4, %esp // pops (@ instruction after sysenter)
	popl 	%edx // recover saved values
	popl 	%ecx
	pop	%ebp
	ret

//void exit();
ENTRY(exit) // SYSENTER
	push	%ebp
	movl	%esp, %ebp
	movl	$0x1, %eax // syscall identifier
	pushl 	%ecx // they are modified by sysenter!!
	pushl 	%edx
	pushl 	$exit_next_instr // save return direction to user (instruction after sysenter)
	// fake dynamic link: used by the handler in system mode
	pushl	%ebp
	movl	%esp, %ebp
        sysenter
exit_next_instr:
	popl 	%ebp //(popl) pops (the fake ebp)
	addl 	$4, %esp // pops (@ instruction after sysenter)
	popl 	%edx // recover saved values
	popl 	%ecx
	pop	%ebp
	ret




