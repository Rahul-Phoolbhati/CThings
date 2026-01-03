#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>

struct termios orig_termios;
int screen_rows;
int screen_cols;

// 1. Restore the terminal to "Normal" mode
void disableRawMode() {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

// 2. Turn off Echo and Canonical mode
void enableRawMode() {
  tcgetattr(STDIN_FILENO, &orig_termios);
  atexit(disableRawMode);

  struct termios raw = orig_termios;
  // ICANON turns off "line-by-line" mode
  // ECHO turns off printing of typed characters
  raw.c_lflag &= ~(ECHO | ICANON);

  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

// 3. Ask the terminal how big the window is
void getWindowSize() {
  struct winsize ws;
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
    screen_rows = 24; // Fallback defaults
    screen_cols = 80;
  } else {
    screen_rows = ws.ws_row;
    screen_cols = ws.ws_col;
  }
}

// 4. Draw the UI and clear screen
void refreshScreen() {
  // \x1b[2J = Clear screen
  write(STDOUT_FILENO, "\x1b[2J", 4);
  // \x1b[H = Move cursor to top-left
  write(STDOUT_FILENO, "\x1b[H", 3);

  // Draw tildes down the left side
  for (int y = 0; y < screen_rows; y++) {
    write(STDOUT_FILENO, "~\r\n", 3);
  }

  // Move cursor back to top-left after drawing
  write(STDOUT_FILENO, "\x1b[H", 3);
}

int main() {
  enableRawMode();
  getWindowSize();

  while (1) {
    refreshScreen();

    char c = '\0';
    // Read 1 byte from keyboard
    if (read(STDIN_FILENO, &c, 1) == -1) break;

    // Quit if 'q' is pressed
    if (c == 'q') {
      // Clear screen one last time before exiting
      write(STDOUT_FILENO, "\x1b[2J", 4);
      write(STDOUT_FILENO, "\x1b[H", 3);
      break;
    }
  }

  return 0;
}