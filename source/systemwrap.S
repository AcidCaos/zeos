#include <asm.h>

// void task_switch (union task_union* new);
ENTRY(task_switch)
	push	%ebp
	mov	%esp, %ebp

	push	%esi
	push	%edi
	push	%ebx
	
	push	8(%ebp)
	call 	inner_task_switch
	addl	$4, %esp // undo push from above
	
	pop	%ebx
	pop	%edi
	pop	%esi

	pop	%ebp
	ret

// void asm_inner_task_switch (unsigned long * curr_k_esp, unsigned long new_k_esp);
ENTRY(asm_inner_task_switch)

	push	%ebp
	mov	%esp, %ebp
	//push	%ebx <-- això la lia amb com construim el system stack de idle a init_idle()
	
	movl	8(%ebp), %eax
	movl	%ebp, (%eax) // Store current ESP -- current()->kernel_esp = %EBP  (%ESP == %EBP here)
	
	movl	12(%ebp), %esp // Restore new ESP -- %ESP = new->task.kernel_esp

	//pop	%ebx
	pop 	%ebp // Restore %EBP from new Stack

	ret

// int asm_get_ebp();
ENTRY(asm_get_ebp)
	mov	%ebp, %eax
	ret



