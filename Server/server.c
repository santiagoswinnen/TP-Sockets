
#include "server.h"

int serverfd;

int handleSockets() {

    struct sockaddr_in servaddr;

    serverfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero( &servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htons(INADDR_ANY);
    servaddr.sin_port = htons(22000);

    bind(serverfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    listen(serverfd, 10);

    while(1) {
        checkForNewClients();
    }

}

void checkForNewClients() {
    int newfd = accept(serverfd, (struct sockaddr*) NULL, NULL);
    if(newfd > 0) {
        if(fork() == 0) {
            attendClient(newfd);
        }
    }
}

void attendClient(int clientfd) {

    char buffer[BUFFERSIZE] = {0};
    char * databaseInfo;
    char * invalid = "invalid number";
    char * flightData;
    int flightNumber;


    while(1) {
        printf("Atendiendo\n");
        read(clientfd,buffer,BUFFERSIZE);
        flightNumber = checkFlightNumber(buffer);
        if(flightNumber == 0) {
            write(clientfd,invalid,BUFFERSIZE);
        } else {
            flightData = getFlightData(flightNumber);
            write(clientfd,flightData,BUFFERSIZE);
        }

    }
}

int checkFlightNumber(char * buffer) {

    int flightNumber;

    char * numPointer = buffer + strlen(buffer) - 1;
    while (isdigit((char)*(numPointer - 1))) {
        numPointer--;
    }

    flightNumber = atoi(numPointer);
    if(flightNumberIsValid(flightNumber)) {
        return flightNumber;
    }

    return 0;

}

int flightNumberIsValid(int number) {
    /*TODO*/
    return TRUE;
}

char * getFlightData(int flightNumber) {
    /*TODO*/

}




