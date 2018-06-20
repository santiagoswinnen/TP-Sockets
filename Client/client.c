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
    char * answerFromClient;

    parsedEntry = parseUserEntry(buff);
    if(parsedEntry != NULL) {
        printf("Writing: %s\n", parsedEntry);
        write(socketfd,parsedEntry,100);
        printf("After Writing\n");
        read(socketfd,socketReception,BUFFERSIZE);
        printf("After Writing\n");

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

char * parseUserEntry(char * entry) {

    if(strcmp(entry,"check flight status") == 0 || strcmp(entry, "book flight\0") == 0
       || strcmp(entry,"cancel booking") == 0 || strcmp(entry,"cancel flight") == 0 ) {

        return parseFlightNumber(entry);
    } else if( strcmp(entry,"new flight") == 0) {
        printf("Returning entry\n");
        return entry;
    }

    return NULL;
}


char * parseFlightNumber(char * entry) {

    char * param;

    printf("Enter flight number:\n");
    if((param = readInputUntil('\n')) != NULL) {
        if(isNumericValue(param)) {
            return strcat(strcat(entry, " "),param);
        } else {
            printf("Invalid flight number\n");
        }
    }

    return NULL;
}

void displayServerResponse(char * firstEntry, char * serverResponse) {

    if(strcmp(firstEntry,"check flight status") == 0
       || strcmp(firstEntry, "book flight") == 0
       || strcmp(firstEntry,"cancel booking") == 0) {

        Flight flight = flightFromString(serverResponse);
        showFlight(flight);

        if(strcmp(firstEntry,"book flight") == 0) {
            printf("Select your seat please (e.g. B23)\n");
        } else if(strcmp(firstEntry,"cancel booking") == 0) {
            printf("Select the seat you want to cancel please (e.g. B23)\n");
        }
    } else {
        printf("%s\n", serverResponse);
    }
}

char * getSecondEntry() {

    char * ret;

    ret = readInputUntil('\n');
    if(isValidSeat(ret)) {
        return ret;
    }

    return NULL;
}

char * readInputUntil(char limit) {

    char * ret = malloc(sizeof(char)*BUFFERSIZE);
    int c;
    int i = 0;

    while((c = getchar()) != limit && i < BUFFERSIZE) {
        ret[i++] = (char)c;
    }

    if(i == BUFFERSIZE) {
        printf("Maximum entry length reached\n");
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

    if(((str[i] >= 'A' && str[i] <= 'G') || (str[i] >= 'a' && str[i] <= 'g'))
       && isNumericValue(str + 1) && atoi(str) <= 80) {

        return TRUE;
    }

    printf("Invalid seat\n");
    return FALSE;
}

int secondEntryRequired(char * firstEntry) {

    if(strcmp(firstEntry, "book flight") == 0 || strcmp(firstEntry,"cancel booking") == 0) {
        return TRUE;
    }

    return FALSE;
}

