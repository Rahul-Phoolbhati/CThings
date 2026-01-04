#include <unistd.h>
#include <termios.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdlib.h>
#include "includes/file_save_utility.h"


enum arrow_key {
  ARROW_LEFT = 1000,
  ARROW_RIGHT,
  ARROW_UP,
  ARROW_DOWN
};
enum command_mode{
  EXIT = 9999,
  SAVETOFILE = 9998,
};

int cx = 0, cy = 0;
int screen_rows;
int screen_cols;

struct aBuf {
  char *b;
  int len;
};

char line[80];
int line_len = 0;

#define ABUF_INIT {NULL,0}
char** text_buffer;
int* line_lengths;

void abAppend(struct aBuf *ab, const char *b, int len){
  char *new = realloc(ab->b, ab->len+len);

  if(new == NULL) return;

  memcpy(&new[ab->len], b, len); //append
  ab->b = new;
  ab->len += len;
}

void abFree(struct aBuf *ab) {
  free(ab->b);
  ab->len =0;
}
void drawRows(struct aBuf *ab) {
  for (int y = 0; y < screen_rows-1; y++) {
    if(line_lengths[y]>0){
      abAppend(ab, text_buffer[y], line_lengths[y]);
    }else{
      abAppend(ab, "~", 1);
    }
    
    
    // if (y == 0) {
    //   // On the first line, draw our "line" buffer
    //   abAppend(ab, line, line_len);
    // } else {
    //   // On all other lines, draw the tilde
    //   abAppend(ab, "~", 1);
    // }         // print ~
    if (y < screen_rows - 1)       // NOT the last row?
      abAppend(ab, "\r\n", 2);     // then go to next line
  }
}

void refreshScreen() {
  struct aBuf ab = ABUF_INIT;
  // \x1b is the escape character (27)
  // [2J clears the screen
  abAppend(&ab, "\x1b[2J", 4);
  // abAppend(&ab, "\x1b[3J", 4);
    
  abAppend(&ab, "\x1b[H", 3);

  drawRows(&ab);
  // [H moves cursor to the top-left
  abAppend(&ab, "\x1b[H", 3);
  char buf[32];
  snprintf(buf, sizeof(buf), "\x1b[%d;%dH", cy + 1, cx + 1);
  abAppend(&ab, buf, strlen(buf));
  write(STDOUT_FILENO, ab.b, ab.len);
  abFree(&ab);
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


int readChar(){
  char c = '\0';
  while(read(STDIN_FILENO, &c, 1) != 1);

  if(c == '\x1b'){
    char seq[3];
    if(read(STDIN_FILENO, &seq[0], 1)!=1) return c;
    if(read(STDIN_FILENO, &seq[1], 1)!=1) return c;

    if (seq[0] == '[') {
      switch (seq[1]) {
        case 'A': return ARROW_UP; // Let's map Up to 'w' for now
        case 'B': return ARROW_DOWN; // Down to 's'
        case 'C': return ARROW_RIGHT; // Right to 'd'
        case 'D': return ARROW_LEFT; // Left to 'a'
      }
    }
    else if (seq[0] == ':'){
      switch (seq[1]) {
        case 'q': return EXIT; 
        case 'w': return SAVETOFILE;
      }
    }
  }

  return c;
}

void handle_arrow_key(int key){
  switch(key){
    case ARROW_UP:
      if(cy>0) {
        cy--;
        if(cx > line_lengths[cy]){
          cx = line_lengths[cy];
        }
      }
      return;
    case ARROW_RIGHT: 
      if(cx < (screen_cols-1)) cx++;
      return;
    case ARROW_DOWN:
      if(cy < (screen_rows-1)) {
        cy++;
        if(cx>line_lengths[cy]){
          cx = line_lengths[cy];
        }
      }
      return;
    case ARROW_LEFT:
      if(cx>0) cx--;
      return;
  }
}

void handle_backspace(){
  if(cx > 0){
    if (cx <= line_lengths[cy]) {
        for (int i = cx - 1; i < line_lengths[cy] - 1; i++) {
            text_buffer[cy][i] = text_buffer[cy][i + 1];
        }
        line_lengths[cy]--;
    }
    cx--;
  }
  // if (line_len > 0) { line_len--; cx--; }
}

int main() {
  struct termios raw, old;
  tcgetattr(0, &raw);
  old = raw;

  // Turn off ECHO and ICANON at the same time
  raw.c_lflag &= ~(ECHO | ICANON);

  tcsetattr(0, TCSAFLUSH, &raw);

  getWindowSize();

  text_buffer = malloc(sizeof(char*)*screen_rows);
  line_lengths = malloc(sizeof(int)*screen_rows);

  for(int i =0;i<screen_rows; i++){
    text_buffer[i] = malloc(screen_cols);
    line_lengths[i] = 0;    
  }

  write(STDOUT_FILENO, "\x1b[?1049h", 8);
  
  while (1) {
    refreshScreen();
    int c = readChar();
    if (c == EXIT) break;
    if(c == SAVETOFILE) {
        int err = saveToFile();
    }
    switch (c)
    {
        case ARROW_LEFT:
        case ARROW_RIGHT:
        case ARROW_UP:
        case ARROW_DOWN:
            handle_arrow_key(c);
            break;

        case 127:
            // write(STDOUT_FILENO, " - pressed backspace - ", 23);
            handle_backspace();
            break;
        
        case '\r':
          line_len = 0; cx = 0;
          break;

        default:
            if (c >= 32 && c <= 126) {

              if(line_lengths[cy] < screen_cols - 1){
                for(int i= line_lengths[cy]; i>cx;i--){
                  text_buffer[cy][i] = text_buffer[cy][i-1];
                }
                text_buffer[cy][cx] = c;
                line_lengths[cy]++;
                cx++;
              }
              // if (line_len < 80) { 
              //     line[line_len] = c;
              //     line_len++;
              //     cx++;
              // }
            }
            // handle_other(key);
            break;
    }


  }


  // char c;
  // // Now read() happens instantly!
  // while (read(0, &c, 1) == 1 && c != 'q') {
  //   printf("Read: %c\n", c);
  // }
  write(STDOUT_FILENO, "\x1b[?1049l", 8);
  tcsetattr(0, TCSAFLUSH, &old);


  // raw.c_lflag |= (ECHO | ICANON);
  // tcsetattr(0, TCSAFLUSH, &raw);
  return 0;
}
