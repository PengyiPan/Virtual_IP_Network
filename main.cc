//
//  main.c
//
//  Created by Yubo Tian on 2/24/15.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <vector>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "ipsum.h"

//int server(uint16_t port);
//int client(const char * addr, uint16_t port);

#define MAX_MSG_LENGTH (1400)
#define MAX_BACK_LOG (5)
#define NUM_THREADS (9)

using namespace std;

char *file;
FILE *fp;

uint16_t command;
uint16_t num_entries;



struct entry{
	uint32_t cost;
	uint32_t address;
};

// struct entry entries[num_entries];

struct interface_t{
	int my_port;
	int remote_port;
	char* my_VIP_addr;
    char* remote_VIP_addr;
};
typedef struct interface_t interface;
vector<interface*> my_interfaces(1);

/*          *****************      from project 0             ****************************        */


void * serverthread(void * parm);
int receive_server(uint16_t port);
int send(const char * addr, uint16_t port);

int send(const char * addr, uint16_t port)
{
    int sock;
    struct sockaddr_in server_addr;
    char msg[MAX_MSG_LENGTH], reply[MAX_MSG_LENGTH*3];
    
    if ((sock = socket(AF_INET, SOCK_STREAM/* use tcp */, 0)) < 0) {   //create socket and check error
        perror("Create socket error:");
        return 1;
    }
    
    
    printf("Socket created\n");
    server_addr.sin_addr.s_addr = inet_addr(addr);  //sin_addr is struct in_addr
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connect error:");
        return 1;
    }
    
    printf("Connected to server %s:%d\n", addr, port);
    
    int recv_len = 0;
    while (1) {
        fflush(stdin);
        printf("Enter message: \n");
        gets(msg);
        
        
        if (send(sock, msg, MAX_MSG_LENGTH, 0) < 0) {
            perror("Send error:");
            return 1;
        }
        
        
        recv_len = read(sock, reply, MAX_MSG_LENGTH*10);
        
        
        if (recv_len < 0) {
            perror("Recv error:");
            return 1;
        }
        reply[recv_len] = 0;
        printf("Server reply:\n%s\n", reply);
        memset(reply, 0, sizeof(reply));
    }
    close(sock);
    return 0;
}

int receive_server(uint16_t port){
    /*
     add your code here
     */
    
    int sock,cfd;
    int len;
    struct sockaddr_in sin;
    char temp[MAX_MSG_LENGTH], reply[MAX_MSG_LENGTH*3];
    int n;
    pthread_t  tid;             // thread id
    
    //build address data structure?
    
    if ((sock = socket(AF_INET, SOCK_STREAM,0)) < 0) {
        perror("Create socket error(server):");
        return 1;
    }
    
    printf("Socket created(server)\n");
    
    bzero(&sin, sizeof(sin)); //
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    
    
    if (bind(sock,(struct sockaddr *)&sin, sizeof(sin))<0){
        perror("Bind error:");
        return 1;
    }
    
    listen(sock,5);
    
    //wait for connection, then receive and print msg
    while(1){
        socklen_t len = sizeof(sin);
        cfd = accept(sock, (struct sockaddr*)&sin, &len);
        
        //printf("after accept\n");
        
        if (cfd<0){
            perror("Accept error:");
            return 1;
        }
        
        pthread_create(&tid, NULL, serverthread, (void *) cfd );
        
    }
    close(sock);
    return 0;
}



void * serverthread(void * parm){
    int n;
    char temp[MAX_MSG_LENGTH], reply[MAX_MSG_LENGTH*3];
    int len;
    int cfd;
    
    cfd = (int) parm;
    
    while(len = recv(cfd, temp, sizeof(temp), 0)){
        //printf("Here is the message: %s\n",temp);
        strcpy(reply,temp);
        strcat(reply,temp);
        strcat(reply,temp);
        //printf("Here is the copy: %s\n",reply);
        
        n = write(cfd,reply,MAX_MSG_LENGTH*3);
        
        
    }
    close(cfd);
    pthread_exit(0);
    
}

/*          *****************      from project 0             ****************************        */


void read_in(){
	fp = fopen( file , "rt");

	char* line= NULL;
	size_t len = 0;
	ssize_t read;

	if (fp == NULL){
		printf("Opening file error\n");
		exit(EXIT_FAILURE);
	}

	int my_port;
	int line_count = 0;



	while ((read = getline(&line, &len, fp)) != -1) {
		if (line_count == 0){
			char *pos;
			if ((pos=strchr(line, '\n')) != NULL){
				*pos = '\0';
			}
			printf("first line : %s\n", line);

			char *p;
			p = strtok(line, ":");
			//printf("local iP :%s\n", p);
			p = strtok(NULL, ":");
			my_port = atoi(p);

		} else {
			char *pos;
			if ((pos=strchr(line, '\n')) != NULL){
				*pos = '\0';
			}
			printf("%s\n", line);

			char *p;
			p = strtok(line, " ");
			p = strtok(NULL, " ");
			int temp_remote_port = atoi(p);

			p = strtok(NULL, " ");
			printf("third :%s\n", p);
			char* temp_my_VIP_addr = p;

			p = strtok(NULL, " ");
			printf("fourth :%s\n", p);
			char* temp_remote_VIP_addr = p;


			interface* new_interface = (interface*) malloc(sizeof(interface));
			new_interface-> my_port = my_port;
			new_interface-> remote_port = temp_remote_port;
			new_interface-> my_VIP_addr = temp_my_VIP_addr;
            new_interface-> remote_VIP_addr = temp_remote_VIP_addr;

			printf("remote_port in new interface is: %d \n", new_interface->remote_port);
            printf("remote_VIP in new interface is: %s \n", new_interface->remote_VIP_addr);

			my_interfaces.push_back(new_interface);



		}
		line_count ++;

	}

	fclose(fp);
}
/*
int IP_to_binary(char* ip){
	//convert IP address to unint32_t
	int s;
	uint32_t buf;
	char* str = (char*) malloc(sizeof(str));
	printf("     ip address :   %s \n",ip);

	s = inet_pton(AF_INET, ip, &buf);
	//buf = htonl(ip);

	printf("        converted to: %u \n", buf);

	//inet_ntop(AF_INET, buf, str, INET_ADDRSTRLEN);
	//printf("                    converted back: %s \n", str);
	return s;
}
*/
void* node (void* a){
	printf("in node interface\n");
//	interface* interfaces = (interface*) malloc(10*sizeof(interface));

	read_in();
	//create server that can accept message from different nodes
    
    
    
    //create client when send message
    //pull needed info from the interfaces vector te get necessary info for setting up client
    
    


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
