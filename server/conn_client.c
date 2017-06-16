#include"server.h"

extern Player players[MAX_PLAYER];

void write_report(char* message);
void send_new_player(int player_id);
void send_old_players(int player_id);

void *connect_client(void *arg){
    int socket_fd;
    struct sockaddr_in server_addr, client_addr;

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(CONN_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    bind(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(socket_fd, 20);

    int connect_fd;
    socklen_t client_len;
    int i;
    char report_message[MAX_REPORT_MAS_LEN];
    char message[MAX_MES_LEN];
    int id;

    while(1){
        client_len = sizeof(client_addr);
        bzero(&client_addr, client_len);
        
        connect_fd = accept(socket_fd, (struct sockaddr*)&client_addr, &client_len);
    
        read(connect_fd, message, MAX_MES_LEN);
        id = message[0] - '0';
        
        if(players[id].is_using == true){
            players[id].score = 0;
            players[id].state = not_ready;
            players[id].connect_fd = connect_fd;

            printf("tcp connect client. ip:%s id:%d\n", inet_ntoa(client_addr.sin_addr), id);
            sprintf(report_message, "tcp connect client. ip:%s id:%d", inet_ntoa(client_addr.sin_addr), id);
            write_report(report_message);

            send_new_player(id);
            send_old_players(id);
        }
        else{
            close(connect_fd);
            printf("tcp connect client failed: address wrong! ip:%s\n", inet_ntoa(client_addr.sin_addr));
            sprintf(report_message, "tcp connect client failed: address wrong! ip:%s", inet_ntoa(client_addr.sin_addr));
            write_report(report_message);
        }
    }
}