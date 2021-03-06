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
#define EOI \
      movb $0x20, %al; \
      outb %al, $0x20;


ENTRY(clock_handler)
  SAVE_ALL
  EOI // clock interrupt : EOI before the call to service routine !!
  call clock_routine
  RESTORE_ALL
  iret


ENTRY(keyboard_handler)
  SAVE_ALL
  call keyboard_routine
  RESTORE_ALL
  EOI
  iret


ENTRY(system_call_handler)
  SAVE_ALL

  cmpl 	$0, %eax     // 0 <= eax <= MAX_SYSCALL
  jl	invalid_eax
  cmpl 	$MAX_SYSCALL, %eax
  jg	invalid_eax

  call 	*sys_call_table(, %eax, 4)
  jmp fin

invalid_eax:
  movl $-38, %eax /* ENOSYS: Function not implemented */

fin:
  movl %eax, 0x18(%esp) // 0x18 == 24  <--- modify software context!
  RESTORE_ALL
  EOI
  iret
























