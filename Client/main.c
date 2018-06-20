#include <stdio.h>
#include <stdlib.h>
#include "client.h"

int main() {
    char buff[100] = "";
    char * parsedEntry;
    int c;
    int i = 0;
    char * socketReception = malloc(sizeof(char)*BUFFERSIZE);
    char * answerFromClient;
    int socketfd = openSocket();

    if(socketfd < 0) {
        printf("Exiting with error\n");
        return -1;
    }

    while(1) {
        c = getchar();
        buff[i++] = (char)c;
        buff[i] = 0;
        if(c == '\n') {
            if((parsedEntry = parseUserEntry(buff)) != NULL) {
                write(socketfd,parsedEntry,100);
                read(socketfd,socketReception,BUFFERSIZE);
                displayServerResponse(parsedEntry,socketReception);
                if(secondEntryRequired(parsedEntry)) {
                    answerFromClient = getSecondEntry();
                    if(answerFromClient != NULL) {
                        write(socketfd,answerFromClient, BUFFERSIZE);
                        read(socketfd,socketReception,BUFFERSIZE);
                        printf("%s\n", socketReception);
                    }
                }
            }
        }
    }
}