#include"server.h"

extern Player players[MAX_PLAYER];

void write_report(char* message);

void *broadcast_response(void *arg){
    int i;
    for(i = 0; i < MAX_PLAYER; i++){
        players[i].is_using = false;
        players[i].connect_fd = NO_CONN;
        players[i].score = 0;
        players[i].state = no_player;
        players[i].address = INADDR_ANY;
    }

    int socket_fd;
    struct sockaddr_in server_addr, client_addr;
    
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(BROA_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    int message_len;
    socklen_t client_sockaddr_len;
    char message[MAX_MES_LEN];
    int new_player_id;
    char report_message[MAX_REPORT_MAS_LEN];

    while(1){
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
                players[i].address = client_addr.sin_addr.s_addr;
                break;
            }
        }

        if(new_player_id == -1){
            printf("udp receive:%s reply:%s ip:%s\n", message, player_full_message, inet_ntoa(client_addr.sin_addr));
            sprintf(report_message, "udp receive:%s reply:%s ip:%s", message, player_full_message, inet_ntoa(client_addr.sin_addr));
            strcpy(message, player_full_message);
        }
        else{
            printf("udp receive:%s reply:%d ip:%s\n", message, new_player_id, inet_ntoa(client_addr.sin_addr));
            sprintf(report_message, "udp receive:%s reply:%d ip:%s", message, new_player_id, inet_ntoa(client_addr.sin_addr));
            sprintf(message, "%d", new_player_id);
        }
        sendto(socket_fd, message, strlen(message), 0, (struct sockaddr*)&client_addr, client_sockaddr_len);
        write_report(report_message);
    }
}