#include <stdio.h>
#include <unistd.h>

int main(){
    char c;
    read(STDOUT_FILENO, &c, 1);
    printf("%c",c);
}