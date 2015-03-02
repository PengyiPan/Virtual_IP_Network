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
#include <unistd.h>
#include <string>
#include <iostream>
#include "ipsum.h"
#include <fstream>
#include <sstream>
#include <netdb.h>
#include <sys/types.h>
#include <time.h>

//int server(uint16_t port);
//int client(const char * addr, uint16_t port);

#define MAX_MSG_LENGTH (1400)
#define MAX_BACK_LOG (5)
#define RECEIVE_BUFSIZE (65535)

using namespace std;

char** argv_in;
int u_socket;                         /* our socket */

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

struct forwarding_table_entry{
	string remote_VIP_addr;
	int cost;
	//time_to_live;
};
typedef struct forwarding_table_entry FTE;

int ifconfig();

vector<interface*> my_interfaces(0);
vector<FTE*> my_forwarding_table(0);

int send(char* des_VIP_addr,char* mes_to_send)
{
	//LOOK up forwarding table to find which port to use



	struct sockaddr_in servaddr;    /* server address */

	/* fill in the server's address and data */
	memset((char*)&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;

	//===HARD CODE START===

	struct sockaddr_in sa_loc;
	struct hostent* pLocalHostInfo = gethostbyname("localhost");
	long LocalHostAddress;
	memcpy( &LocalHostAddress, pLocalHostInfo->h_addr, pLocalHostInfo->h_length );

	if(!strcmp(des_VIP_addr,"10.10.168.73")){ //sending from A
		servaddr.sin_port = htons(17001);
	}
	else if(!strcmp(des_VIP_addr,"14.230.5.36")){//B forwarding to C
		servaddr.sin_port = htons(17002);
	}
	else{
		printf("No port to send in else\n");
	}

	servaddr.sin_addr.s_addr = LocalHostAddress;

	//===HARD CODE END===

	if (sendto(u_socket, mes_to_send, strlen(mes_to_send), 0, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("sendto failed");
		return 0;
	}


	return 0;
}

int update_forwarding_table(){


	return 0;
}


int start_receive_service(uint16_t port){

	struct sockaddr_in myaddr;      /* our address */
	struct sockaddr_in remaddr;     /* remote address */
	socklen_t addrlen = sizeof(remaddr);            /* length of addresses */
	int recvlen;                    /* # bytes received */

	unsigned char buf[RECEIVE_BUFSIZE];     /* receive buffer */

	/* create a UDP socket */

	if ((u_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
			perror("cannot create socket\n");
			return 0;
	}

	/* bind the socket to any valid IP address and a specific port */

	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(my_port);

	if (bind(u_socket, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
			perror("bind failed");
			return 0;
	}

	/* now loop, receiving data and printing what we received */
	while (1) {
			printf("waiting on port %d\n", my_port);
			recvlen = recvfrom(u_socket, buf, RECEIVE_BUFSIZE, 0, (struct sockaddr *)&remaddr, &addrlen);
			printf("received %d bytes\n", recvlen);
			if (recvlen > 0) {
				buf[recvlen] = 0;
				//HARD CODE============================
				if(my_port==17001){ //receiving from A
						string tmp("14.230.5.36");
						send((char*)tmp.c_str(),(char*)&buf);
					}
				else{
					printf("received message: %s\n", buf);
				}
				//HARD CODE====================
			}
	}

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

			FTE* new_FTE = new FTE;

			new_FTE -> remote_VIP_addr = temp_remote_VIP_addr;
			new_FTE -> cost = 1;

			my_forwarding_table.push_back(new_FTE);

		}

		line_count ++;

	}

}

void* node (void* a){
	printf("in node interface\n");

	//read the input file
	read_in();

	//create server that can accept message from different nodes

	start_receive_service(my_port);

	update_forwarding_table();

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

int down_interface(int interface_id){ //TODO: Close the UDP socket
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

			send(dest_ip,to_send_msg);

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
