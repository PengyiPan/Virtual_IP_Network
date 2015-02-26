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
#include <arpa/inet.h>
#include "ipsum.h"

char *file;
FILE *fp;

uint16_t command;
uint16_t num_entries;

struct entry{
    uint32_t cost;
    uint32_t address;
};

// struct entry entries[num_entries];

struct interface{
    int my_port;
    int remote_port;
    uint32_t my_VIP_address;
    uint32_t remote_IP_address;
};


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

int IP_to_binary(char* ip){
    //convert IP address to unint32_t
    int s;
    uint32_t buf;
    
    char* str = (char*) malloc(sizeof(str));
    
    printf("     ip address :   %s \n",ip);

    s = inet_pton(AF_INET, ip, &buf);
    
    printf("        converted to: %u \n", buf);
    
    
    //inet_ntop(AF_INET, buf, str, INET_ADDRSTRLEN);

    //printf("                    converted back: %s \n", str);
    return s;
}

void* node (void* a){
    printf("in node interface\n");
    read_in();
    //create socket
    
    
    char* ip = (char*) malloc(sizeof(ip));
    ip = "10.116.89.157";
    IP_to_binary(ip);
    ip = "255.0.255.0";
    IP_to_binary(ip);
    
    
    return NULL;
}







int main(int argc, char* argv[]){

    file = argv[1];
     
    pthread_t node_thread;
    
    if(pthread_create(&node_thread, NULL, node, NULL)) {
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
