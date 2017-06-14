#include"server.h"

Player players[MAX_PLAYER];

void *broadcast_response(void *arg);

int main(){
    /*
    pthread_t udp_thread_id;
    if(pthread_create(&udp_thread_id, NULL, broadcast_response, NULL) != 0){
        printf("create udp thread error!\n");
        return -1;
    }

    while(true){

    }
    */
    broadcast_response(NULL);
    return 0;
}
