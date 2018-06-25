#include "server.h"

extern int serverfd;
extern sqlite3 * db;

int main() {
	serverfd = sqlite3_open("tp4data.db", &db);
	if(serverfd) {
		fprintf(stderr, "Database cannot be open: %s\n", sqlite3_errmsg(db));
		return 1;
	}
	initializeTables();
    handleSockets();
    sqlite3_close(db);
    return 0;
}

