

// Simple as possible, take one host one start and one end, use sockets + connect to forloop and check 
// if port is open or not.

// V2 


#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <unistd.h>


struct scan_context {
    const char *host;
    int next_port;
    int end_port;
};

// get the next port for routine by struct pointer
// return the next port (int) on success else return -1
int get_next_port(struct scan_context *ctx) {
    
    if ( ctx->next_port > ctx->end_port ) return -1;

    int port = ctx->next_port;
    ctx->next_port++;
    return port;

}


// take a string and try to convert to integer
// On Sucess return 0 and fill the dst, on Error return -1
int converStrToInt(char* str, int* dst) {
        char *p;
        int res = (int)strtol(str, &p, 10);
        if ( str == p ) {
            // p didnt run at all mean the first char in nan
            return -1;
        }
        else if ( *p != '\0' ) {
            // p stuck on a nan char = nan char int the str
            return -1;
        }
        *dst = res;
        return 0;
}

// create empty socket.On success return the fd,on error return -1
int createEmptySocket(void) {
    int sock;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket create failed");
        return -1;
    }
    return sock;
}


// create client socket with port and host to call.
// Om Sucess return the fd and fill struct sockaddr_in, on Error return -1
int createClientSocket(char *host, int port, struct sockaddr_in* s_addr_out) {
    int c_sock;
    struct sockaddr_in c_addr;
    memset(&c_addr, 0, sizeof(c_addr));
    struct timeval timeout;

    c_sock = createEmptySocket();
    c_addr.sin_family = AF_INET;
    c_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, host, &c_addr.sin_addr) != 1) {
        fprintf(stderr,"Invalid Host: %s\n", host);
        return -1;
    }

    // timeout setup 
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    if (setsockopt( c_sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout))) {
        fprintf(stderr, "failed to setup send timeou for port %d\n", port);
        return -1;
    }

    if (setsockopt( c_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout))) {
        fprintf(stderr, "failed to setup rcv timeou for port %d\n", port);
        return -1;
    }

    *s_addr_out = c_addr;
    return c_sock;
}



int main(int argc, char **argv) {
    
    if ( argc < 2 ) {
    fprintf(stderr, "Bad Args!\nUsage: prog <host> <start> <end>\n");
    return 1;
    }

    char* HOST = argv[1];
    int START = 1;
    int END = 63000;

    int s, i;
    struct sockaddr_in s_a;

    if ( argc >= 3 ) {
        if (converStrToInt(argv[2], &START)) {
            fprintf(stderr, "your START is not a number\nDefault Start setup: %d\n", START);
        }
    }

    if ( argc >= 4 ) {
        if (converStrToInt(argv[3], &END)) {
            fprintf(stderr, "your END is not a number\nDefault End setup: %d\n", END);
        }
    }

    printf("Scanning on host:%s | start_port = %d | end_port = %d\n", HOST, START, END );

    
    for ( i = START; i <= END; i++ ) {
    
            
        s = createClientSocket(HOST, i, &s_a);

        int re = connect(s, (const struct sockaddr*)&s_a, sizeof(s_a));
        if ( re == 0 ) {
            printf("port %d open\n", i);
        }   
        else {
            //printf("port close\n");
        }
        close(s);
       }











    return 0;
}
