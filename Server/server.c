#include <string.h>
#include <unistd.h>
#include "server.h"

int handleSockets() {
    int listen_fd;
    int comm_fd;

    char str [100];

    struct sockaddr_in servaddr;

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    bzero( &servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htons(INADDR_ANY);
    servaddr.sin_port = htons(22000);

    bind(listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    listen(listen_fd, 10);

    comm_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL);


    while (1) {


        /*Leer todos los sockets*/
        read(comm_fd,str,100);

        printf("Echoing back - %s",str);

        write(comm_fd, str, strlen(str)+1);



        int i;

        for (i = 0; i < MAX_SOCKETS; i++) {
            FD_SET(sockets[i], &set);
        }

        int returned = select(highfd + 1, &set, NULL, NULL, NULL);

        if (returned) {
            for (i = 0; i < MAX_SOCKETS; i++) {
                if (FD_ISSET(sockets[i], &set)) {
                    int byteCount = recvfrom(sockets[i], (void*)&buffer, sizeof(buffer), 0, NULL, NULL);
                    printf("%s", buffer);
                }
            }
        }
    }
}