#include<stdio.h>
#include "includes/file_save_utility.h"

int saveToFile() {
    FILE *fp = fopen("output.txt", "w");
    if (fp == NULL) return -1;

    int last_line_with_data = -1;
    for (int i = 0; i < screen_rows; i++) {
        if (line_lengths[i] > 0) last_line_with_data = i;
    }

    for (int i = 0; i <= last_line_with_data; i++) {
        if (line_lengths[i] > 0) {
            if(fwrite(text_buffer[i], 1, line_lengths[i], fp)!=line_lengths[i]){
                return -1;
            }
        }
        if(fputc('\n', fp)==EOF){
            return -1;
        }
    }

    if(fclose(fp)!=0){
        return -1;
    }
    return 0;
}

