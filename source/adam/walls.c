#include <libc.h>
#include <adam.h>
#include <adamlib.h>

  char a [30];
  char face[] ="e";
  char wall[] ="#";
  int life = 3;
  int p;


void printMap(){
  for (int w = 0; w < 5; ++w) {
    print("  ");
    if (w%4 == 0) {for (int s = 0; s < 11; ++s) print (wall); print ("\n");}
    else {
      print (wall);
      char aux[2];
      for (int i = 1; i < 10; ++i){
        aux[0] = a[i+10*(w-1)];
        aux[1] = '\0';
        print ( & aux[0] );
      }
      print (wall); print ("\n");
    }
  }
}
    
void cleanMap(){
  for (int i = 0; i < 30; ++i) a[i] = ' ';
}

void printHeader() {
    
    print("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
    print("   > WALLS <");
    print("\n");
    
    if(life==3) print("     @ @ @\n");
    if(life==2) print("     @ @ _\n");
    if(life==1) print("     @ _ _\n");
}

   
void walls() {
  char input;

  cleanMap();
  a[1] = face[0];
  printHeader();
  printMap();
  print("\n\n");

  p = 1;
  
  while(input!='p'){
    cleanMap();
    input = readchar();
    print("\n\n");
    if (input == 'w') {p=p-10;}
    else if (input == 'a') {p--;}
    else if (input == 's') {p=p+10;}
    else if (input == 'd') {p++;}
    if( ! (p>=1 && (p<=9 || p>=11) && (p<=19 || p>=21) && p<=29) ) {
      if (input == 'w') {p=p+10;}
      else if (input == 'a') {p++;}
      else if (input == 's') {p=p-10;}
      else if (input == 'd') {p--;}
      
      life--;
    }
    
    a[p] = face[0];
    printHeader();
    printMap();
    print("\n\n");
    if(life==0) {
      print("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
      print("   GAME OVER!\n");
      print("\n\n");
      break;
    }
  }
}
