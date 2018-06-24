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
    int i, j;
    char * str = malloc(sizeof(char)*SEATS);
    for(i = 0; i < SEATS; i ++) {
        if(i%2) {
            str[i] = '0';
        } else {
            str[i] = '1';
        }
    }
    flight = flightFromString(str);
    printf("Sorry, flight preview is under construction\n");
    printf("        ___\n");
    printf("       /   \\\n");
    printf("      /     \\\n");
    printf("     /       \\\n");
    printf("    /         \\\n");
    printf("   /           \\\n");
    for(i = 0; i < ROWS; i++) {
        if(i < 9) {
            printf(" ");
        }
        printf("%d |",i+1);
        for(j = 0; j < SEATS_PER_ROW; j++) {
            printf("%c",flight->seats[i][j]);
            if(j == 1 || j == 6) {
                printf(" ");
            }
        }
        printf("|\n");
    }
    printf("   \\___________/\n");
    printf("    AB CDEFG HI\n");
}