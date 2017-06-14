#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<sys/ioctl.h>
#include<pthread.h>
#define SERV_PORT 1106
#define MAX_MES_LEN 1024
#define NO_CONN -1
#define NAME_MAX_LEN 51
#define MAX_PLAYER 6

typedef enum {false = 0, true = 1} bool;

typedef struct{
    bool is_using;
    int connect_fd;
    char name[NAME_MAX_LEN];
    int score;
}Player;

static char player_full_message[] = "player_full";