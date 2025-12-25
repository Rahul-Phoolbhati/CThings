
#include <unistd.h>
#include <termios.h>
#include <stdlib.h> // Required for atexit()

struct termios orig_termios;

void disableRawMode() {
  tcsetattr(0, TCSAFLUSH, &orig_termios);
}

void enableRawMode() {
  tcgetattr(0, &orig_termios);
  atexit(disableRawMode); // Tell C to run disableRawMode on exit

  struct termios raw = orig_termios;
  raw.c_lflag &= ~(ECHO | ICANON);

  tcsetattr(0, TCSAFLUSH, &raw);
}

int main() {
  enableRawMode();

  char c;
  while (read(0, &c, 1) == 1 && c != 'q') {
    // We'll process input here
    printf("Read : %c \n",c );
  }

  return 0;
}