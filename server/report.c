#include"server.h"

extern int report_fd;

void get_current_time(char* date){
    time_t timer;
    timer = time(&timer);
    strcpy(date, ctime(&timer));
}

void write_report(char* message){
    char current_time[MAX_TIME_LEN];
    get_current_time(current_time);
    char report_line[MAX_REPORT_LIEN];
    sprintf(report_line, "%s    %s\n", current_time, message);
    write(report_fd, report_line, strlen(report_line));
}