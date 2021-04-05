#include <cyclic_buffer.h>


void init_cyclic_buffer(struct cyclic_buffer* cb){
  cb->head = cb->buff;
  cb->tail = cb->buff;
  cb->size = 0;
}

int cyclic_buffer_is_full (struct cyclic_buffer* cb) {
  return cb->size == CON_BUFFER_SIZE;
}

int cyclic_buffer_is_empty (struct cyclic_buffer* cb) {
  return cb->size == 0;
}

int cyclic_buffer_push (struct cyclic_buffer* cb, char c) {
  //printk("cyclic_buffer_push()\n");
  if (cyclic_buffer_is_full(cb)) return 0;
  *(cb->tail) = c;
  cb->size++;
  cb->tail++;
  if (cb->tail == &(cb->buff[CON_BUFFER_SIZE])) cb->tail = cb->buff;
  //printk("cyclic_buffer_push() : EOF\n");
  return 1;
}

char cyclic_buffer_pop (struct cyclic_buffer* cb) { // Revise
  //printk("cyclic_buffer_pop()\n");
  char c = *(cb->head);
  if (cyclic_buffer_is_empty(cb)) return '\0';
  if (cb->head != cb->tail) {
    cb->size--;
    cb->head++;
    if (cb->head == &(cb->buff[CON_BUFFER_SIZE])) cb->head = cb->buff;
  }
  //printk("cyclic_buffer_pop() : EOF\n");
  return c;
}


