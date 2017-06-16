#include<stdio.h>
#include<malloc.h>
#include<string.h>

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

/*
int main(){
    char *test = "aabbcc^ddee^asd";
    int end_pos = 0;
    char *sub;
    while(end_pos != -1){
        end_pos = str_index_of(test, '^');
        sub = str_subn(test, 0, end_pos);
        printf("%s\n", sub);
        test = str_sub(test, end_pos + 1);
    }
    return 0;
}
*/