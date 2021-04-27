#include <asm.h>
#include <segment.h>

ENTRY (sys_call_table)       // %eax
	.long sys_ni_syscall // 0    not implemented
	.long sys_exit       // 1    exit
	.long sys_fork       // 2    fork
	.long sys_read	     // 3    read
	.long sys_write      // 4    write
	.long sys_open_tty_ro// 5    open_tty_ro
	.long sys_ni_syscall // 6    not implemented
	.long sys_close      // 7    close
	.long sys_create_screen // 8    createScreen
	.long sys_set_focus  // 9    setFocus
	.long sys_gettime    // 10   gettime
	.long sys_ni_syscall // 11   not implemented
	.long sys_ni_syscall // 12   not implemented
	.long sys_ni_syscall // 13   not implemented
	.long sys_ni_syscall // 14   not implemented
	.long sys_ni_syscall // 15   not implemented
	.long sys_ni_syscall // 16   not implemented
	.long sys_ni_syscall // 17   not implemented
	.long sys_ni_syscall // 18   not implemented
	.long sys_ni_syscall // 19   not implemented
	.long sys_getpid     // 20   getpid
	.long sys_ni_syscall // 21   not implemented
	.long sys_ni_syscall // 22   not implemented
	.long sys_ni_syscall // 23   not implemented
	.long sys_ni_syscall // 24   not implemented
	.long sys_ni_syscall // 25   not implemented
	.long sys_ni_syscall // 26   not implemented
	.long sys_ni_syscall // 27   not implemented
	.long sys_ni_syscall // 28   not implemented
	.long sys_ni_syscall // 29   not implemented
	.long sys_ni_syscall // 30   not implemented
	.long sys_ni_syscall // 31   not implemented
	.long sys_ni_syscall // 32   not implemented
	.long sys_ni_syscall // 33   not implemented
	.long sys_ni_syscall // 34   not implemented
	.long sys_get_stats  // 35   get_stats

.globl MAX_SYSCALL
MAX_SYSCALL = (. - sys_call_table) / 4
