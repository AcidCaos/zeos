#include <asm.h>

/*

.globl name; \
  .type name, @function; \
  .align 0; \
  name: \

*/
#define SYSENTER_SYSCALL(name, syscall_id) \
  ENTRY(name) \
  \
  push	%ebp; \
  movl	%esp, %ebp; \
  \
  push  %ebx; \
  push  %esi; \
  push  %edi; \
  \
  movl	 8(%ebp), %ebx; \
  movl	12(%ebp), %ecx; \
  movl	16(%ebp), %edx; \
  movl	20(%ebp), %esi; \
  movl	24(%ebp), %edi; \
  movl	28(%ebp), %ebp; \
  \
  movl	$ ## syscall_id , %eax; \
  \
  pushl %ecx; \
  pushl %edx; \
  pushl $sysenter_next_instr; \
  \
  pushl	%ebp; \
  movl	%esp, %ebp; \
  sysenter;
  
sysenter_next_instr:
  popl 	%ebp        // pops the fake ebp
  addl 	$4, %esp    // pops @ sysenter_next_instr
  
  popl 	%edx        // recover saved values
  popl 	%ecx
  
  cmp 	$0, %eax
  jge	sysenter_end  // jump if %eax not negative
  
  negl	%eax
  movl	%eax, errno
  movl	$-1, %eax
  
sysenter_end:
  pop	%edi
  pop	%esi
  pop	%ebx
  
  pop	%ebp
  ret


//void exit();
SYSENTER_SYSCALL(exit, 0x1)

//int fork();
SYSENTER_SYSCALL(fork, 0x2)

// int read(int fd, char* user_buff, int count);
SYSENTER_SYSCALL(read, 0x3)

//int write(int fd, char *buffer, int size);
SYSENTER_SYSCALL(write, 0x4)

//int close(inf fd);
SYSENTER_SYSCALL(close, 0x7)

//int createScreen();
SYSENTER_SYSCALL(createScreen, 0x8)

//int gettime();
SYSENTER_SYSCALL(gettime, 0x0A)

//int getpid();
SYSENTER_SYSCALL(getpid, 0x14)

// int get_stats(int pid, struct stats *s);
SYSENTER_SYSCALL(get_stats, 0x23)



