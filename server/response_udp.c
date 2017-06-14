#include"server.h"

void *broadcast_response(void *arg){
    extern Player players[MAX_PLAYER];
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

    while(true){
        new_player_id = -1;
        client_sockaddr_len = sizeof(client_addr);
        bzero(message, sizeof(message));
        bzero(&client_addr, sizeof(client_addr));

        recvfrom(socket_fd, message, MAX_MES_LEN, 0, (struct sockaddr*)&client_addr, &client_sockaddr_len);

        printf("receive message:%s\n",message);

        for(i = 0; i < MAX_PLAYER; i++){
            if(!players[i].is_using){
                new_player_id = i;
                players[i].is_using = true;
                strcpy(players[i].name, message);
                break;
            }
        }

        if(new_player_id == -1){
            strcpy(message, player_full_message);
            printf("player full...\n");
        }
        else{
            sprintf(message, "%d", new_player_id);
            printf("new player:%s\nplayer id:%d\n", players[new_player_id].name, new_player_id);
        }
        sendto(socket_fd, message, strlen(message), 0, (struct sockaddr*)&client_addr, client_sockaddr_len);
    }
}