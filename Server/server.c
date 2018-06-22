
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
    char seat[BUFFERSIZE] = {0};
    char * invalid = "invalid number";
    char * flightData;
    char * databaseResponseToSeat;
    int flightNumber;

    while(1) {
        printf("Antes del read\n");
        read(clientfd,buffer,BUFFERSIZE);

        flightNumber = checkFlightNumber(buffer);

        if(flightNumber == 0) {
            printf("No entre al else\n");
            write(clientfd,invalid,BUFFERSIZE);
        } else {
            printf("Entre al else\n");
            flightData = getFlightData(flightNumber);
            printf("Por escribir flight data: %s\n",flightData);
            write(clientfd,flightData,BUFFERSIZE);
            if(seatNumberExpected(buffer)) {
                printf("Espera seat number\n");
                clearBuffer(buffer);
                read(clientfd,seat,BUFFERSIZE);
                databaseResponseToSeat = checkSeat(cutAction(buffer), seat);
                write(clientfd,databaseResponseToSeat,BUFFERSIZE);
                free(databaseResponseToSeat);
            }
        }

        clearBuffer(seat);
        clearBuffer(buffer);
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



int seatNumberExpected(char * action) {

    if(strncmp(action, "book flight", strlen("book flight")) == 0
       || strncmp(action,"cancel booking", strlen("cancel booking")) == 0) {

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

char * cutAction(char * action) {

    char * shortenedAction = malloc(sizeof(char)*BUFFERSIZE);

    if(strncmp(action,"book",strlen("book")) == 0) {
        memcpy(shortenedAction,"book",strlen("book"));
        shortenedAction[strlen("book")] = 0;
    } else if (strncmp(action,"cancel",strlen("cancel")) == 0) {
        memcpy(shortenedAction,"cancel",strlen("cancel"));
        shortenedAction[strlen("cancel")] = 0;
    }
}

int flightNumberIsValid(int number) {
    /*TODO*/
    return TRUE;
}

char * getFlightData(int flightNumber) {
    /*TODO*/
    char * flightData = malloc(sizeof(char)*BUFFERSIZE);
    memcpy(flightData,"This is flight data", strlen("This is flight data")+1);
    flightData[strlen("This is flight data")] = 0;
    return flightData;
}

char * checkSeat(char * action, char * seat) {
    /*TODO*/
    char * ret = malloc(sizeof(char)*BUFFERSIZE);
    memcpy(ret,"Database reached successfully",strlen("Database reached successfully"));
    return ret;
    /*el seat pasado como parametro se pasa con formato correcto (e.g. B23)
     * los valores van de la A a la G en mayuscula o minuscula y numero de 1 al 80
     * las acciones pueden ser "book" o "cancel"*/
}




