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
#include <string>
#include <iostream>
#include "ipsum.h"
#include <iostream>
#include <fstream>
#include <sstream>

//int server(uint16_t port);
//int client(const char * addr, uint16_t port);

#define MAX_MSG_LENGTH (1400)
#define MAX_BACK_LOG (5)
#define NUM_THREADS (9)

using namespace std;

char** argv_in;

uint16_t command;
uint16_t num_entries;
uint16_t my_port;


struct entry{
	uint32_t cost;
	uint32_t address;
};

// struct entry entries[num_entries];

struct interface_t{
	uint16_t unique_id;
	uint16_t my_port;
	uint16_t remote_port;
	string my_VIP_addr;
    string remote_VIP_addr;
    int status;

};
typedef struct interface_t interface;

int ifconfig();

vector<interface*> my_interfaces(0);

int send(const char * addr, uint16_t port)
{
	int sock;
	struct sockaddr_in server_addr;
	char msg[MAX_MSG_LENGTH], reply[MAX_MSG_LENGTH*3];

	if ((sock = socket(AF_INET, SOCK_STREAM/* use tcp */, 0)) < 0) {   //create socket and check error
		perror("Create socket error:");
		return 1;
	}


	printf("Socket created in send\n");
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

void * receive_thread(void * parm){
	//refresh the forwarding table here and print message to console

	int n;
	char temp[65535];
	int len;
	int cfd;

	cfd = *(int*) parm;

	while(len = recv(cfd, temp, sizeof(temp), 0)){

		printf("echo to console: %s\n",temp);
		memset(temp, 0, sizeof(temp));

	}
	close(cfd);
	pthread_exit(0);

}

int start_receive(uint16_t port){

	int sock,cfd;
	int len;
	struct sockaddr_in sin;
	char temp[MAX_MSG_LENGTH], reply[MAX_MSG_LENGTH*3];
	int n;
	pthread_t  tid;             // thread id


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

		pthread_create(&tid, NULL, receive_thread, (void*) &cfd );

	}
	close(sock);
	return 0;
}

/*          *****************      from project 0             ****************************        */


void read_in(){
	int line_count = 0;
	ifstream file(argv_in[1]);
	string line_buffer;

	while (getline(file, line_buffer)){
		if (line_buffer.length() == 0)continue;

		if(line_count == 0){
			//my_port = atoi(line_buffer.substr(line_buffer.find(":")).c_str());
			my_port = atoi(line_buffer.substr(line_buffer.find(":")+1).c_str());
		}
		else{
			int temp_remote_port = atoi(line_buffer.substr(line_buffer.find(":")+1,line_buffer.find(" ")).c_str());
			line_buffer = line_buffer.substr(line_buffer.find(" ")+1);

			string temp_my_VIP_addr = line_buffer.substr(0,line_buffer.find(" "));
			line_buffer = line_buffer.substr(line_buffer.find(" ")+1);

			string temp_remote_VIP_addr = line_buffer;

			interface* new_interface = new interface;
			new_interface -> unique_id = line_count;
			new_interface -> my_port = my_port;
			new_interface -> remote_port = temp_remote_port;
			new_interface -> my_VIP_addr = temp_my_VIP_addr;
			new_interface -> remote_VIP_addr = temp_remote_VIP_addr;
			new_interface -> status = 1;

			my_interfaces.push_back(new_interface);

		}

		line_count ++;

	}

}

void* node (void* a){
	printf("in node interface\n");
	//	interface* interfaces = (interface*) malloc(10*sizeof(interface));

	read_in();

	//create server that can accept message from different nodes

	start_receive(my_port);

	//create client when send message
	//pull needed info from the interfaces vector te get necessary info for setting up client




	return NULL;
}


int ifconfig(){
	for(int i =0; i< my_interfaces.size();i++){
		interface* cur = my_interfaces[i];
		const char* std;
		if(cur->status){
			string up("up");
			std = up.c_str();
		}else{
			string down("down");
			std = down.c_str();
		}
		printf("%d %s %s\n",cur->unique_id,cur->my_VIP_addr.c_str(),std);
	}
	return 0;
}

int up_interface(int interface_id){
	for(int i =0; i< my_interfaces.size();i++){
		if(my_interfaces[i]->unique_id == interface_id){

			my_interfaces[i]->status = 1;
			printf("Interface %d up.\n", my_interfaces[i]->unique_id);
			return 0;
		}
	}
	printf("Interface %d not found.\n", interface_id);
	return 0;
}

int down_interface(int interface_id){
	for(int i =0; i< my_interfaces.size();i++){
		if(my_interfaces[i]->unique_id == interface_id){
			my_interfaces[i]->status = 0;
			printf("Interface %d down.\n", my_interfaces[i]->unique_id);
			return 0;
		}
	}
	printf("Interface %d not found.\n", interface_id);
	return 0;
}


int main(int argc, char* argv[]){

	argv_in = argv;;

	pthread_t node_thread;

	if(pthread_create(&node_thread, NULL, node, NULL)) {
		fprintf(stderr, "Error creating thread\n");
		return 1;
	}


	char* command = (char*) malloc(sizeof(command));

	string mystr;

	while(1){
		//printf ("Enter user command: ");
		//scanf ("%s", command);
		getline (cin, mystr);

		std::vector<char> writable(mystr.begin(), mystr.end());
		writable.push_back('\0');

		command = &writable[0];

		char *t;
		t = strtok(command, " ");

		if (!strcmp(t,"ifconfig")){
			// config
			printf("command is %s\n", t);
			ifconfig();

		}

		else if (!strcmp(t,"routes")){
			// print out routes
			printf("command is %s\n", t);

		}

		else if (!strcmp(t,"up")){
			t = strtok(NULL, " ");
			int up_id = atoi(t);
			printf("command is %s, bringing up interface id: %d \n", t, up_id);
			if(up_interface(up_id)<0){
				printf("up interface error\n");
				return 0;
			}
		}

		else if (!strcmp(t,"down")){
			printf("command is %s\n", t);
			t = strtok(NULL, " ");
			int down_id = atoi(t);
			printf("command is %s, taking down interface id: %d \n", t, down_id);

			if(down_interface(down_id)<0){
				printf("down interface error\n");
				return 0;
			}

		}

		else if (!strcmp(t,"send")){

			t = strtok(NULL, " ");
			char* dest_ip = t;
			printf("command is send, destination ip address: %s \n", dest_ip);

			t = strtok(NULL, "");
			printf("message :%s\n", t);
			char* to_send_msg = t;
			// find the corresponding interface

			// create client socket with dest_ip


		}

		else if (!strcmp(t,"kill")){
			break;
		}

		else {
			printf("Unrecognized command. Please retry.\n");
		}


	}

	return 0;


}
