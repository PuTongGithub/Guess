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