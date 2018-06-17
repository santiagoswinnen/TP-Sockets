#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

int openSocket(); /*Abre un socket y lo conecta al server*/
void checkFlightStatus();
void bookFlight();
void cancelBooking();
void newFlight();
void deleteFlight();