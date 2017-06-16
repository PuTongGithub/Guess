#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<sys/ioctl.h>
#include<pthread.h>
#include<time.h>
#include<malloc.h>
#include<fcntl.h>
#include<sys/stat.h>

#define BROA_PORT 1106
#define CONN_PORT 1107

#define MAX_MES_LEN 1024
#define NO_CONN -1
#define NAME_MAX_LEN 51

#define MAX_PLAYER 6
#define MAX_PLAYER_INFO 101

#define MAX_REPORT_LIEN 1000
#define MAX_TIME_LEN 50
#define MAX_REPORT_MAS_LEN (MAX_REPORT_LIEN - MAX_TIME_LEN)

typedef enum {false = 0, true = 1} bool;
typedef enum {no_player = -1, not_ready = 0, ready = 1, gaming = 2} state_t;

typedef struct{
    bool is_using;
    char name[NAME_MAX_LEN];
    int score;
    state_t state;
    int connect_fd;
    in_addr_t address;
}Player;

static char player_full_message[] = "player_full";
static char message_main_mode[] = "%d&%d&%s^";     //sourse_di&data_type&data\0
static char message_data_mode[][12] = {"%d#%d#%d#%s",   //1 id#state#score#name
                                        "%d#%s:%s",     //2 id#name:message
                                        "%d#%s",        //3 id#ink_data
                                        "%d#%d#%s",     //4 id#type#data 1#start 2#score 3#message
                                        "%d"};          //5 leave_id