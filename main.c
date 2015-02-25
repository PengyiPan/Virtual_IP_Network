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
#include "ipsum.h"

FILE *fp;

int main(int argc, char* argv[]){
    
    char* file = argv[1];
    fp = fopen( argv[1] , "rt");
    
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
}
