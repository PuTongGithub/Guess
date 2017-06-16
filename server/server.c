#include"server.h"

Player players[MAX_PLAYER];
int report_fd;

void *broadcast_response(void *arg);
void *connect_client(void *arg);
void set_report_fd();
void processing_message(int id, char *message);
void write_report(char *message);

int main(){
    set_report_fd();
    
    pthread_t udp_thread_id;
    if(pthread_create(&udp_thread_id, NULL, broadcast_response, NULL) != 0){
        printf("create udp thread error!\n");
        write_report("create udp thread error!");
        return -1;
    }
    printf("broadcast response thread started.\n");

    pthread_t tcp_thread_id;
    if(pthread_create(&tcp_thread_id, NULL, connect_client, NULL) != 0){
        printf("create tcp thread error!\n");
        write_report("create tcp thread error!");        
        return -1;
    }
    printf("connect client thread started.\n");


    int max_fd;
    int i;
    fd_set read_fds;
    struct timeval tv;
    bzero(&tv, sizeof(tv));
    char message[MAX_MES_LEN];
    while(true){
        FD_ZERO(&read_fds);
        max_fd = -1;
        for(i = 0; i < MAX_PLAYER; i++){
            if(players[i].is_using == true && players[i].connect_fd != NO_CONN){
                FD_SET(players[i].connect_fd, &read_fds);
                max_fd = players[i].connect_fd > max_fd ? players[i].connect_fd : max_fd;
            }
        }

        select(max_fd + 1, &read_fds, NULL, NULL, &tv);

        for(i = 0; i < MAX_PLAYER; i++){
            if(players[i].is_using == true && players[i].connect_fd != NO_CONN){
                if(FD_ISSET(players[i].connect_fd, &read_fds)){
                    bzero(&message, sizeof(message));
                    if(read(players[i].connect_fd, message, MAX_MES_LEN - 1) != EOF){
                        processing_message(i, message);
                    }
                    else{
                        processing_message(i, "close");
                    }
                }
            }
        }
    }
    return 0;
}
