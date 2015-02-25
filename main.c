//
//  main.c
//
//
//  Created by Yubo Tian on 2/24/15.
//
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "ipsum.h"

char *file;
FILE *fp;

void* read_in(){
    fp = fopen( file , "rt");
    
    char* line= NULL;
    size_t len = 0;
    ssize_t read;
    
    if (fp == NULL){
        printf("Opening file error\n");
        exit(EXIT_FAILURE);
    }
    
    while ((read = getline(&line, &len, fp)) != -1) {
        
        char *pos;
        if ((pos=strchr(line, '\n')) != NULL){
            *pos = '\0';
        }
        printf("%s\n", line);
        printf("next line\n");
    }
    
    fclose(fp);
    return NULL;
}

void* user_interface(void* a){
    printf("user interface\n");
    //takes care of four user standard inputs
    
    
    
    return NULL;
}

int main(int argc, char* argv[]){
    
    file = argv[1];
    read_in();
    
    pthread_t user_interface_thread;
    
    if(pthread_create(&user_interface_thread, NULL, user_interface, NULL)) {
        fprintf(stderr, "Error creating thread\n");
        return 1;
    }
    
    return 0;
    
    
}
