#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <pthread.h>
#include "sqlite3.h"
#define BUFFERSIZE 64
#define TRUE 1
#define FALSE 0
#define ERRCHECK(err) {if (err!=NULL) {\
						printf("%s\n", err);\
						sqlite3_free(err);\
						return 0;}}
#define SEATS_PER_ROW 9
#define ROWS 30
#define SEATS (SEATS_PER_ROW*ROWS)

typedef int tableField;

int handleSockets();
void checkForNewClients();
void attendClient(const int * clientfd);
void existingFlightActions(int clientfd, int flightNumber, char * buffer);
int checkFlightNumber(char * buffer);
char * checkSeat(char * action, char * seat);
int seatNumberExpected(char * action);
char * cutAction(char * action);
void clearBuffer(char * buffer);
int flightNumberIsValid(int number);
char * getFlightData(int flightNumber);
char * newFlight();
char * cancelFlight(char * action);

int callbackOneTouple(void * param, int argc, char ** argv, char ** colName);
int callbackStdout(void * param, int argc, char ** argv, char ** colName);
char * SQLCommandToString(char * opStart, char * tableName, char * opEnd);
int initializeTables();
int executeOperation(char * opStart, char * tableName, char * opEnd);
int updateFlightSeats(int flightId, int row, int col, int newStatus);
int retreiveReservedFlights(char * flightNum, tableField ** matrix);
char * getFlightSeats(int flightId);
int executeOperationRetOneTouple(char * opStart, char * tableName,
    char * opEnd, char ** touple);

