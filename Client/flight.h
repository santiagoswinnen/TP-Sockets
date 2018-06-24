#include <malloc.h>
#define ONTIME 1
#define SEATS_PER_ROW 9
#define ROWS 30
#define SEATS (SEATS_PER_ROW*ROWS)

typedef struct flightCDT {
    char seats [SEATS_PER_ROW][ROWS];
    int number;
    int status;
} flight;

typedef flight * Flight;


Flight newFlight(); /*Crea la estructura del vuelo*/
void showFlight(Flight flight); /*Muestra visualmente la ocupacion de los asientos*/
Flight flightFromString(const char * string); /* Arma la estructura flight en base al string del server*/
