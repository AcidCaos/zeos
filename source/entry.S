/*
 * entry.S - Entry point to system mode from user mode -- HANDLERS
 */

#include <asm.h>
#include <segment.h>


/**************************************************/
/**** Save & Restore ******************************/
/**                                              **/
/** When we change to privilege level 0 (kernel) **/
/** (through an interrupt, a system call, an     **/
/** exception ...) we must save the state of the **/
/** currently running task (save).               **/
/**                                              **/
/** Stack layout in 'systemCall':                **/
/**                                              **/
/**   0(%esp) - %ebx    \                        **/
/**   4(%esp) - %ecx     |                       **/
/**   8(%esp) - %edx     |                       **/
/**   C(%esp) - %esi     | Register saved        **/
/**  10(%esp) - %edi     |  by 'save'            **/
/**  14(%esp) - %ebp     |                       **/
/**  18(%esp) - %eax     |                       **/
/**  1C(%esp) - %ds      |                       **/
/**  20(%esp) - %es      |                       **/
/**  24(%esp) - %fs      |                       **/
/**  28(%esp) - %gs     /                        **/
/**  2C(%esp) - %eip    \                        **/
/**  30(%esp) - %cs      |                       **/
/**  34(%esp) - %eflags  |  Return context saved **/
/**  38(%esp) - %oldesp  |   by the processor.   **/
/**  3C(%esp) - %oldss  /                        **/
/**                                              **/
/**************************************************/

#define SAVE_ALL \
      pushl %gs; \
      pushl %fs; \
      pushl %es; \
      pushl %ds; \
      pushl %eax; \
      pushl %ebp; \
      pushl %edi; \
      pushl %esi; \
      pushl %edx; \
      pushl %ecx; \
      pushl %ebx; \
      movl $__KERNEL_DS, %edx; \
      movl %edx, %ds; \
      movl %edx, %es

#define RESTORE_ALL \
      popl %ebx; \
      popl %ecx; \
      popl %edx; \
      popl %esi; \
      popl %edi; \
      popl %ebp; \
      popl %eax; \
      popl %ds; \
      popl %es; \
      popl %fs; \
      popl %gs;

/*  EOI :
  notify the system that we have received the interrupt, 
  and, therefore, we are ready to receive new interrupts.
*/

// For interrupts IRQ 0 .. 8
#define EOI \
      movb $0x20, %al; \
      outb %al, $0x20;

// For interrupts IRQ 8 .. 15 --> Managed by the Slave PIC. (Send EOI to both)

#define EOI_PIC2 \
      movb $0x20, %al; \
      outb %al, $0x20; \
      outb %al, $0xA0;


#define USER2SYS \
      push %eax; \
      call chstat_user_to_sys; \
      pop %eax;

#define SYS2USER \
      push %eax; \
      call chstat_sys_to_user; \
      pop %eax;


ENTRY(writeMsr)
  push %ebp
  movl %esp, %ebp
  movl 8(%ebp), %ecx
  movl 12(%ebp), %eax
  movl $0, %edx
  wrmsr // MSR[%ecx] = %edx:%eax (%edx is upper bits, set to zero)
  pop %ebp
  ret


ENTRY(clock_handler)
  SAVE_ALL
  USER2SYS
  EOI // clock interrupt : EOI before the call to service routine !!
  call clock_routine
  SYS2USER
  RESTORE_ALL
  iret


ENTRY(keyboard_handler)
  SAVE_ALL
  USER2SYS
  call keyboard_routine
  EOI
  SYS2USER
  RESTORE_ALL
  iret


ENTRY(ps2_mouse_handler)
  SAVE_ALL
  USER2SYS
  call ps2_mouse_routine
  EOI_PIC2
  SYS2USER
  RESTORE_ALL
  iret


ENTRY(syscall_handler_sysenter)
  push 	$__USER_DS // user data segment
  push 	%ebp // user return address
  pushfl	// push flags register
  push 	$__USER_CS // user code segment
  push 	4(%ebp) // user return address

  // Save Context
  SAVE_ALL
  USER2SYS

  // 0 <= eax <= MAX_SYSCALL
  cmpl 	$0, %eax     
  jl	sysenter_invalid_eax
  cmpl 	$MAX_SYSCALL, %eax
  jg	sysenter_invalid_eax

  // call service routine
  call 	*sys_call_table(, %eax, 4)
  jmp sysenter_fin

sysenter_invalid_eax:
  /* ENOSYS: Function not implemented */
  movl $-38, %eax 

sysenter_fin:
  // Return value:  modify software context! (0x18 == 24, eax position in stack)
  movl %eax, 0x18(%esp) 
  SYS2USER
  RESTORE_ALL
  // Return address
  movl (%esp), %edx
  // User stack address
  movl 12(%esp), %ecx
  // Enable interrupts again
  sti
  sysexit



ENTRY(syscall_handler) // UNUSED (see interrupt.c) // PARAMETERS ARE NOT CORRECTLY PASSED TO THE syscall service routine
  SAVE_ALL
  USER2SYS

  cmpl 	$0, %eax     // 0 <= eax <= MAX_SYSCALL
  jl	int_invalid_eax
  cmpl 	$MAX_SYSCALL, %eax
  jg	int_invalid_eax

  call 	*sys_call_table(, %eax, 4)
  jmp int_fin

int_invalid_eax:
  movl $-38, %eax /* ENOSYS: Function not implemented */

int_fin:
  movl %eax, 0x18(%esp) // Return value:  modify software context! (0x18 == 24, eax position in stack)
  SYS2USER
  EOI
  RESTORE_ALL
  iret

























