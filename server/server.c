#include"server.h"

Player players[MAX_PLAYER];
int report_fd;

void *broadcast_response(void *arg);

void set_report_fd(){
    time_t timer;
    struct tm *tm_timer;
    char date[15];
    char server_report_file_name[51];

    timer = time(&timer);
    tm_timer = localtime(&timer);
    sprintf(date, "%d_%d_%d", 1 + tm_timer->tm_mon, tm_timer->tm_mday, 1900 + tm_timer->tm_year);
    sprintf(server_report_file_name, "report/report_%s.file", date);
    report_fd = open(server_report_file_name, O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IWGRP | S_IRGRP);
}

int main(){
    set_report_fd();
    pthread_t udp_thread_id;
    if(pthread_create(&udp_thread_id, NULL, broadcast_response, NULL) != 0){
        printf("create udp thread error!\n");
        return -1;
    }

    while(true){

    }
    return 0;
}
