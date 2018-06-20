#include "flight.h"

static int flightCount = 1;

Flight newFlight() {
    Flight f = malloc(sizeof(flight));
    f->number = flightCount++;
    f->status = ONTIME;
    return f;
}

Flight flightFromString(const char * string) {
    int i, j, k;
    k = 0;

    Flight f = newFlight();

    for( i = 0; i < ROWS;  i++) {
        for ( j = 0; j < SEATS_PER_ROW; j++) {
            f->seats[i][j] = string[k++];
        }
    }

    return f;
}

void showFlight(Flight flight) {
    printf("Sorry, flight preview is under construction\n");
}