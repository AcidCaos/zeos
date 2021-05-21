#include <libc.h>
#include <adam.h>
#include <adamlib.h>

int must_close;

int adam () {
  
  char head[] = "\033[314madam\033[315m@\033[311mzeos\033[0m> ";
  char buffer[256];
  must_close = 0;
  
  print("\n\033[313mAdam [Version 2.0]\n");
  print("\033[35mA simple shell in ZeOS, and the father of all processes.\n");
  print("Enter 'h' for help.\033[0m\n\n");
  print("Enter 't' for testing options.\033[0m\n\n");
  
  while (must_close == 0) {
    
    print(head);
    input(buffer); // Reads until [ENTER] is hit.
    
    execute(buffer);
    
  }
  
  print("Adam ended.\033[0m\n");
  
  return 0;
}


int execute(char * command) {
  
  // Comandes Documentades
  if      (strequ(command, "h") || strequ(command, "help")) help();
  else if (strequ(command, "t") || strequ(command, "testing")) testing_help();
  else if (strequ(command, "ping")) print("pong\n");
  else if (strequ(command, "clear")) print("\033[1m");
  else if (strequ(command, "test")) test();
  else if (strequ(command, "stats")) printstats();
  else if (strequ(command, "exit") || strequ(command, "quit")) /*exit(); */ must_close = 1;
  else if (strequ(command, "eva")) new_adam_tty();
  else if (strequ(command, "")) return 0;
  // Easter-eggs i altres...
  else if (strequ(command, "adam")) print("i eva!\n");
  else if (strequ(command, "walls")) walls();
  else if (strequ(command, "bye")) print("Sorry, this is not ftp... Try with 'quit'.\n");
  else if (strequ(command, "hola")) print("parles sol?\n");
  else if (strequ(command, "menu")) menu();
  // Testing
  else if (strequ(command, "tty")) tty();
  else if (strequ(command, "close")) close_tty();
  else if (strequ(command, "epilepsia")) epilepsia();
  
  else    {
    print("Command '"); print(command); print("' does not exist.\n");
    return -1;
  }
  return 0;
}

void help() {
  print("List of accepted commands:\n\n");
  print("\033[312m       h \033[0m-\033[37m Shows this help message.\n");
  print("\033[312m       t \033[0m-\033[37m Shows testing options.\n");
  print("\033[312m    ping \033[0m-\033[37m Answers 'pong'.\n");
  print("\033[312m   clear \033[0m-\033[37m Clears the screen.\n");
  print("\033[312m    menu \033[0m-\033[37m Tests ANSI escape codes.\n");
  print("\033[312m    test \033[0m-\033[37m Executes a series of tests for all syscalls.\n");
  print("\033[312m   stats \033[0m-\033[37m Shows Adam process stats.\n");
  print("\033[312m   walls \033[0m-\033[37m A little game. (Don't touch the walls!)\n");
  print("\033[312m    exit \033[0m-\033[37m Closes the shell.\n");
  print("\033[312m     eva \033[0m-\033[37m New Adam to another TTY.\n");
  print("\033[0m\n");

}

void testing_help() {
  print("\n\033[314m TESTING: (Comanda + synopsis)   SHIFT+TAB per fer zapping.\n\n");
  print("\033[311m        eva \033[37m- fa fork, tanca el fd 1, fa una nova pantalla, i executa Adam.\n");
  print("\033[311m        tty \033[37m- obre una nova pantalla per aquest proces.\n");
  print("\033[311m      close \033[37m- tancar una pantalla.\n");
  print("\033[311m  epilepsia \033[37m- s'obren, es fa focus, i es tanquen pantalles.\n");
  print("\033[311m       test \033[37m- fa forks i prova les syscalls de la Entrega 1.\n");
  print("\033[311m      walls \033[37m- un joc per testejar el read(). Control: W A S D\n");
  
  print("\033[311m\n        \033[41m      '' Solo tu puedes decidir que hacer                   \033[0m\n");
  print("\033[311m        \033[41m     con las herramientas que se te han dado'' - Gandalf.   \033[0m\n\n");
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

void tty () {
  char buff [256];
  int fd = createScreen();
  
  // escriu a la pantalla inicial
  print("Nova tty : File Descriptor = ");
  itoa(fd, buff);
  print(buff);
  print(".\nPots tancar-la amb la comanda 'close'.\n");
  print("Fes SHIFT + TAB per arribar-hi.\n");
  
  // escriu a la nova pantalla
  strcpy(buff, "Pantalla creada amb 'tty' per PID=");
  write(fd, buff, strlen(buff));
  int my_pid = getpid();
  itoa(my_pid, buff);
  strcat(buff, "\n");
  write(fd, buff, strlen(buff));
}

void close_tty (int fd) {
  print("Indica el File Descriptor: ");
  int ret = input_number();
  ret = close(ret);
  //if (ret == 0) print("Done!\n");
  //else perror();
  if (ret < 0) perror();
}

void new_adam_tty() {
  char buff[32];
  int ret_f, ret_0, ret_1; //, ret_2;
  ret_f = fork();
  if (ret_f == 0) { // Fill
    
    // Substituir STDOUT
    close(1);
    ret_1 = createScreen();
    if (ret_1 != 1) {
      close(ret_1);
      exit();
    }
    
    // Substituir STDIN
    close(0);
    ret_0 = open_tty_ro (ret_1);
    if (ret_0 != 0) {
      close(ret_0);
      close(ret_1);
      exit();
    }
    
    // Substituir STDERR
    /*close(2);
    ret_2 = createScreen(); // must make anoter custom open for stderr...
    if (ret_2 != 2) {
      close(ret_0);
      close(ret_1);
      close(ret_2);
      exit();
    }*/
    
    adam();
    //setFocus(ret_1);
    exit();
  }
  if (ret_f > 0) {
    itoa (ret_f, buff);
    print("Done! New Adam (PID ");
    strcat(buff, ")\n");
    print(buff);
  } else {
    perror();
  }
}

void epilepsia() {
  int ret_1;
  
  for (int k = 0; k < 5; k++) {
    ret_1 = createScreen();
    setFocus(ret_1);
    write (ret_1, "HOLA", sizeof("HOLA"));
    for (int i = 0; i < 10000000; i++) if (i % 100000 == 0) write (ret_1, ".", sizeof("."));
    write (ret_1, "ADEU", sizeof("ADEU"));
    close(ret_1);
  }
  
  setFocus(1);
  
}

#define MAX_NUM_OPTIONS 30
#define MAX_STRING_SIZE 50

struct menu {
  char * title;
  int n_options;
  char * option[MAX_NUM_OPTIONS];
  int selected;
};

#define NUM_COLUMNS 80
#define NUM_ROWS    25

void print_menu(struct menu * m) {
  char buffer[256];
  char aux[32];
  
  int width, height;
  int height_offset, width_offset;
  
  // Clear screen
  print("\033[1m");
  
  // Calculate needed dimensions
  int title_len = strlen(m->title);
  if (width < title_len) width = title_len;
  for (int i = 0; i < m->n_options; i++) {
    int opt_len = strlen(m->option[i]);
    if (width < opt_len) width = opt_len;
  }
  width = width + 4;
  height = m->n_options + 4;
  
  // Calculate offset
  height_offset = NUM_ROWS/2 - height/2;
  width_offset = NUM_COLUMNS/2 - width/2 - 1;
  
  // Print Background and Options
  for (int i = 0; i < height; i++) {
    // Go to x,y pos
    strcpy (buffer, "\033[");
    itoa(width_offset, aux);
    strcat (buffer, aux);
    strcat (buffer, ";");
    itoa(height_offset + i, aux);
    strcat (buffer, aux);
    strcat (buffer, "H");
    print(buffer);
    
    // First line is title
    if (i == 0) {
      print("\033[41m  ");
      print(m->title);
      for (int j = 0; j < width - strlen(m->title) - 2; j++) print (" ");
      print("\033[0m \n");
      continue;
    }
    
    // Print menu options
    int top_margin = 2;
    int left_margin = 2;
    
    if (m->selected == i - top_margin) strcpy (buffer, "\033[44m\033[315m");
    else strcpy (buffer, "\033[47m\033[31m");
    for (int k = 0; k < left_margin; k++) strcat (buffer, " ");
    if ( i - top_margin >= 0 && i - top_margin < m->n_options) {
      strcat(buffer, m->option[i - top_margin]);
      for (int j = 0; j < width - strlen(m->option[i - top_margin]) - left_margin; j++) strcat (buffer, " ");
    } else for (int j = 0; j < width - left_margin; j++) {
      strcat (buffer, " ");
    }
    print(buffer);
  }
}

void menu() {
  char buffer[32];
  struct menu menu;
  char input;
  
  menu.title = "Menu Title";
  menu.n_options = 3;
  
  menu.option[0] = "Option 0";
  menu.option[1] = "This is option 1";
  menu.option[2] = "Option 2 ...";
  
  menu.selected = 0;
  
  while (1) {
    print_menu (& menu);
    print("\033[0;24H");
    
    input = readchar();
    if (input == '2' || input == '6') menu.selected = (menu.selected+1) % (menu.n_options);
    if (input == '4' || input == '8') menu.selected = (menu.selected+menu.n_options-1) % (menu.n_options);
    if (input == '\n') {
      print("\033[1m");
      print("\nOption selected is ");
      itoa(menu.selected, buffer);
      print(buffer);
      print(".\n");
      break;
    }
  }
}



