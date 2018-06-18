
#include "client.h"

int openSocket() {
    int socketfd;
    struct sockaddr_in servaddr;

    socketfd = socket(AF_INET,SOCK_STREAM,0);
    bzero(&servaddr,sizeof servaddr);


    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(22000);

    if(inet_pton(AF_INET,"127.0.0.1",&(servaddr.sin_addr)) < 0) {
        printf("Invalid address\n");
        return -1;
    };

    if(connect(socketfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        printf("Connection failed\n");
        return -1;
    }

    return socketfd;
}

