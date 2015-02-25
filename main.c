//
//  main.c
//  
//
//  Created by Yubo Tian on 2/24/15.
//
//

#include <stdio.h>
#include <stdlib.g>

FILE *fp;

int main(int argc, char* argv[]){
    char* file = argv[1];
    fp = fopen( file , "r");
    char bar[1000];
    while(1){
        fscanf(fp,"%s\n", bar);
        printf("%s", bar);
    }
    
    
    fclose(file);
}
