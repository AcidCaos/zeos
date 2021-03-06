#include <asm.h>
#include <segment.h>

ENTRY (sys_call_table)       // %eax
	.long sys_ni_syscall // 0    not implemented
	.long sys_ni_syscall // 1    not implemented
	.long sys_ni_syscall // 2    not implemented
	.long sys_ni_syscall // 3    not implemented
	.long sys_write      // 4    write

.globl MAX_SYSCALL
MAX_SYSCALL = (. - sys_call_table) / 4
