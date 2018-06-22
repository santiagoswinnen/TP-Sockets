#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#define BUFFERSIZE 64
#define TRUE 1
#define FALSE 0

int handleSockets();
void checkForNewClients();
void attendClient(int clientfd);
int checkFlightNumber(char * buffer);
char * checkSeat(char * action, char * seat);
int seatNumberExpected(char * action);
char * cutAction(char * action);
void clearBuffer(char * buffer);
int flightNumberIsValid(int number);
char * getFlightData(int flightNumber);