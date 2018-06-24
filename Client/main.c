#include <stdio.h>
#include <signal.h>
#include "client.h"

int socketfd;
int ended = 0;

void sigintHandler(int sig_num) {
    signal(SIGINT, sigintHandler);
    write(socketfd,"exit\0",BUFFERSIZE);
    close(socketfd);
    putchar('\n');
    exit(1);
}

int main() {

    signal(SIGINT, sigintHandler);

    char buff[BUFFERSIZE] = "";
    int c;
    int i = 0;
    socketfd = openSocket();

    if(socketfd < 0) {
        printf("Exiting with error\n");
        return -1;
    }

    printf("Welcome! Available actions: "
                   "\n\t-> book flight "
                   "\n\t-> cancel booking"
                   "\n\t-> check flight status"
                   "\n\t-> new flight"
                   "\n\t-> cancel flight"
                   "\n\t-> exit\n>");
    while(!ended) {
        c = getchar();
        if(c == '\n') {
            if(strcmp(buff,"exit") == 0) {
                write(socketfd,buff,BUFFERSIZE);
                close(socketfd);
                return 1;
            }
            startDialogue(buff, socketfd);
            clearBuffer(buff);
            i = 0;
        } else {
            buff[i++] = (char)c;
            buff[i] = 0;
        }

    }

    return 1;
}

