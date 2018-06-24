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

int handleSockets();
void checkForNewClients();
void attendClient(const int * clientfd);
int existingFlightActions(int clientfd, int flightNumber, char * buffer);
int checkFlightNumber(char * buffer);
char * checkSeat(char * action, char * seat);
int seatNumberExpected(char * action);
char * cutAction(char * action);
void clearBuffer(char * buffer);
void terminateClientThread(int fd);
int flightNumberIsValid(int number);
char * getFlightData(int flightNumber);
char * newFlight();
char * cancelFlight(char * action);

int callbackStdout(void * param, int argc, char ** argv, char ** colName);
int callbackStdout(void * param, int argc, char ** argv, char ** colName);
int initializeTables();
int executeOperation(char * opStart, char * tableName, char * opEnd);
