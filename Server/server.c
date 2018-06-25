#include "server.h"


int serverfd;
sqlite3 * db;
int flight_num = 1;

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
        pthread_t thread;
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
    int retVal;
    ssize_t bytesRead;

    while(1) {
        clearBuffer(buffer);
        bytesRead = read(clientfd,buffer,BUFFERSIZE);
        if(bytesRead == -1) {
            printf("Error at reading operation\n");
            terminateClientThread(clientfd);
        }
        if(strcmp(buffer,"new flight") == 0) {
            flightData = newFlight();
            write(clientfd,flightData,BUFFERSIZE);
        } else if (strcmp(buffer,"exit") == 0) {
            terminateClientThread(clientfd);
        } else if (strncmp(buffer,"cancel flight",strlen("cancel flight")) == 0) {
            flightData = cancelFlight(buffer);
            write(clientfd,flightData,BUFFERSIZE);
        } else {
            flightNumber = checkFlightNumber(buffer);
            if(flightNumber == 0) {
                write(clientfd,invalid,BUFFERSIZE);
            } else {
                retVal = existingFlightActions(clientfd, flightNumber, buffer);
                if(retVal == -1) { terminateClientThread(clientfd); }
            }
        }
    }
}



int existingFlightActions(int clientfd, int flightNumber, char * buffer) {

    char * flightData;
    char * databaseResponseToSeat;
    char seat[BUFFERSIZE] = {0};
    ssize_t bytesRead;

    if(flightIsCanceled(flightNumber)) {
        return 1;
    }
    flightData = getFlightData(flightNumber);
    write(clientfd,flightData,BUFFERSIZE);
    if(seatNumberExpected(buffer)) {
        bytesRead = read(clientfd,seat,BUFFERSIZE);
        if(bytesRead == -1) {
            printf("Error at reading operation\n");
            return -1;
        }

        databaseResponseToSeat = checkSeat(cutAction(buffer), seat,flightNumber, clientfd);
        write(clientfd,databaseResponseToSeat,BUFFERSIZE);
        free(databaseResponseToSeat);
    }

    return 1;
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

void terminateClientThread(int fd) {
    close(fd);
    pthread_exit(NULL);
}
//Checks if sql statement returns valid tuple
int isValidSQL(char * selectStatement) {

    sqlite3_stmt * res;
    int error;
    const char * tail;
    error = sqlite3_prepare_v2(db, selectStatement, 1000, &res, &tail);
    printf("DENTRO DE IS VALID SQL, RESULTADO DE LA CONSULTA FUE: %s \n",error == SQLITE_OK ? "bien" : "mal");
    if (error == SQLITE_OK) {
        if(sqlite3_step(res) == SQLITE_ROW) {
            return 1;
            printf("DEVOLVIO TUPLA VALIDA \n");
        }
        printf("NO DEVOLVIO TUPLA VALIDA \n");
    }
    printf("NOT VALID SQL \n");
    return 0;
}

int flightNumberIsValid(int number) {

    printf("VALIDANDO FLIGHT NUMBER, SE PASO %d POR PARAMETRO \n",number);
    char sql[100];
    sprintf(sql,"SELECT * FROM flight WHERE id = %d;",number);
    return isValidSQL(sql);
}

char * getFlightData(int flightNumber) {
    return getFlightSeats(flightNumber);
}

int flightIsCanceled(int flightNumber) {
    sqlite3_stmt * res;
    int error;
    int status;
    const char * tail;
    char sql[100];
    sprintf(sql,"SELECT status FROM flight WHERE id = %d;",flightNumber);
    error = sqlite3_prepare_v2(db, sql, 1000, &res, &tail);
    if (error == SQLITE_OK) {
        if(sqlite3_step(res) == SQLITE_ROW) {
            status = sqlite3_column_int(res, 0);
            printf("STATUS IS %d \n",status);
        }
    }

    sqlite3_finalize(res);
    return status == 0;
}

char * checkSeat(char * action, char * seat, int flightNumber, int clientid) {
    int seatNum;
    int row,col;
    char letter;
    char sql[200];
    char * ptr;
    char * seatData;
    char * ret = malloc(sizeof(char)*BUFFERSIZE);
    char * num = malloc(sizeof(char) * 2);
    if(flightNumberIsValid(flightNumber)) {

            seatData = getFlightSeats(flightNumber);
            ptr = seat + 1;
            strcpy(num,ptr);
            row = atoi(num);
            row -= 1;
            letter = *seat;
            col = letterToInt(letter);
            seatNum = (SEATS_PER_ROW * row) + col;

            if(strcmp(action,"book") == 0) {
                if(seatData[seatNum] == '1') {
                    strcpy(ret,"The seat you wish to book is already occupied.");
                    ret[strlen("The seat you wish to book is already occupied.")] = 0;
                }
                else {
                    sprintf(sql,"SELECT * FROM reservation WHERE flight_id = %d AND client_id = %d AND seatRow = %d AND seatCol = %d;",flightNumber,clientid,row,col);
                    updateFlightSeats(flightNumber,row,col,'1');
                    if(isValidSQL(sql)) {
                        updateReservation(clientid,flightNumber,row,col,1);
                    }
                    else {
                        insertReservation(clientid,flightNumber,row,col,1);
                    }
                    return getFlightSeats(flightNumber);
                }
                char * temp = getFlightSeats(flightNumber);
                strcpy(ret,temp);
                free(temp);
                ret[SEATS] = 0;
            }
            else {
                sprintf(sql,"SELECT * FROM reservation WHERE flight_id = %d AND client_id = %d AND seatRow = %d AND seatCol = %d;",flightNumber,clientid,row,col);
                updateFlightSeats(flightNumber,row,col,'0');
                if(isValidSQL(sql)) {
                    updateReservation(clientid,flightNumber,row,col,0);
                }
                else {
                    sprintf(sql,"SELECT * FROM reservation WHERE flight_id = %d AND client_id = %d AND seatRow = %d AND seatCol = %d;",flightNumber,clientid,row,col);
                    updateFlightSeats(flightNumber,row,col,'0');
                    if(isValidSQL(sql)) {
                        updateReservation(clientid,flightNumber,row,col,0);
                    }
                    else {
                        insertReservation(clientid,flightNumber,row,col,0);
                    }
                    return getFlightSeats(flightNumber);
                }
            }   
    }
    else  {
        strcpy(ret,"Invalid flight number");
        ret[strlen("Invalid flight number")] = 0;
    }

    /*el seat pasado como parametro se pasa con formato correcto (e.g. B23)
     * los valores van de la A a la G en mayuscula o minuscula y numero de 1 al 80
     * las acciones pueden ser "book" o "cancel"*/
    return ret;
}

int letterToInt(char c) {
    switch(toupper(c)) {
        case 'A':return 0;
        case 'B': return 1;
        case 'C': return 2;
        case 'D': return 3;
        case 'E': return 4;
        case 'F': return 5;
        case 'G': return 6;
        case 'H':return 7;
        case 'I': return 8;
        default: return 0;
    }
}

char * newFlight() {

    char * ret = malloc(sizeof(char)*BUFFERSIZE);
    char sql[100];
    char * result;
    char seats[270];
    char str[100];
    fillSeats(seats);
    int error;
    sprintf(sql,"INSERT INTO flight (id,status,seats) VALUES (%d,1,'%s');",flight_num,seats);
    if( (error = sqlite3_exec(db,sql,NULL,NULL,&result)) != SQLITE_OK) {
        strcpy(result,"Error creating flight");
    }
    sprintf(str,"New Flight created: flight %d",flight_num++);
    strcpy(ret,str);

    return ret;
}

char * cancelFlight(char * action) {
    int error;
    int flight_number;
    char * ret = malloc(sizeof(char)*BUFFERSIZE);
    char sql[100];
    char * p = action;
    while (*p) { 
        if (isdigit(*p)) { 
            flight_number = (int)strtol(p, &p, 10); 
        } else { 
            p++;
        }
    }

    if(flightNumberIsValid(flight_number)) {
        sprintf(sql,"UPDATE flight SET status = 0 WHERE id = %d;",flight_number);
        if( (error = sqlite3_exec(db,sql,NULL,NULL,NULL)) == SQLITE_OK) {
            strcpy(ret,"Flight has been canceled");
            ret[strlen("Flight has been canceled")] = 0;
        }
        else {
            strcpy(ret,"Error deleting flight");
            ret[strlen("Error deleting flight")] = 0;
        }
        
    }
    else {
        strcpy(ret,"Invalid flight number");
        ret[strlen("Invalid flight number")] = 0;
    }

    return ret;
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
int callbackOneTouple(void * param, int argc, char ** argv, char ** colName) {
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

int callbackOneField(void * param, int argc, char ** argv, char ** colName) {
    char * ret = (char *) param;
    int i;
    for(i = 0; i < argc; i++) {
        if(argv[i] != NULL) {
            int len = strlen(argv[i]);
            memcpy(ret, argv[i], len + 1);
        }
    }
    return 0;
}

int initializeTables() {
    int rc;
    char * cmd_result = NULL;
    char * flightTable = "CREATE TABLE flight(" \
                         "id NOT NULL," \
                         "status INT NOT NULL, " \
                         "seats TEXT NOT NULL, " \
                         "PRIMARY KEY(id));";
    rc = sqlite3_exec(db, flightTable, callbackStdout, NULL, &cmd_result);
    ERRCHECK(cmd_result);
    char * reservTable = "CREATE TABLE reservation(" \
                         "client_id INT NOT NULL," \
                         "flight_id INT NOT NULL," \
                         "seatRow INT NOT NULL," \
                         "seatCol INT NOT NULL," \
                         "status INT DEFAULT 1," \
                         "FOREIGN KEY(flight_id) REFERENCES flight(id),"\
                         "PRIMARY KEY(client_id, flight_id, seatRow, seatCol, status) );";


    rc = sqlite3_exec(db, reservTable, callbackStdout, NULL, &cmd_result);
    ERRCHECK(cmd_result);
    return rc == SQLITE_OK;
}

int executeOperation(char * opStart, char * tableName, char * opEnd) {
    int rc;
    char * cmd_result = NULL;
    char * cmd = SQLCommandToString(opStart, tableName, opEnd);
    rc = sqlite3_exec(db, cmd, callbackStdout, NULL, &cmd_result);
    ERRCHECK(cmd_result);
    free(cmd);
    return rc == SQLITE_OK;
}

char * SQLCommandToString(char * opStart, char * tableName, char * opEnd) {
    int opStartLen = strlen(opStart);
    int nameLen = strlen(tableName);
    int opEndLen = strlen(opEnd);
    char * cmd = malloc(opStartLen + nameLen + opEndLen + 2);
    strcpy(cmd, opStart);
    strcpy(cmd + opStartLen, tableName);
    strcpy(cmd + opStartLen + nameLen, opEnd);
    cmd[opStartLen+nameLen+opEndLen+1] = ';';
    cmd[opStartLen+nameLen+opEndLen+2] = '\0';
    return cmd;
}

// touple should have enough mem for n char *s, where n is the number of columns
int executeOperationRetOneTouple(char * opStart, char * tableName,
    char * opEnd, char ** touple) {

    int rc;
    char * cmd_result = NULL;
    char * cmd = SQLCommandToString(opStart, tableName, opEnd);
    rc = sqlite3_exec(db, cmd, callbackOneTouple, (void *)touple, &cmd_result);
    ERRCHECK(cmd_result);
    free(cmd);
    return rc == SQLITE_OK;
}

char * getFlightSeats(int flightId) {
    int rc;
    char * cmd_result = NULL;
    char * id = malloc(12);
    sprintf(id, "%d", flightId);
    char * cmd = SQLCommandToString("SELECT seats FROM flight WHERE id = ",
        "", id);
    char * seats = calloc(1, SEATS + 1);
    rc = sqlite3_exec(db, cmd, callbackOneField, seats, &cmd_result);
    ERRCHECK(cmd_result);
    free(id);
    free(cmd);
    if(seats[0] == '\0' || rc != SQLITE_OK)
        return NULL;
    return seats;
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

int updateFlightSeats(int flightId, int row, int col, int newStatus) {
    int rc;
    char * cmd_result = NULL;
    char * opStart = "UPDATE flight SET seats = '";
    char * opEnd1 = "' WHERE id = ";
    char * opEnd = malloc(strlen(opEnd1) + 12);
    sprintf(opEnd, "%s%d", opEnd1, flightId);
    char * seats = getFlightSeats(flightId);
    seats[row * SEATS_PER_ROW + col] = newStatus;
    char * cmd = SQLCommandToString(opStart, seats, opEnd);
    rc = sqlite3_exec(db, cmd, callbackStdout, NULL, &cmd_result);
    ERRCHECK(cmd_result);
    return rc;
}

int updateFlightStatus(int status, int flight_id) {
    char select_sql[100];
    char sql[200];
    sqlite3_stmt * res;
    int error;
    char * result;
    const char * tail;
    sprintf(select_sql,"SELECT * FROM flight WHERE id = %d;",flight_id);
    error = sqlite3_prepare_v2(db, select_sql, 1000, &res, &tail);
    if (error != SQLITE_OK) {
        printf("Error in database, try again later...\n ");
        return error;
    }
    if(sqlite3_step(res) == SQLITE_ROW) {
        sprintf(sql,"UPDATE flight SET status = %d WHERE id = %d;",status,flight_id);
        if( (error = sqlite3_exec(db,sql,NULL,NULL,&result)) != SQLITE_OK) {
            printf("Error updating flight status");
        }
    }
    else {
        printf("No flight was found");
    }

    return error;
}

int insertReservation(int clientid, int flightid, int seatrow, int seatcol, int status) {

    char sql[200];
    char * result;
    int error;
    sprintf(sql,"INSERT INTO reservation (client_id,flight_id,seatRow,seatCol,status) VALUES (%d,%d,%d,%d,%d);",clientid,flightid,seatrow,seatcol,status);
    if( (error = sqlite3_exec(db,sql,NULL,NULL,&result)) != SQLITE_OK) {
        printf("Error creating reservation");
    }
    return error;
}

int updateReservation(int clientid, int flightid, int seatrow, int seatcol, int status) {

    char select_sql[100];
    char sql[200];
    sqlite3_stmt * res;
    int error;
    char * result;
    const char * tail;
    sprintf(select_sql,"SELECT * FROM reservation WHERE flight_id = %d AND client_id = %d AND seatRow = %d AND seatCol = %d;",flightid,clientid,seatrow,seatcol);
    error = sqlite3_prepare_v2(db, select_sql, 1000, &res, &tail);
    if (error != SQLITE_OK) {
        printf("Error in database, try again later...\n ");
        return error;
    }
    if(sqlite3_step(res) == SQLITE_ROW) {
        sprintf(sql,"UPDATE reservation SET status = %d WHERE client_id = %d AND flight_id = %d AND seatRow = %d AND seatCol = %d;",status,clientid,flightid,seatrow,seatcol);
        if( (error = sqlite3_exec(db,sql,NULL,NULL,&result)) != SQLITE_OK) {
            printf("Error updating reservation");
        }
    }
    else {
        printf("No reservation was found. \n");
    }
    sqlite3_finalize(res);
    return error == SQLITE_OK;
}

int checkFlightStatus(int flightid) {
    char sql[100];
    sqlite3_stmt * res;
    int error = 0;
    int status;
    const char * tail;
    sprintf(sql,"SELECT status FROM flight WHERE id = %d;",flightid);
    error = sqlite3_prepare_v2(db, sql, 1000, &res, &tail);
    if (error != SQLITE_OK) {
        printf("Error in database, try again later...\n ");
        return error;
    }
    if(sqlite3_step(res) == SQLITE_ROW) {
        status = sqlite3_column_int(res, 1);
        printf("Flight %d status is: %s \n",flightid,status?"available":"canceled");
    }
    else {
        printf("No flight was found with that flight number");
    }
    sqlite3_finalize(res);
    return error == SQLITE_OK;
}


void fillSeats(char * seats) {
    int i;
    for(i=0 ; i<SEAT_QTY ; i++ ) {
        seats[i] = '0';
    }
}


