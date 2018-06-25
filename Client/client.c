#include "client.h"

int openSocket() {

    int socketfd;
    struct sockaddr_in servaddr;

    socketfd = socket(AF_INET,SOCK_STREAM,0);
    bzero(&servaddr,sizeof servaddr);


    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(22000);

    if(inet_pton(AF_INET,"127.0.0.1",&(servaddr.sin_addr)) < 0) {
        printf("Invalid address\n");
        return -1;
    };

    if(connect(socketfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        printf("Connection failed\n");
        return -1;
    }

    return socketfd;
}

void startDialogue(char * buff, int socketfd) {

    char * parsedEntry;
    char * socketReception = malloc(sizeof(char)*BUFFERSIZE);
    ssize_t bytesWritten;

    parsedEntry = parseUserEntry(buff);
    if(parsedEntry != NULL) {
        bytesWritten = write(socketfd,parsedEntry,BUFFERSIZE);
        if(bytesWritten == -1) {
            printf("Error at writing operation\n>");
            return;
        }
        read(socketfd,socketReception,BUFFERSIZE);
        if(strcmp(socketReception,"invalid number") == 0) {
            printf("%s\n>", socketReception);
        } else {
            successfulServerResponse(parsedEntry, socketReception, socketfd);
        }
        clearBuffer(parsedEntry);
    } else {
        printf("Invalid action\n>");
    }

    free(socketReception);
}

void successfulServerResponse(char * parsedEntry, char * socketReception, int socketfd ) {

    char * answerFromClient;
    ssize_t bytesWritten;
    int resp;

    resp = displayServerResponse(parsedEntry,socketReception);
    if(resp == 0) {
        return;
    }
    if(secondEntryRequired(parsedEntry)) {
        answerFromClient = getSecondEntry();
        if(answerFromClient != NULL) {
            bytesWritten = write(socketfd,answerFromClient, BUFFERSIZE);
            if(bytesWritten == -1) {
                printf("Error at writing operation\n>");
                return;
            }
            read(socketfd,socketReception,BUFFERSIZE);
            displaySecondResponse(socketReception);
            clearBuffer(answerFromClient);
        }
    }
}

char * parseUserEntry(char * entry) {

    if(strcmp(entry,"check flight status") == 0 || strcmp(entry, "book flight") == 0
       || strcmp(entry,"cancel booking") == 0 || strcmp(entry,"cancel flight") == 0 ) {
        return parseFlightNumber(entry);
    } else if( strcmp(entry,"new flight") == 0) {
        return entry;
    }

    return NULL;
}


char * parseFlightNumber(char * entry) {

    char * param;

    printf("Enter flight number:\n>");
    if((param = readInputUntil('\n')) != NULL) {
        if(isNumericValue(param)) {
            return strcat(strcat(entry, " "),param);
        } else {
            printf("Invalid flight number\n>");
        }
    }

    return NULL;
}

int displayServerResponse(char * firstEntry, char * serverResponse) {

    if(strncmp(firstEntry,"check flight status", strlen("check flight status")) == 0
       || strncmp(firstEntry, "book flight",strlen("book flight")) == 0
       || strncmp(firstEntry,"cancel booking",strlen("cancel booking")) == 0) {

        if(strcmp("Flight has either been cancelled or does not exist",serverResponse) == 0) {
            printf("%s\n>", serverResponse);
            return 0;
        }

        Flight flight = flightFromString(serverResponse);
        showFlight(flight);

        if(strncmp(firstEntry,"book flight", strlen("book flight")) == 0) {
            printf("Select your seat please (e.g. B23)\n");
        } else if(strncmp(firstEntry,"cancel booking", strlen("cancel booking")) == 0) {
            printf("Select the seat you want to cancel please (e.g. B23)\n");
        }
        putchar('>');
    } else {
        printf("%s\n>", serverResponse);
    }

    return 1;
}

char * getSecondEntry() {

    char * ret;

    ret = readInputUntil('\n');
    if(isValidSeat(ret)) {
        return ret;
    }

    return NULL;
}


void displaySecondResponse(char * socketReception) {

    Flight modifiedFlight;

    if(socketReception[0] == '1' || socketReception[0] == '0') {
        modifiedFlight = flightFromString(socketReception);
        showFlight(modifiedFlight);
        printf("Your operation has been completed successfully\n>");
    } else {
        printf("%s\n", socketReception);
    }
}

char * readInputUntil(char limit) {

    char * ret = malloc(sizeof(char)*BUFFERSIZE);
    int c;
    int i = 0;

    while((c = getchar()) != limit && i < BUFFERSIZE) {
        ret[i++] = (char)c;
    }

    if(i == BUFFERSIZE) {
        printf("Maximum entry length reached\n>");
        return NULL;
    }

    ret[i] = 0;
    return ret;
}

int isNumericValue(const char * str) {

    int i = 0;

    while(str[i] != 0) {
        if(!isdigit(str[i++])) {
            return FALSE;
        }
    }
    return TRUE;
}

int isValidSeat(const char * str) {

    int i = 0;

    if(((str[i] >= 'A' && str[i] <= 'I') || (str[i] >= 'a' && str[i] <= 'i'))
       && isNumericValue(str + 1) && atoi(str + 1) <= ROWS) {

        return TRUE;
    }

    printf("Invalid seat\n>");
    return FALSE;
}

int secondEntryRequired(char * firstEntry) {

    if(strncmp(firstEntry, "book flight", strlen("book flight")) == 0
       || strncmp(firstEntry,"cancel booking", strlen("cancel booking")) == 0) {

        return TRUE;
    }
    return FALSE;
}

void clearBuffer(char * buffer) {
    int i;
    for(i = 0; i < BUFFERSIZE; i++) {
        buffer[i] = 0;
    }
}










