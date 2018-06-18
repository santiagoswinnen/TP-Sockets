#include "server.h"

int serverfd;

int handleSockets() {

    struct sockaddr_in servaddr;

    serverfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero( &servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htons(INADDR_ANY);
    servaddr.sin_port = htons(22000);

    bind(serverfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    listen(serverfd, 10);

    while(1) {
        checkForNewClients();
    }

}

void checkForNewClients() {
    int newfd = accept(serverfd, (struct sockaddr*) NULL, NULL);
    if(newfd > 0) {
        if(fork() == 0) {
            attendClient(newfd);
        }
    }
}

void attendClient(int clientfd) {
    char buffer[BUFFSIZE] = {0};
    while(1) {
        read(clientfd,buffer,BUFFSIZE);
        printf("%s\n",buffer);
    }
}
