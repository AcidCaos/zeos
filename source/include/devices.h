#ifndef DEVICES_H__
#define  DEVICES_H__

#include <cyclic_buffer.h>
#include <sched.h>

struct list_head blocked;
//struct list_head read_queue;

void init_devices();

void copy_taula_canals (struct taula_canals* from, struct taula_canals* to);

void init_task1_std_io (struct taula_canals* tc);


#endif /* DEVICES_H__*/
