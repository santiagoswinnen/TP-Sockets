#include <stdio.h>
#include "client.h"

int main() {
    char buff[100] = "";
    int socketfd = openSocket();
    if(socketfd < 0) {
        printf("Exiting with error\n");
        return -1;
    }
    int c;
    int i = 0;
    while(1) {
        c = getchar();
        buff[i++] = (char)c;
        buff[i] = 0;
        if(c == '\n') {
            write(socketfd,buff,100);
            i = 0;
        }

    }
}