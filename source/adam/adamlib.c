#include <libc.h>
#include <adamlib.h>

#define MAX_READ_SIZE 256


char readchar () {
  char one;
  read(0, &one, 1);
  return one;
}


void input(char* in) {
  char buff[MAX_READ_SIZE];
  int count = 0;
  
  while (count < MAX_READ_SIZE - 1) { // -1 pel \0 del final
    
    char ch = readchar();
    
    if (ch == '\n') {
      buff[count] = '\0';
      strcpy(in, buff);
      break;
    }
    
    //itoa(strlen(buff), debug); // Fixed in devices.c ()
    
    buff[count] = ch;
    count++;
  }
}

int input_number() {
  char buff[32];
  input(buff);
  return atoi(buff);
}

void print (char * s) {
	int ret = write(1, s, strlen(s));
  if (ret < 0) perror();
}


void pstats (struct stats* s) {
  char buff[64];
  print("\n Stats for PID = ");
  itoa(getpid(), buff);
  strcat(buff, "\n");
  print(buff);
	print("\n user_ticks = ");
  itoa(s->user_ticks, buff);
  strcat(buff, "\n system_ticks = ");
  print(buff);
  itoa(s->system_ticks, buff); 
  strcat(buff, "\n blocked_ticks = ");
  print(buff);
  itoa(s->blocked_ticks, buff); 
  strcat(buff, "\n ready_ticks = ");
  print(buff);
  itoa(s->ready_ticks, buff); 
  strcat(buff, "\n total_trans = ");
  print(buff);
  itoa(s->total_trans, buff);
  strcat(buff, "\n elapsed_total_ticks = ");
  print(buff);
  itoa(s->elapsed_total_ticks, buff);
  strcat(buff, "\n remaining_ticks = ");
  print(buff);
  itoa(s->remaining_ticks, buff);
  strcat(buff, "\n\n");
  print(buff);
}


void pstatsline (struct stats* s) {
  char buff[64];
  int procticks = s->user_ticks + s->system_ticks + s->ready_ticks + s->blocked_ticks;
  // PID
  print("PID=");
  itoa(getpid(), buff);
  print(buff);
  // usr
	print("; usr="); itoa(s->user_ticks, buff); print(buff);
	print("("); itoa((100*s->user_ticks)/procticks, buff); print(buff); print("%)");
  // sys
  print("; sys="); itoa(s->system_ticks, buff); print(buff);
	print("("); itoa((100*s->system_ticks)/procticks, buff); print(buff); print("%)");
  // rdy
  print("; rdy="); itoa(s->ready_ticks, buff); print(buff);
	print("("); itoa((100*s->ready_ticks)/procticks, buff); print(buff); print("%)");
  // blk
  print("; blk="); itoa(s->blocked_ticks, buff); print(buff);
	print("("); itoa((100*s->blocked_ticks)/procticks, buff); print(buff); print("%)");
  print("\n");
}

