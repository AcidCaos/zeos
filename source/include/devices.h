#ifndef DEVICES_H__
#define  DEVICES_H__

#include <cyclic_buffer.h>

struct list_head blocked;
struct list_head read_queue;

struct cyclic_buffer console_input;



void init_devices();

int sys_write_console(char *buffer,int size);
int sys_write_console_error(char *buffer,int size);
int sys_read_console(char* user_buff, int count);

#endif /* DEVICES_H__*/
