#include"server.h"

extern Player players[MAX_PLAYER];
extern char current_word[20];
static int played_id = -1;
static int gaming_num = 0;

void write_report(char* message);
char *choose_word();

void tell_sb_sth(int player_id, char* message){
    char report_message[MAX_REPORT_MAS_LEN];
    if(players[player_id].is_using == true){
        write(players[player_id].connect_fd, message, strlen(message));
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

void send_leave_message(int id){
    char message[MAX_MES_LEN];
    char data_message[MAX_DATA_MES_LEN];
    
    close(players[id].connect_fd);

    players[id].is_using = false;
    players[id].score = 0;
    players[id].connect_fd = NO_CONN;
    players[id].state = no_player;

    sprintf(data_message, message_data_mode[4], id);
    sprintf(message, message_main_mode, 0, 5, data_message);
    tell_others_sth(id, message);
}

void game_right(int id){
    char message[MAX_MES_LEN];
    char data_message[MAX_DATA_MES_LEN];
    char score[5];
    static int right_num = 0;

    if(players[id].state == gaming){
        players[id].state = not_ready;
        players[id].score++;

        sprintf(score, "%d", players[id].score); 
        sprintf(data_message, message_data_mode[3], id, 2, score);
        sprintf(message, message_main_mode, id, 4, data_message);
        tell_others_sth(MAX_PLAYER, message);
        printf("player:%d guess right!\n", id);
        right_num++;

        if(right_num >= gaming_num - 1){
            bzero(message, sizeof(message));
            bzero(data_message, sizeof(data_message));
            sprintf(data_message, message_data_mode[3], id, 3, "end");
            sprintf(message, message_main_mode, id, 4, data_message);
            tell_others_sth(MAX_PLAYER, message);
            printf("game over!\n");
            gaming_num = 0;
            right_num = 0;
        }
    }
}

void transmit_message(int id, char *data){
    char message[MAX_MES_LEN];
    char data_message[MAX_DATA_MES_LEN];
    bool is_right = false;

    char *player_message = str_sub(data, 2);
    if(strcmp(player_message, current_word) == 0){
        strcpy(player_message, "*");
        if(id != played_id){
            is_right = true;
        }
    }

    sprintf(data_message, message_data_mode[1], id, player_message);
    sprintf(message, message_main_mode, id, 2, data_message);
    tell_others_sth(id, message);
    printf("player:%d message:%s\n", id, data);

    if(is_right){
        game_right(id);        
    }
}

void transmit_ink_data(int id, char *data){
    char message[MAX_MES_LEN];
    sprintf(message, message_main_mode, id, 3, data);
    tell_others_sth(id, message);
}

void game_ready(int id){
    char message[MAX_MES_LEN];
    char data_message[MAX_DATA_MES_LEN];
    char *ready_str = "ready";
    char word_data[31];

    players[id].state = ready;
    sprintf(data_message, message_data_mode[3], id, 1, ready_str);
    sprintf(message, message_main_mode, id, 4, data_message);
    tell_others_sth(id, message);

    int i;
    int ready_num = 0;
    for(i = 0; i < MAX_PLAYER; i++){
        if(players[i].is_using == true){
            if(players[i].state != ready){
                return;
            }
            else{
                ready_num++;
            }
        }
    }

    if(ready_num < 2) return;
    
    for(i = 0; i < MAX_PLAYER; i++){
        if(players[i].is_using == true){
            players[i].state = gaming;
            gaming_num++;
        }
    }

    int find_id = played_id;
    for(i = 0; i < MAX_PLAYER; i++){
        find_id = (find_id + 1) % 6;
        if(players[find_id].is_using == true && players[find_id].state == gaming){
            played_id = find_id;
            players[find_id].state = not_ready;
            break;
        }
    }

    bzero(message, sizeof(message));
    bzero(data_message, sizeof(data_message));
    strcpy(current_word, choose_word());
    sprintf(word_data, "%s,%d", current_word, GAME_DURA);
    sprintf(data_message, message_data_mode[3], played_id, 1, word_data);
    sprintf(message, message_main_mode, MAX_PLAYER, 4, data_message);
    tell_others_sth(MAX_PLAYER, message);
}

void game_contral(int id, char *data){
    int type = data[2] - '0';
    char *mes = str_sub(data, 4);
    switch(type){
        case 1:game_ready(id); break;
        default:break;
    }
}

void execute_message(int id, char *message){
    pthread_t pid;
    int sourse_id = message[0] - '0';
    int data_type = message[2] - '0';
    char *data = str_sub(message, 4);
    switch(data_type){
        case 2:transmit_message(sourse_id, data); break;
        case 3:transmit_ink_data(sourse_id, data); break;
        case 4:game_contral(sourse_id, data); break;
        default:break;
    }
}

void processing_message(int id, char *message){
    if(strcmp(message, "close") == 0){
        send_leave_message(id);
        printf("player:%d, bey!\n", id);
        return;
    }
    int end_pos = str_index_of(message, '^');
    char *sub;
    while(end_pos != -1){
        sub = str_subn(message, 0, end_pos);
        execute_message(id, sub);
        message = str_sub(message, end_pos + 1);
        end_pos = str_index_of(message, '^');
    }
}