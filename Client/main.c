#include <stdio.h>
#include "client.h"

int main() {
    char buff[BUFFERSIZE] = "";
    int c;
    int i = 0;
    int socketfd = openSocket();

    if(socketfd < 0) {
        printf("Exiting with error\n");
        return -1;
    }

    while(1) {
        c = getchar();
        if(c == '\n') {
            startDialogue(buff, socketfd);
            clearBuffer(buff);
        } else {
            buff[i++] = (char)c;
            buff[i] = 0;
        }

    }
}