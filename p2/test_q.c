


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NB_THREADS  8
#define MAX_PORT    100000


struct scan_context {
    const char*host;
    int next_port;
    int end_port;
    pthread_mutex_t m;
};

// get the next port for routine by struct pointer
// return the next port (int) on Success else return -1
int get_next_port(struct scan_context *ctx) {
    int port;
    pthread_mutex_lock(&ctx->m);
    if ( ctx->next_port > ctx->end_port ) {
        port = -1;
    } else {
    port = ctx->next_port;
    ctx->next_port++;
    }
    pthread_mutex_unlock(&ctx->m);
    return port;
}

// tableau gobal compte combien de fois chaque port est pioche
int compteur[MAX_PORT +1];

void *worker(void *args) {
    struct scan_context *ctx = (struct scan_context *)args;
    int port;

    while ( (port = get_next_port(ctx)) != -1 ) {
        compteur[port]++;
    }
    return NULL;
}


int main() {

    struct scan_context ctx;
    ctx.host = "127.0.0.1";
    ctx.next_port = 1;
    ctx.end_port = MAX_PORT;

    pthread_mutex_init(&ctx.m, NULL);

    pthread_t threads[NB_THREADS];
    int i;
    for (i = 0; i < NB_THREADS; i++ ) {
        pthread_create(&threads[i], NULL, worker, &ctx);
    }
    for ( i = 0; i < NB_THREADS; i++ ) {
        pthread_join(threads[i], NULL);
    }

    // verif 
    int jamais =0, plusieurs = 0;

    for ( i = 1; i <= MAX_PORT; i++ ) {
        if (compteur[i] == 0) jamais++;
        if (compteur[i] > 1) plusieurs++;
    }

    printf("ports jamais piochees %d\n", jamais);
    printf("ports pioches plusieurs fois %d\n", plusieurs);
    
    pthread_mutex_destroy(&ctx.m);

    return 0;
}


