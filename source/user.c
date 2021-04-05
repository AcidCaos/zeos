#include <libc.h>


void print (char * s);
void pstats (struct stats* s);
void pstatsline (struct stats* s);


int __attribute__ ((__section__(".text.main"))) main(void) {
  
  char buff[1024];
  
  // TEST 1
  print("\n## -> TEST 1 : a normal write()\n");
  strcpy(buff, "Well, in fact this is a normal write()\n");
  if ( write(1, buff, strlen(buff)) < 0 ) perror(); // Should work
  
  // TEST 2
  print("\n## -> TEST 2 : write() with an 'invalid' file descriptor\n");
  if ( write(22, buff, -1) < 0 ) perror(); // Invalid fd
  
  // TEST 3
  print("\n## -> TEST 3 : write() with an invalid size\n");
  if ( write(1, buff, -1) < 0 ) perror(); // Invalid size
  
  // TEST 4
  print("\n## -> TEST 4 : gettime()\n");
  print("Number of ticks is ");
  itoa(gettime(), buff);
  strcat(buff, "\n");
  print(buff);

  // TEST 5 (interactive)
  print("\n## -> TEST 5 : press any key.\n");
  print("The screen can scroll down...\n");

  // TEST 6
  print("\n## -> TEST 6 : getpid() + fork() + get_stats(..)\n");
  
  char name[32];
  int ret, ret2;
  int lim = 400;
  
  ret = fork();
  strcpy(name, "ADAM!");
  if (ret == 0) {
  	strcpy(name, "F-111");
  	lim = 140;
  }
  ret2 = fork();
  if (ret2 == 0) {
  	if (ret == 0) {strcpy(name, "F-333"); lim = 240;}
  	else					{strcpy(name, "F-222"); lim = 90;}
  }
  
  int my_pid = getpid();
  print(name);
  print("> PID=");
  itoa(my_pid, buff);
  strcat(buff, "\n");
  print(buff);
  if (ret == 0 && ret2 == 0) print("\n");
  
  int last, now;
  last = 0;
  while(last < lim) {  // Wait some time ...
    now = gettime();
    if (now > last) {
      last = now;
    }
  }
  
  // STATS
  struct stats s;
  if ( get_stats(my_pid, &s) < 0 ) perror();
  //pstats (&s); // prints stats (too big.. small screen)
  pstatsline (&s); // prints stats in one line
  
  
  // TEST 8
  if (ret == 0 || ret2 == 0) exit();
  print("\n## -> TEST 7 : exit() all except Adam (father of all)\n");
  print("Only ADAM sais this. Others are dead. Now ADAM will exit(), and CPU goes idle\n");
  
  // TEST 9
  print("\n\n Trying to read from stdin...\n");
  char in[256];
  ret = read(0, in, 5); // Read 1 byte (1 character)
  if (ret < 0) perror();

  print("main() : Done! : ");
  if (strlen(in) < 1) print("size of in[] < 1\n");
  print(in);

  print("ADAM ended.");
  exit();
  return 0; // Never reached
}

void input() {
	char in[256];
  int ret = read(0, in, 5); // Read 1 byte (1 character)
  if (ret < 0) perror();
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



