#include "server.h"


int serverfd;
sqlite3 * db;

typedef int tableField; // has as many strings as columns

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
    printf("Chequeando\n");
    if(newfd > 0) {
        pthread_t thread;
        printf("Por crear el thread\n");
        if(pthread_create(&thread, NULL, (void * (*)(void *))attendClient, &newfd) == -1) {
            printf("Error creating thread\n");
        }
    }
}

void attendClient(const int * clientFd) {

    char buffer[BUFFERSIZE] = {0};
    char * invalid = "invalid number";
    char * flightData;
    int flightNumber;
    int clientfd = *clientFd;
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
        } else if (strcmp(buffer,"exit") == 0) {
            return;
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
    return shortenedAction;
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

/* Prints SQL command output */
int callbackStdout(void * param, int argc, char ** argv, char ** colName) {
   int i;
   for(i = 0; i < argc; i++) {
      printf("%s = %s\n", colName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

// devuelve solo una tupla. Tiene que pasarse una array de char * con tantos
// char * como columnas tenga la tabla.
int callback(void * param, int argc, char ** argv, char ** colName) {
    char ** ret = (char **) param;
    int i;
    for(i = 0; i < argc; i++) {
        if(argv[i] == NULL)
            ret[i] = NULL;
        else {
            int len = strlen(argv[i]);
            ret[i] = malloc(len + 1);
            memcpy(ret[i], argv[i], len + 1);
        }
    }
    return 0;
}

int initializeTables() {
    int rc;
    char * cmd_result = NULL;
    char * flightTable = "CREATE TABLE flight(" \
                         "id INT PRIMARY KEY NOT NULL," \
                         "status INT NOT NULL );";
    rc = sqlite3_exec(db, flightTable, callbackStdout, NULL, &cmd_result);
    ERRCHECK(cmd_result);
    char * clientTable = "CREATE TABLE client(" \
                         "id INT PRIMARY KEY NOT NULL );";
    rc = sqlite3_exec(db, clientTable, callbackStdout, NULL, &cmd_result);
    ERRCHECK(cmd_result);
    char * reservTable = "CREATE TABLE reservation(" \
                         "client_id INT NOT NULL," \
                         "flight_id INT NOT NULL," \
                         "seat INT NOT NULL," \
                         "reserv_status INT DEFAULT 1," \
                         "FOREIGN KEY(client_id) REFERENCES client(id)," \
                         "FOREIGN KEY(flight_id) REFERENCES flight(id),"\
                         "PRIMARY KEY(client_id, flight_id, seat, reserv_status) );";
    rc = sqlite3_exec(db, reservTable, callbackStdout, NULL, &cmd_result);
    ERRCHECK(cmd_result);
    return rc == SQLITE_OK;
}

int executeOperation(char * opStart, char * tableName, char * opEnd) {
    int rc;
    char * cmd_result = NULL;
    int opStartLen = strlen(opStart);
    int nameLen = strlen(tableName);
    int opEndLen = strlen(opEnd);
    char * cmd = malloc(opStartLen + nameLen + opEndLen + 2);
    strcpy(cmd, opStart);
    strcpy(cmd + opStartLen, tableName);
    strcpy(cmd + opStartLen + nameLen, opEnd);
    cmd[opStartLen+nameLen+opEndLen+1] = ';';
    cmd[opStartLen+nameLen+opEndLen+2] = '\0';
    rc = sqlite3_exec(db, cmd, callback, NULL, &cmd_result);
    ERRCHECK(cmd_result);
    return rc == SQLITE_OK;
}

int retreiveReservedFlights(char * flightNum, tableField ** matrix) {
    sqlite3_stmt * res;
    int error = 0;
    int rec_count = 0;
    const char * tail;
    error = sqlite3_prepare_v2(db, "SELECT * FROM reservation",
    1000, &res, &tail);
    if (error != SQLITE_OK) {
        printf("An error occurred");
        return error;
    }
    while (sqlite3_step(res) == SQLITE_ROW) {
        matrix[rec_count][0] = sqlite3_column_int(res, 0);
        matrix[rec_count][1] = sqlite3_column_int(res, 1);
        matrix[rec_count][2] = sqlite3_column_int(res, 2);
        matrix[rec_count][3] = sqlite3_column_int(res, 3);
        rec_count++;
    }
    sqlite3_finalize(res);
    return error == SQLITE_OK;
}
