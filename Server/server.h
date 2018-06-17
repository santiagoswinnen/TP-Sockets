#include <sys/socket.h>
#include <arpa/inet.h>
#include "flight.h"
#include "user.h"

int handleSockets();
Seat * retrieveFreeSeats(); /*Devuelve una lista de punteros a seat con aquellos que estan libres*/
User * retrieveUsersWithBooking(); /*Devuelve lista de usuarios con reserva*/
User * retrieveUsersWithCancellation(); /*Devuelve lista de usuarios con cancelacion*/
