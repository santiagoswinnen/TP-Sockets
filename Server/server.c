
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
    char * invalid = "invalid number";
    char * flightData;
    int flightNumber;
    ssize_t bytesRead;

    printf("Atendiendo\n");

    while(1) {
        clearBuffer(buffer);
        bytesRead = read(clientfd,buffer,BUFFERSIZE);
        if(bytesRead == -1) {
            printf("Error at reading operation\n");
        }
        if(strcmp(buffer,"new flight") == 0) {
            flightData = newFlight();
            write(clientfd,flightData,BUFFERSIZE);
        } else if (strncmp(buffer,"cancel flight",strlen("cancel flight")) == 0) {
            flightData = cancelFlight(buffer);
            write(clientfd,flightData,BUFFERSIZE);
        } else {
            flightNumber = checkFlightNumber(buffer);
            if(flightNumber == 0) {
                write(clientfd,invalid,BUFFERSIZE);
            } else {
                existingFlightActions(clientfd, flightNumber, buffer);
            }
        }
    }
}

void existingFlightActions(int clientfd, int flightNumber, char * buffer) {

    char * flightData;
    char * databaseResponseToSeat;
    char seat[BUFFERSIZE] = {0};
    ssize_t bytesRead;

    flightData = getFlightData(flightNumber);
    write(clientfd,flightData,BUFFERSIZE);
    if(seatNumberExpected(buffer)) {
        bytesRead = read(clientfd,seat,BUFFERSIZE);
        if(bytesRead == -1) { printf("Error at reading operation\n"); }
        databaseResponseToSeat = checkSeat(cutAction(buffer), seat);
        write(clientfd,databaseResponseToSeat,BUFFERSIZE);
        free(databaseResponseToSeat);
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
    memcpy(ret,"Seat successfully booked\0",strlen("Seat successfully booked\0"));
    return ret;
    /*el seat pasado como parametro se pasa con formato correcto (e.g. B23)
     * los valores van de la A a la G en mayuscula o minuscula y numero de 1 al 80
     * las acciones pueden ser "book" o "cancel"*/
}

char * newFlight() {
    char * ret = malloc(sizeof(char)*BUFFERSIZE);
    memcpy(ret,"New flight created\0",strlen("New flight created\0"));
    return ret;
    /*TODO*/
    /*crear un vuelo y devolver un mensaje de error o exito con el nuevo numero
     * del vuelo creado*/
}

char * cancelFlight(char * action) {
    char * ret = malloc(sizeof(char)*BUFFERSIZE);
    memcpy(ret,"Flight cancelled\0",strlen("Flight cancelled\0"));
    return ret;
    /*TODO*/
    /*action viene con la forma "cancel flight [numero]"
     * eliminarlo de la base de datos y retornar un string que diga
     * si hubo exito o no*/

}




