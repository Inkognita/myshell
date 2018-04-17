#include <fcntl.h>
#include <sys/types.h>
#include <stdio.h>
#include <bits/errno.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "stdio.h"

#define BUFSIZE 100

char dec_to_hex_selector(int in) {
    if (in <= 9) {
        return (char) (48 + in);
    } else {
        return (char) (55 + in);
    }
}

void to_hex(int in, char hex_ex[]) {
    char first;
    char second;
    first = dec_to_hex_selector(in % 16);
    second = dec_to_hex_selector((in / 16) % 16);
    hex_ex[3] = first;
    hex_ex[2] = second;
}

void readfile(char filename[], bool print_all) {
    char buffer[BUFSIZE];
    int length;


    int filedesc = open(filename, O_RDONLY);
    if (filedesc < 0) {
        perror("Unable to open");
        exit(1);
    }


    char hex_ex[5] = "\\xAB";
    while ((length = read(filedesc, buffer, BUFSIZE)) > 0)
        for (int buff_i = 0; buff_i < length; buff_i++) {
            if (!isprint(buffer[buff_i]) && print_all) {
                to_hex(buffer[buff_i], hex_ex);
                write(1, hex_ex, 4);
            } else { write(1, buffer + buff_i, 1); }
        }


    close(filedesc);
}

int main(int argc, char *argv[]) {
    bool print_all = false;
    for (int i = 1; i < argc; ++i) {
        if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0)) {
            printf("Usage: mycat [-h|--help] [-A] <file1> <file2> ... <fileN>\n"
                           "\t-h|--help \t for help\n"
                           "\t-A \t\t print even invisible symbols in hex format\n");
            return 0;
        } else if (strcmp(argv[i], "-A") == 0) {
            print_all = true;
        }
    }
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-A") != 0) {
            readfile(argv[i], print_all);
        }
    }
    return 0;
}

