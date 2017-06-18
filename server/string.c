#include<stdio.h>
#include<malloc.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<stdlib.h>
#include<time.h>

char words[500][20];
int words_num;

int str_index_of(char *str, char ch){
    int index = 0;
    while(str[index] != '\0'){
        if(str[index] == ch){
            return index;
        }
        index++;
    }
    return -1;
}

int str_index_of_begin_with(char *str, char ch, int pos){
    if(pos == -1) return -1;
    int index = pos;
    while(str[index] != '\0'){
        if(str[index] == ch){
            return index;
        }
        index++;
    }
    return -1;
}

int str_copy(char *s_str, char *c_str, int begin, int end){
    int index = begin;
    int i = 0;
    for(; index < end; index++, i++){
        s_str[i] = c_str[index];
    }
    s_str[i] = '\0';
    return 0;
}

char* str_sub(char *str, int index){
    int len = strlen(str);
    char *sub = (char *)malloc(sizeof(char) * (len - index + 1));
    int i = 0;
    while(str[index] != '\0'){
        sub[i++] = str[index++];
    }
    sub[i] = '\0';
    return sub;
}

char* str_subn(char *str, int index, int len){
    if(len < 0) return str;
    int i;
    char *sub = (char *)malloc(sizeof(char) * (len + 1));
    for(i = 0; i < len; i++){
        sub[i] = str[index + i];
    }
    sub[i] = '\0';
    return sub;
}

void get_words_num(){
    int count = 0;
    while(strcmp(words[count++], "") != 0){}
    words_num = count - 1;
}

void get_words(){
    char buf[5000];
    bzero(buf, sizeof(buf));
    int fd = open("words", O_RDONLY);
    read(fd, buf, 5000);
    int pos = str_index_of(buf, '\n');
    int end_pos;
    int i = 0;
    str_copy(words[i++], buf, 0, pos);
    while(pos != -1){
        end_pos =  str_index_of_begin_with(buf, '\n', pos + 1);
        str_copy(words[i++], buf, pos + 1, end_pos);
        pos = end_pos;
    }
    close(fd);
    get_words_num();
}

char *choose_word(){
    srand((unsigned)time(NULL));
    int index = rand() % (words_num - 1);
    return words[index];
}

/*
int main(){
    get_words();
    get_words_num();
    printf("%s\n", choose_word());
    return 0;
}
*/