#include <unistd.h>
#include <termios.h>
#include <stdio.h>
#include <sys/ioctl.h>


int screen_rows;
int screen_cols;

void refreshScreen() {
  // \x1b is the escape character (27)
  // [2J clears the screen
  // write(STDOUT_FILENO, "\x1b[2J", 4);
  
  // [H moves cursor to the top-left
  write(STDOUT_FILENO, "\x1b[H", 3);
}

void getWindowSize() {
  struct winsize ws;

  // ioctl(file_descriptor, request, address_of_struct)
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
    // Fallback if ioctl fails
    screen_rows = 24;
    screen_cols = 80;
  } else {
    screen_rows = ws.ws_row;
    screen_cols = ws.ws_col;
  }
}

void drawRows() {
  for (int y = 0; y < screen_rows; y++) {
    write(STDOUT_FILENO, "~\r\n", 3);
  }
}


int main() {
  struct termios raw, old;
  tcgetattr(0, &raw);
  old = raw;

  // Turn off ECHO and ICANON at the same time
  raw.c_lflag &= ~(ECHO | ICANON);

  tcsetattr(0, TCSAFLUSH, &raw);

  getWindowSize();
  drawRows();
  while (1) {
    refreshScreen();
    
    char c = '\0';
    if (read(STDIN_FILENO, &c, 1) == -1) break;
    
    if (c == 'q') break; // Our emergency exit
    printf("%c", c);
    fflush(stdout);
  }


  // char c;
  // // Now read() happens instantly!
  // while (read(0, &c, 1) == 1 && c != 'q') {
  //   printf("Read: %c\n", c);
  // }
  // tcsetattr(0, TCSAFLUSH, &old);


  raw.c_lflag |= (ECHO | ICANON);
  tcsetattr(0, TCSAFLUSH, &raw);
  return 0;
}
