#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include "flight.h"

#define BUFFERSIZE 128
#define TRUE 1
#define FALSE 0

int openSocket();
char * parseUserEntry(char * entry);
char * parseFlightNumber(char * entry);
void displayServerResponse(char * originalEntry, char * serverResponse);
int secondEntryRequired(char * firstEntry);
char * getSecondEntry();
char * readInputUntil(char limit);
int isNumericValue(const char * str);
int isValidSeat(const char * str);