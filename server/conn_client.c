#include"server.h"

void write_report(char* message);
extern Player players[MAX_PLAYER];

void tell_sb_sth(int player_id, char* message){
    char report_message[MAX_REPORT_MAS_LEN];
    if(players[player_id].is_using == true){
        write(players[player_id].connect_fd, message, strlen(message));
    }
    sprintf(report_message, "tcp send message to someone. target:%d message:%s", player_id, message);
    write_report(report_message);
}

void tell_others_sth(int sourse_id, char* message){
    char report_message[MAX_REPORT_MAS_LEN];
    int i;
    for(i = 0; i < MAX_PLAYER; i++){
        if(i != sourse_id && players[i].is_using == true){
            write(players[i].connect_fd, message, strlen(message));
        }
    }
    sprintf(report_message, "tcp send message to others. sourse:%d message:%s", sourse_id, message);
    write_report(report_message);
}

void set_player_info(int player_id, char* info){
    if(players[player_id].is_using == true){
        sprintf(info, "%d#%d#%d#%s", player_id, players[player_id].state, players[player_id].score, players[player_id].name);
    }
}

void send_new_player(int player_id){
    char player_info[MAX_PLAYER_INFO];
    set_player_info(player_id, player_info);
    tell_others_sth(player_id, player_info);
}

void send_old_players(int player_id){
    char player_info[MAX_PLAYER_INFO];
    int i;
    for(i = 0; i < MAX_PLAYER; i++){
        if(i != player_id && players[i].is_using == true){
            set_player_info(i, player_info);
            tell_sb_sth(player_id, player_info);
        }
    }
}

void *connect_client(void *arg){
    int socket_fd;
    struct sockaddr_in server_addr, client_addr;

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(CONN_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(socket_fd, 128);

    int connect_fd;
    socklen_t client_len;
    int i;
    bool player_exist;
    char report_message[MAX_REPORT_MAS_LEN];

    while(1){
        client_len = sizeof(client_addr);
        bzero(&client_addr, client_len);
        player_exist = false;
        
        connect_fd = accept(socket_fd, (struct sockaddr*)&client_addr, &client_len);
        for(i = 0; i < MAX_PLAYER; i++){
            if(players[i].is_using == true && players[i].address == client_addr.sin_addr.s_addr){
                players[i].score = 0;
                players[i].state = not_ready;
                players[i].connect_fd = connect_fd;

                sprintf(report_message, "tcp connect client. ip:%s id:%d", inet_ntoa(client_addr.sin_addr), i);
                write_report(report_message);

                send_new_player(i);
                send_old_players(i);

                player_exist = true;
                break;
            }
        }

        if(player_exist == false){
            close(connect_fd);
            sprintf(report_message, "tcp connect client failed: address wrong! ip:%s", inet_ntoa(client_addr.sin_addr));
            write_report(report_message);
        }
    }
}