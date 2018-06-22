#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include "flight.h"

#define BUFFERSIZE 64
#define TRUE 1
#define FALSE 0

int openSocket();
void startDialogue(char * buff, int socketfd);
char * parseUserEntry(char * entry);
char * parseFlightNumber(char * entry);
void displayServerResponse(char * originalEntry, char * serverResponse);
int secondEntryRequired(char * firstEntry);
char * getSecondEntry();
char * readInputUntil(char limit);
int isNumericValue(const char * str);
int isValidSeat(const char * str);
void clearBuffer(char * buffer);