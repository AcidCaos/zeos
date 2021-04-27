#include <libc.h>
#include <adam.h>
#include <adamlib.h>

int must_close;

int adam () {
  
  char head[] = "adam@zeos> ";
  char buffer[1024];
  must_close = 0;
  
  print("\nAdam [Version 1.0]\n");
  print("A simple shell in ZeOS, the father of all processes.\n");
  print("Enter 'h' for help.\n\n");
  
  while (must_close == 0) {
    
    print(head);
    input(buffer); // Reads until [ENTER] is hit.
    
    execute(buffer);
    
  }
  
  print("Adam ended.\n");
  
  return 0;
}


int execute(char * command) {
  
  // Comandes Documentades
  if      (strequ(command, "h") || strequ(command, "help")) help();
  else if (strequ(command, "ping")) print("pong\n");
  else if (strequ(command, "test")) test();
  else if (strequ(command, "stats")) printstats();
  else if (strequ(command, "exit") || strequ(command, "quit")) must_close = 1;
  else if (strequ(command, "eva")) new_adam_tty();
  else if (strequ(command, ""));
  // Easter-eggs i altres...
  else if (strequ(command, "adam")) print("i eva!\n");
  else if (strequ(command, "walls")) walls();
  else if (strequ(command, "bye")) print("Sorry, this is not ftp... Try with 'quit'.\n");
  else    {
    print("Command '"); print(command); print("' does not exist.\n");
    return -1;
  }
  return 0;
}

void help() {
  print("Adam is a simple shell in ZeOS.\n");
  print("List of accepted commands:\n\n");
  print("       h - Shows this help message.\n");
  print("    ping - Answers 'pong'.\n");
  print("    test - Executes a series of tests for all syscalls.\n");
  print("   stats - Shows Adam process stats.\n");
  print("   walls - A little game. (Don't touch the walls!)\n");
  print("    exit - Closes the shell.\n");
  print("     eva - New Adam to another TTY.\n");
  print("\n");
}

void printstats() {
  struct stats s;
  if ( get_stats(getpid(), &s) < 0 ) perror();
  pstatsline (&s);
}

void test() {
  
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

  // TEST 5
  print("\n## -> TEST 5 : getpid() + fork() + get_stats()\n");
  
  char name[32];
  int ret, ret2;
  int lim;
  
  lim = gettime();
  ret = fork();
  
  if (ret == 0)       { strcpy(name, "F-111"); lim += 40;} // Primer fill
  else                { strcpy(name, "ADAM!"); lim += 400;} // Pare (i.e. task1/adam)

  ret2 = fork();
  if (ret2 == 0) {
        lim = gettime();
  	if (ret == 0) {strcpy(name, "F-333"); lim += 230;}  // Tercer fill (fill del primer fill)
  	else          {strcpy(name, "F-222"); lim += 160;}   // Segon fill  (altre fill de task1/adam)
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
  
  
  // TEST 6
  if (ret == 0 || ret2 == 0) exit();
  print("\n## -> TEST 6 : exit() all except Adam.\n");
  print("Only ADAM sais this. Others are dead.\n");

  
}

void new_adam_tty() {
  char buff[32];
  int ret;
  ret = fork();
  if (ret == 0) { // Fill
    close(1); // Tancar STDIN
    ret = createScreen();
    if (ret != 1) {
      close(ret); // Per si no sobreescriu STDIN
      exit();
    }
    adam();
    setFocus(ret);
    exit();
  }
  itoa (ret, buff);
  print("Done! New Adam (PID ");
  strcat(buff, ")\n");
  print(buff);
}



