#include"server.h"

void write_report(int report_fd, char* message);

void *broadcast_response(void *arg){
    extern Player players[MAX_PLAYER];
    extern int report_fd;
    int i;
    for(i = 0; i < MAX_PLAYER; i++){
        players[i].is_using = false;
        players[i].connect_fd = NO_CONN;
        players[i].score = 0;
    }

    int socket_fd;
    struct sockaddr_in server_addr, client_addr;
    
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERV_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    int message_len;
    socklen_t client_sockaddr_len;
    char message[MAX_MES_LEN];
    int new_player_id;
    char report_message[MAX_REPORT_MAS_LEN];

    while(true){
        new_player_id = -1;
        client_sockaddr_len = sizeof(client_addr);
        bzero(message, sizeof(message));
        bzero(&client_addr, sizeof(client_addr));

        recvfrom(socket_fd, message, MAX_MES_LEN, 0, (struct sockaddr*)&client_addr, &client_sockaddr_len);

        for(i = 0; i < MAX_PLAYER; i++){
            if(!players[i].is_using){
                new_player_id = i;
                players[i].is_using = true;
                strcpy(players[i].name, message);
                break;
            }
        }

        if(new_player_id == -1){
            sprintf(report_message, "receive:%s reply:%s", message, player_full_message);
            strcpy(message, player_full_message);
        }
        else{
            sprintf(report_message, "receive:%s reply:%d", message, new_player_id);
            sprintf(message, "%d", new_player_id);
        }
        sendto(socket_fd, message, strlen(message), 0, (struct sockaddr*)&client_addr, client_sockaddr_len);
        write_report(report_fd, report_message);
    }
}