#include <stdio.h>
#include "client.h"

int main() {
    char buff[100] = "";
    int socketDescriptor = openSocket();
    int c;
    int i = 0;
    while(1) {
        c = getchar();
        buff[i++] = (char)c;
        buff[i] = 0;
        if(c == '\n') {
            write(socketDescriptor,buff,100);
            i = 0;
        }

    }
}