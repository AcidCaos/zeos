#ifndef __CYCLIC_BUFFER__
#define __CYCLIC_BUFFER__


#define CON_BUFFER_SIZE 1024


struct cyclic_buffer {
  //                      head    tail
  //                        v     v
  // [.....................|abcde|.........] <-- buff[]
  //                       <----->
  //                        size

  char* head;
  char* tail;
  int size;
  char buff[CON_BUFFER_SIZE];
};


// Cyclic buffer operations

void init_cyclic_buffer(struct cyclic_buffer* cb);
int cyclic_buffer_is_full (struct cyclic_buffer* cb);
int cyclic_buffer_is_empty (struct cyclic_buffer* cb);
int cyclic_buffer_push (struct cyclic_buffer* cb, char c);
char cyclic_buffer_pop (struct cyclic_buffer* cb);


#endif
