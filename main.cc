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
#include <netinet/ip.h>

//int server(uint16_t port);
//int client(const char * addr, uint16_t port);

#define MAX_MSG_LENGTH (1400)
#define MAX_BACK_LOG (5)
#define RECEIVE_BUFSIZE (65535)
#define IP_PACKET_MAX_PAYLOAD (1336)

using namespace std;

char** argv_in;
int u_socket;                         /* our socket */
pthread_mutex_t ft_lock;

uint16_t my_port;

struct interface_t{
	uint16_t unique_id;
	uint16_t my_port;
	uint16_t remote_port;
	struct in_addr my_VIP_addr;
	struct in_addr remote_VIP_addr;
	int status;
};
typedef struct interface_t interface;

struct forwarding_table_entry{
	struct in_addr remote_VIP_addr;
	uint16_t interface_uid;
	int cost;
	time_t time_last_updated;//time_to_live;
};
typedef struct forwarding_table_entry FTE;

/*The packet that will be encapsulated in IP packet as payload*/
struct entry_t{
	uint32_t cost;
	uint32_t address;
};
typedef struct entry_t entry;


struct RIP_packet{
	uint16_t command;
	uint16_t num_entries;
	entry entries[64];
};

typedef struct RIP_packet RIP_packet;

/*The packet that we send using UDP as the link layer*/

struct IP_packet {
	struct ip ip_header;
	char msg[IP_PACKET_MAX_PAYLOAD];
};



int ifconfig();
void update_forwarding_table(RIP_packet* RIP, struct in_addr new_next_hop_VIP_addr);

vector<interface*> my_interfaces(0);
vector<FTE*> my_forwarding_table(0);

/*Compare two in_addr struct, return true if the addr is the same*/
bool in_addr_compare(struct in_addr addr1,struct in_addr addr2){

	char addr1_char[50];
	char addr2_char[50];
	inet_ntop(AF_INET, &addr1,  addr1_char, INET_ADDRSTRLEN);
	inet_ntop(AF_INET, &addr2,  addr2_char, INET_ADDRSTRLEN);
	if(strcmp(addr1_char,addr2_char) == 0){
		return true;
	}else{
		return false;
	}
}

int send(struct in_addr des_VIP_addr,char* mes_to_send,int msg_length,bool msg_encapsulated,bool msg_is_RIP)
{
	interface* interface_to_use;
	bool found = 0;
	for(int i=0;i < my_forwarding_table.size();i++){
		FTE* cur = my_forwarding_table[i];
		if(in_addr_compare(des_VIP_addr,cur->remote_VIP_addr)){// can be reach

			for(int j=0;j<my_interfaces.size();j++){
				if(my_interfaces[j]->unique_id == cur->interface_uid){
					interface_to_use = my_interfaces[j];
					found = 1;
					//printf("In send. Found next hop. Interface id: %d\n",cur->interface_uid);
				}
			}
		}
	}

	if(!found){
		char str[50];
		inet_ntop(AF_INET, &des_VIP_addr, str, INET_ADDRSTRLEN);
		printf("%s cannot be reached\n",str);
	}
	/*Found which interface to use*/

	if(!msg_encapsulated){
		/*Msg not encapsulated, need to create IP packet*/

		struct IP_packet* ip_packet_to_send = new IP_packet;
		struct ip* ip_header = new ip;

		ip_header->ip_dst = des_VIP_addr;
		ip_header->ip_src = interface_to_use->my_VIP_addr;

		if(msg_is_RIP){
			ip_header->ip_p = 200;
		}else{
			ip_header->ip_p = 0;
		}

		ip_header->ip_ttl = 16;

		ip_header->ip_sum = htons(ip_sum((char*)ip_header, sizeof(*ip_header)));

		memcpy(&(ip_packet_to_send->ip_header),ip_header,sizeof(*ip_header));
		//ip_packet_to_send->ip_header = *ip_header;

		memcpy(&(ip_packet_to_send->msg),mes_to_send,msg_length);

		mes_to_send = (char*)ip_packet_to_send;
	}

	/*Finished encapsulation*/

	struct sockaddr_in servaddr;    /* server address */

	/* fill in the server's address and data */
	memset((char*)&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;


	struct sockaddr_in sa_loc;
	struct hostent* pLocalHostInfo = gethostbyname("localhost");
	long LocalHostAddress;
	memcpy( &LocalHostAddress, pLocalHostInfo->h_addr, pLocalHostInfo->h_length );

	servaddr.sin_port = htons(interface_to_use->remote_port);

	servaddr.sin_addr.s_addr = LocalHostAddress;


	if (sendto(u_socket, mes_to_send, sizeof(IP_packet), 0, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("SEND failed\n");
		return 0;
	}


	return 0;
}

void forward_or_print(IP_packet* packet){

	struct ip* ip = &(packet->ip_header);
	struct in_addr des_addr = ip->ip_dst;

	for(int i=0;i<my_interfaces.size();i++){
		interface* cur = my_interfaces[i];
		if(in_addr_compare(des_addr,cur->my_VIP_addr)){
			printf("received: %s\n",packet->msg);
			return;
		}
	}
	/*Des addr not found in infterfaces, forward*/
	char str[50];
	inet_ntop(AF_INET, &des_addr, str, INET_ADDRSTRLEN);
	printf("Forwarding to %s\n",str);
	send(des_addr, (char*)packet,sizeof(*packet),true,false);
}

void handle_packet(IP_packet* ip_packet){

	struct ip* ip = &(ip_packet->ip_header);

	uint16_t rcv_cksum = ntohs(ip->ip_sum);

	ip->ip_sum = 0;

	uint16_t cal_cksum = ntohs(ip_sum((char*)ip,sizeof(*ip)));
	if(rcv_cksum != cal_cksum) {
		printf("***Checksum Mismatch***\n");
		/* discard packet */
	} else {
		printf("***Checksum Match***\n");
		uint8_t ttl = ip->ip_ttl; /* check if zero */
		if(ttl <= 1) {
			/* packet: timeout drop packet*/
			printf("packet timed out, dropped\n");

		} else {
			ip->ip_ttl = ttl - 1;
			ip->ip_sum = htons(ip_sum((char*)ip, sizeof(*ip)));
			/* IP packet manipulation complete */

			uint8_t type = ip->ip_p;
			if(type == 0){
				/*Payload is test data*/
				forward_or_print(ip_packet);
			}
			else if(type == 200){
				/*Payload is RIP, process*/
				struct in_addr src_addr = ip->ip_src ;
				update_forwarding_table((RIP_packet*)&(ip_packet->msg),src_addr);

			}else{
				printf("Payload is neither data nor RIP, dropped\n");
			}

		}
	}

}

void merge_route(entry new_entry , int next_hop_interface_id, in_addr next_hop_VIP_addr){

	int i;

	for( i = 0; i < my_forwarding_table.size(); i++){

		uint32_t my_dest_addr = htonl(my_forwarding_table[i] -> remote_VIP_addr.s_addr);

		if ( my_dest_addr == new_entry.address ){
				if ( new_entry.cost + 1 < my_forwarding_table[i] -> cost ) {

					FTE * new_FTE = new FTE;
					new_FTE -> interface_uid = next_hop_interface_id;

					// convert uint32_t address to in_addr
				    struct in_addr temp_ip_addr;
				    temp_ip_addr.s_addr = ntohl(new_entry.address);


					new_FTE -> remote_VIP_addr = temp_ip_addr;
					new_FTE -> cost = new_entry.cost + 1;
					new_FTE -> time_last_updated = time(NULL);

					//pthread_mutex_lock(&ft_lock);
					my_forwarding_table[i] = new_FTE;
					//pthread_mutex_unlock(&ft_lock);

					return;

				} else if ( in_addr_compare( (my_forwarding_table[i] -> remote_VIP_addr), next_hop_VIP_addr )){

					//pthread_mutex_lock(&ft_lock);

					my_forwarding_table[i] -> interface_uid = next_hop_interface_id;
					my_forwarding_table[i] -> time_last_updated = time(NULL);

					//pthread_mutex_unlock(&ft_lock);

					return;

				} else {
					return;
				}
			}

	}
	if (i == my_forwarding_table.size() ){
		// found a new route
		if (my_forwarding_table.size() < 64 ){
			FTE* new_FTE = new FTE;

			new_FTE -> interface_uid = next_hop_interface_id;

			// convert uint32_t address to string
		    struct in_addr temp_ip_addr;
		    temp_ip_addr.s_addr = ntohl(new_entry.address);


			new_FTE -> remote_VIP_addr = temp_ip_addr;
			new_FTE -> cost = new_entry.cost + 1;
			new_FTE -> time_last_updated = time(NULL);

			//pthread_mutex_lock(&ft_lock);
			my_forwarding_table.push_back( new_FTE );
			//pthread_mutex_unlock(&ft_lock);

			return;
		} else {
			return;
		}
	}
}

void update_forwarding_table(RIP_packet* RIP, struct in_addr new_next_hop_VIP_addr){
	// called by rip response
	// bellman-ford
	int numNewRoutes = RIP -> num_entries;

	int next_hop_interface_id = 0;
	for( int i = 0; i < my_interfaces.size(); i++){
		if ( in_addr_compare( (my_interfaces[i]->remote_VIP_addr) , new_next_hop_VIP_addr )){
			next_hop_interface_id = i;
			break;
		}
	}

	int i;

	for (i = 0 ; i < numNewRoutes ; i++){
		merge_route( RIP->entries[i] , next_hop_interface_id,  new_next_hop_VIP_addr );
	}

}

struct RIP_packet* create_RIP_packet(interface* cur_interface){
	//create a RIP_packet that should be sent to cur_interface, w/ split horizon and poison reverse

	struct in_addr destination_VIP = cur_interface -> remote_VIP_addr;

	struct RIP_packet* RIP_packet_tosend = new struct RIP_packet;

	uint16_t temp_num_entries = my_forwarding_table.size();
	uint16_t temp_command = 2;

	//entry temp_entries[64];

	for (int i = 0; i < my_forwarding_table.size(); i++){
		FTE* cur_FTE = my_forwarding_table[i];
		//split horizon and poison reverse
		//find cur_FTE->interface_uid  in the interface table, and if the interface->remote_VIP == cur_interface, poison reverse
		bool do_pr = false;

		for (int j = 0; j< my_interfaces.size(); j++){
			if (cur_FTE -> interface_uid == my_interfaces[j] -> unique_id ){
				if (in_addr_compare( (my_interfaces[j] -> remote_VIP_addr) , destination_VIP )){
					do_pr = true;
				}
			}
		}

		if (do_pr){
			//set cost = 16 and add to entries[]
			RIP_packet_tosend -> entries[i].cost = 16 ;

			//uint32_t temp_entry_addr = ntohl(inet_addr(( cur_FTE -> remote_VIP_addr ).c_str()));
			uint32_t temp_entry_addr = htonl((cur_FTE->remote_VIP_addr).s_addr);

			RIP_packet_tosend -> entries[i].address = temp_entry_addr  ;
		} else {
			RIP_packet_tosend -> entries[i].cost = cur_FTE -> cost ;

			uint32_t temp_entry_addr = htonl((cur_FTE->remote_VIP_addr).s_addr);

			RIP_packet_tosend -> entries[i].address = temp_entry_addr  ;
		}
	}

	RIP_packet_tosend -> num_entries = temp_num_entries;
	RIP_packet_tosend -> command = temp_command;

	return RIP_packet_tosend;
}

void* send_rip_response(void* a){
	//send stuff in your forwarding table to your neighbors every five seconds

	while(1){
		for (int i = 0; i < my_interfaces.size(); i++){
			interface* cur_interface = my_interfaces[i];

			RIP_packet* RIP_packet_tosend = create_RIP_packet(cur_interface);

			printf("created a RIP packet tosend;\n");

			//send response
			int t;
			t = send( (my_interfaces[i]->remote_VIP_addr), (char*) RIP_packet_tosend, sizeof(*RIP_packet_tosend),false,true);

		}
		sleep(5);
		printf("wake up");
	}
	return NULL;
}

void* clean_forwarding_table(void* a){
	/* periodically: check */
	//printf("in clean thread  \n");

	while(1){
		//printf("in clean thread while loop \n");

		for(int i =0; i< my_forwarding_table.size();i++){
			FTE* cur = my_forwarding_table[i];
			//printf("%s %d %d\n",cur->remote_VIP_addr.c_str(),cur->interface_uid,cur->cost);
			double seconds;
			time_t timer;

			time_t FTE_last_updated_time = cur->time_last_updated;
			timer = time(NULL);

			seconds = difftime(timer, FTE_last_updated_time);
			//printf("check sec \n");

			if(seconds >= 8 ){
				//delete from forwarding table
				//pthread_mutex_lock(&ft_lock);
				my_forwarding_table.erase(my_forwarding_table.begin()+i);
				//pthread_mutex_unlock(&ft_lock);

			}
		}
	}
	return NULL;
}


void* start_receive_service(void* a){

	struct sockaddr_in myaddr;      /* our address */
	struct sockaddr_in remaddr;     /* remote address */
	socklen_t addrlen = sizeof(remaddr);            /* length of addresses */
	int recvlen;                    /* # bytes received */

	unsigned char buf[RECEIVE_BUFSIZE];     /* receive buffer */

	/* create a UDP socket */

	if ((u_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("cannot create socket\n");
		return NULL;
	}

	/* bind the socket to any valid IP address and a specific port */

	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(my_port);

	if (bind(u_socket, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
		perror("bind failed");
		return NULL;
	}

	/* now loop, receiving data and printing what we received */
	while (1) {
		printf("waiting on port %d\n", my_port);
		recvlen = recvfrom(u_socket, buf, RECEIVE_BUFSIZE, 0, (struct sockaddr *)&remaddr, &addrlen);
		printf("received %d bytes\n", recvlen);
		if (recvlen > 0) {
			buf[recvlen] = 0;

			handle_packet((IP_packet*)&buf);

		}
	}
	return NULL;
}


/*          *****************      from project 0             ****************************        */


void read_in(){
	int line_count = 0;
	ifstream file(argv_in[1]);
	string line_buffer;

	while (getline(file, line_buffer)){
		if (line_buffer.length() == 0)continue;

		if(line_count == 0){

			my_port = atoi(line_buffer.substr(line_buffer.find(":")+1).c_str());
		}
		else{
			int temp_remote_port = atoi(line_buffer.substr(line_buffer.find(":")+1,line_buffer.find(" ")).c_str());
			line_buffer = line_buffer.substr(line_buffer.find(" ")+1);

			struct in_addr temp_my_VIP_addr;
			string temp_my_VIP_addr_str = line_buffer.substr(0,line_buffer.find(" "));
			inet_pton(AF_INET, temp_my_VIP_addr_str.c_str(), (void*)&temp_my_VIP_addr);

			line_buffer = line_buffer.substr(line_buffer.find(" ")+1);

			struct in_addr temp_remote_VIP_addr;
			string temp_remote_VIP_addr_str = line_buffer;
			inet_pton(AF_INET, temp_remote_VIP_addr_str.c_str(), (void*)&temp_remote_VIP_addr);

			interface* new_interface = new interface;

			new_interface -> unique_id = line_count;
			new_interface -> my_port = my_port;
			new_interface -> remote_port = temp_remote_port;
			new_interface -> my_VIP_addr = temp_my_VIP_addr;
			new_interface -> remote_VIP_addr = temp_remote_VIP_addr;
			new_interface -> status = 1;

			my_interfaces.push_back(new_interface);

			FTE* new_FTE = new FTE;

			new_FTE -> interface_uid = line_count;
			new_FTE -> remote_VIP_addr = temp_remote_VIP_addr;
			new_FTE -> cost = 1;
			new_FTE -> time_last_updated = time(NULL);  //set to current time

			//pthread_mutex_lock(&ft_lock);
			my_forwarding_table.push_back(new_FTE);
			//pthread_mutex_unlock(&ft_lock);

		}

		line_count ++;

	}

}

void* node (void* a){
	printf("in node interface\n");

	pthread_t clean_ft_thread;
	pthread_t send_rip_response_thread;
	pthread_t receive_service_thread;


	//read the input file
	read_in();

	//create server that can accept message from different nodes

	if(pthread_create(&receive_service_thread, NULL, start_receive_service, NULL)) {
		fprintf(stderr, "Error creating clean forwarding table thread\n");
		return NULL;
	}

	//create client when send message

	//send rip_request

	//start new thread: send_rip_response every 5 sec
	if(pthread_create(&send_rip_response_thread, NULL, send_rip_response, NULL)) {
		fprintf(stderr, "Error creating clean forwarding table thread\n");
		return NULL;
	}

	//start new thread: clean forwarding table every sec
	cout<< "\n start cleaning forwarding table" << endl;
	if(pthread_create(&clean_ft_thread, NULL, clean_forwarding_table, NULL)) {
		fprintf(stderr, "Error creating clean forwarding table thread\n");
		return NULL;
	}

	//triggered event: update_forwarding_table();




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

		char str_temp[50];
		inet_ntop(AF_INET, &(cur->my_VIP_addr), str_temp, INET_ADDRSTRLEN);
		printf("%d %s %s\n",cur->unique_id,str_temp,std);
	}
	return 0;
}

int routes(){
	for(int i =0; i< my_forwarding_table.size();i++){
		FTE* cur = my_forwarding_table[i];

		char str_temp[50];
		inet_ntop(AF_INET, &(cur->remote_VIP_addr), str_temp, INET_ADDRSTRLEN);

		printf("%s %d %d\n", str_temp ,cur->interface_uid,cur->cost);

	}
	return 0;
}

int up_interface(int interface_id){
	struct in_addr addr_to_add;
	bool found = false;

	for(int i =0; i< my_interfaces.size();i++){
		if(my_interfaces[i]->unique_id == interface_id){
			addr_to_add = my_interfaces[i]-> remote_VIP_addr;
			my_interfaces[i]->status = 1;
			printf("Interface %d up.\n", my_interfaces[i]->unique_id);
			found = true;
		}
	}
	if(found && (my_forwarding_table.size()<64 )){
		FTE* new_FTE = new FTE;
		new_FTE -> remote_VIP_addr = addr_to_add;
		new_FTE -> interface_uid = interface_id;
		new_FTE -> cost = 1;
		new_FTE -> time_last_updated = time(NULL);

		my_forwarding_table.push_back(new_FTE);
	}

	if(!found){
		printf("Interface %d not found.\n", interface_id);
	}
	return 0;
}

int down_interface(int interface_id){
	//TODO: Close the UDP socket
	//TODO: should also delete FTE from forwarding table
	struct in_addr addr_to_erase;

	bool found = false;

	for(int i =0; i< my_interfaces.size();i++){
		if(my_interfaces[i]->unique_id == interface_id){
			my_interfaces[i]->status = 0;
			addr_to_erase = my_interfaces[i]-> remote_VIP_addr;
			printf("Interface %d down.\n", my_interfaces[i]->unique_id);
			found = true;
		}
	}

	for (int j = 0; j< my_forwarding_table.size(); j++){
		//delete FTE from forwarding table if using this interface
		if (my_forwarding_table[j]->interface_uid == interface_id ){
			my_forwarding_table.erase(my_forwarding_table.begin()+j);
		}
	}

	if(!found) { printf("Interface %d not found.\n", interface_id); }
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
			routes();

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

			struct in_addr dest_addr;
			inet_pton(AF_INET, dest_ip, (void*)&dest_addr);

			t = strtok(NULL, "");
			printf("message :%s\n", t);
			char* to_send_msg = t;

			int len = strlen(to_send_msg);

			send(dest_addr,to_send_msg,len,false,false);

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
