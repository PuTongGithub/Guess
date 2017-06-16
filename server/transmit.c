#include"server.h"

extern Player players[MAX_PLAYER];

void write_report(char* message);

void tell_sb_sth(int player_id, char* message){
    char report_message[MAX_REPORT_MAS_LEN];
    if(players[player_id].is_using == true){
        write(players[player_id].connect_fd, message, strlen(message));
        fsync(players[player_id].connect_fd);
        sprintf(report_message, "tcp send message to someone. target:%d message:%s", player_id, message);
        write_report(report_message);
    }
}

void tell_others_sth(int sourse_id, char* message){
    char report_message[MAX_REPORT_MAS_LEN];
    int i;
    for(i = 0; i < MAX_PLAYER; i++){
        if(i != sourse_id && players[i].is_using == true){
            write(players[i].connect_fd, message, strlen(message));
            fsync(players[i].connect_fd);
        }
    }
    sprintf(report_message, "tcp send message to others. sourse:%d message:%s", sourse_id, message);
    write_report(report_message);
}


void set_player_info(int player_id, char* info){
    if(players[player_id].is_using == true){
        sprintf(info, message_data_mode[0], player_id, players[player_id].state, players[player_id].score, players[player_id].name);
    }
}

void send_new_player(int player_id){
    char player_info[MAX_PLAYER_INFO];
    set_player_info(player_id, player_info);
    char message[MAX_MES_LEN];
    sprintf(message, message_main_mode, player_id, 1, player_info);
    tell_others_sth(player_id, message);
}

void send_old_players(int player_id){
    char player_info[MAX_PLAYER_INFO];
    char message[MAX_MES_LEN];
    int i;
    for(i = 0; i < MAX_PLAYER; i++){
        if(i != player_id && players[i].is_using == true){
            set_player_info(i, player_info);
            sprintf(message, message_main_mode, player_id, 1, player_info);
            tell_sb_sth(player_id, message);
        }
    }
}