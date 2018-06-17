#include <malloc.h>

typedef struct flightCDT {
    char seats [7][80];
    int number;
} flight;

typedef flight * Flight;

typedef struct seatCDT {
    int row;
    char pos;
} seat;

typedef seat * Seat;

Flight newFlight(); /*Crea la estructura del vuelo*/
void deleteFlight(); /*Elimina un vuelo*/
Seat newSeat(int row, char pos); /*Crea un asiento*/
void deleteSeat(Seat seat); /*Elimina un asiento*/
void showFlight(Flight flight); /*Muestra visualmente la ocupacion de los asientos*/
