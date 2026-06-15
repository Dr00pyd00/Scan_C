
#include <stdio.h>
#include <pthread.h>
#include <string.h>

#define MAX_PORT    100000
#define MAX_TH      8


struct scan_ctx {
    const char *host;
    int next_port;
    int end_port;
    pthread_mutex_t m;
};


// prend une struct et prend le port a scan puis incremente pour le prochaim worker
// return le port on succcess else return -1
int get_next_port(struct scan_ctx* ctx) {
    int port;
    pthread_mutex_lock(&ctx->m);
    if ( ctx->next_port > ctx->end_port ) {
        port = -1;
    }
    else {
        port = ctx->next_port;
        ctx->next_port++;
    }
    pthread_mutex_unlock(&ctx->m);
    return port;
}


int testRepArr[MAX_PORT + 1];

void *worker(void *args) {

    struct scan_ctx *ctx = (struct scan_ctx*)args;
    int port;

    while ( (port = get_next_port(ctx)) != -1) {
        testRepArr[port]++;
    }
    return NULL;
}


int main(void) {

    struct scan_ctx myCtx;
    memset(&myCtx, 0, sizeof(myCtx));
    myCtx.host = "127.0.0.1";
    myCtx.next_port = 1;
    myCtx.end_port = MAX_PORT;

    pthread_t threads[MAX_TH];
    int i;

    pthread_mutex_init(&myCtx.m, NULL);


    for ( i = 0; i < MAX_TH; i++ ) {
        pthread_create(&threads[i], NULL, &worker, &myCtx);
    }
    for ( i = 0; i < MAX_TH; i++ ) {
        pthread_join(threads[i], NULL);
    }

    int jamais = 0, plusieurs = 0;

    for ( i = 0; i <= MAX_PORT; i++ ) {
        
        if (testRepArr[i] == 0) jamais++;
        if (testRepArr[i] > 1) plusieurs++;
    }


    printf("jamais: %d  | plusieurs: %d\n", jamais, plusieurs);


    pthread_mutex_destroy(&myCtx.m);




    return 0;
}
