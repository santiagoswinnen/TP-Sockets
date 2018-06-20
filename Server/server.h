#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#define BUFFSIZE 128

int handleSockets();
void checkForNewClients();
void attendClient(int clientfd);
