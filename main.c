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
        printf("next line\n");
    }
    
    fclose(fp);
    //return NULL;
}

void* user_interface(void* a){
    //takes care of four user standard inputs
    bool go = true;
    char* command = NULL;
    
    printf("user interface\n");

    while(go){
        printf ("Enter user command: ");
        scanf ("%s", in);
        
        //parse in into command and arg for up/down/send
        printf("command is %s\n", command);
        
//        switch (command) {
//            case "ifconfig":
//                printf("command is ifconfig : %s\n", command);
//                break;
//            case "routes":
//                printf("command is routes : %s\n", command);
//                break;
//            case "up":
//                printf("command is up : %s\n", command);
//                break;
//            case "down":
//                printf("command is down : %s\n", command);
//                break;
//            case "send":
//                printf("command is send : %s\n", command);
//                break;
//            case "killnode":
//                printf("command is killnode: %s\n", command);
//                go = false;
//                break;
//            default:
//                printf("Illegal command. Please retry. \n");
//                break;
//        }
        
        go = false;
        
        
    }
    
    return NULL;
}

int main(int argc, char* argv[]){
    
    file = argv[1];
    
    
    pthread_t user_interface_thread;
    
    if(pthread_create(&user_interface_thread, NULL, user_interface, NULL)) {
        fprintf(stderr, "Error creating thread\n");
        return 1;
    }
    
    read_in();
    
    //printf("why do i need this\n");
    return 0;
    
    
}
