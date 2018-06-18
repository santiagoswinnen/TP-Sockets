#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include "flight.h"
#include "user.h"
#define BUFFSIZE 128

int handleSockets();
void checkForNewClients();
void attendClient(int clientfd);
Seat * retrieveFreeSeats(); /*Devuelve una lista de punteros a seat con aquellos que estan libres*/
User * retrieveUsersWithBooking(); /*Devuelve lista de usuarios con reserva*/
User * retrieveUsersWithCancellation(); /*Devuelve lista de usuarios con cancelacion*/
