#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>

int screen_rows;
int screen_cols;

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


int main(){
    getWindowSize();
    printf("%d",screen_rows);
    printf("%d",screen_cols);
    return 0;
}