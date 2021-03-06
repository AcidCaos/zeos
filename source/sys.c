/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>
#include <utils.h>
#include <io.h>
#include <mm.h>
#include <mm_address.h>
#include <sched.h>

#include <ticks.h>

#define LECTURA 0
#define ESCRIPTURA 1

int check_fd(int fd, int permissions) {
  if (fd!=1) return -9; /* EBADF: Bad file number */
  if (permissions!=ESCRIPTURA) return -13; /* EACCES: Permission denied */
  return 0;
}

int sys_ni_syscall() {
	return -38; /* ENOSYS: Function not implemented */
}

int sys_getpid() {
	return current()->PID;
}

int sys_fork() {
  int PID=-1;

  // creates the child process
  
  return PID;
}

void sys_exit() {
}

int sys_write(int fd, char* buffer, int size) {

  char sys_buffer [1024];
  int ret;

  ret = check_fd(fd, ESCRIPTURA);
  if (ret != 0) return ret;
  if (buffer == NULL) return -14; /* EFAULT: Bad address */
  if (size <= 0) return -22; /* EINVAL: Invalid argument */

  copy_from_user(buffer, sys_buffer, size); /* utils.c :: copy from user-buffer to system-buffer */
  ret = sys_write_console(sys_buffer, size); /* devices.c */

  return ret; /* ret = num. of bytes written */

}

int sys_gettime () {
  return zeos_ticks;
}








