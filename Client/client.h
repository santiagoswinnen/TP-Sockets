#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include "flight.h"

#define BUFFERSIZE 512
#define TRUE 1
#define FALSE 0

int openSocket();
void startDialogue(char * buff, int socketfd);
void successfulServerResponse(char * parsedEntry, char * socketReception, int socketfd );
char * parseUserEntry(char * entry);
char * parseFlightNumber(char * entry);
void displayServerResponse(char * originalEntry, char * serverResponse);
int secondEntryRequired(char * firstEntry);
char * getSecondEntry();
void displaySecondResponse(char * socketReception);
char * readInputUntil(char limit);
int isNumericValue(const char * str);
int isValidSeat(const char * str);
void clearBuffer(char * buffer);