#include"server.h"

Player players[MAX_PLAYER];
int report_fd;

void *broadcast_response(void *arg);
void *connect_client(void *arg);
void set_report_fd();

int main(){
    set_report_fd();
    
    pthread_t udp_thread_id;
    if(pthread_create(&udp_thread_id, NULL, broadcast_response, NULL) != 0){
        printf("create udp thread error!\n");
        return -1;
    }
    printf("broadcast response thread started.\n");

    pthread_t tcp_thread_id;
    if(pthread_create(&tcp_thread_id, NULL, connect_client, NULL) != 0){
        printf("create tcp thread error!\n");
        return -1;
    }
    printf("connect client thread started.\n");

    while(true){
        
    }
    return 0;
}
