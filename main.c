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

void read_in(){
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
        //printf("next line\n");
    }
    
    fclose(fp);
}

void* node_interface (void* a){
    printf("in node interface\n");
    read_in();
    //do things
    
    return NULL;
}


int main(int argc, char* argv[]){
    
    file = argv[1];
     
    pthread_t node_thread;
    
    if(pthread_create(&node_thread, NULL, node_interface, NULL)) {
        fprintf(stderr, "Error creating thread\n");
        return 1;
    }
    
    
    char* command = (char*) malloc(sizeof(command));

    while(1){
        //printf ("Enter user command: ");
        scanf ("%s", command);
        
        
        /* deal with five possible user std input command
         * ifconfig, routes, up, down, send
         *
         */
        
        if (!strcmp(command,"ifconfig")){
            // config
            printf("command is %s\n", command);

        }
        
        else if (!strcmp(command,"routes")){
            // print out routes
            printf("command is %s\n", command);

        }
        
        else if (!strcmp(command,"up")){
            printf("command is %s\n", command);

        }
        
        else if (!strcmp(command,"down")){
            printf("command is %s\n", command);

        }
        
        else if (!strcmp(command,"send")){
            printf("command is %s\n", command);

        }
        
        else if (!strcmp(command,"kill")){
            break;
        }
        
        else {
            printf("Unrecognized command. Please retry.\n");
        }
        
        
    }

    return 0;
    
    
}
