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

using namespace std;

// max payload  = MTU(set by user) - header size
#define IP_PACKET_MAX_PAYLOAD (1336)

struct IP_packet {
	struct ip ip_header;
	char msg[IP_PACKET_MAX_PAYLOAD];
};


vector<IP_packet*> message_to_be_assembly(0);

vector< vector<IP_packet*> >message_pool (0);  // size = number of ID s.

void fragmentation(char msg[],int msg_length){

	int num_of_packet = msg_length/IP_PACKET_MAX_PAYLOAD;
	if(msg_length%IP_PACKET_MAX_PAYLOAD != 0) num_of_packet++;

	int offset = 0;

	for(int i=0;i<num_of_packet;i++){

		struct IP_packet* ip_packet_to_send = new IP_packet;
		struct ip* ip_header = new ip;

		int temp_offset;

		if ((msg_length - offset) < IP_PACKET_MAX_PAYLOAD){
			temp_offset = msg_length - offset;
		}else{
			temp_offset = IP_MF + offset;
			offset += IP_PACKET_MAX_PAYLOAD;
		}


		ip_header->ip_p = 0;
		ip_header->ip_ttl = 16;
		ip_header->ip_id = 1;
		ip_header->ip_off = IP_MF + temp_offset; //dont know how to add flag

		ip_header->ip_sum = 0;
		ip_header->ip_sum = htons(ip_sum((char*)ip_header, sizeof(struct ip)));

		memcpy(&(ip_packet_to_send->ip_header),ip_header,sizeof(struct ip));

		memcpy(&(ip_packet_to_send->msg),msg,msg_length);

		//
	}


}

void reassembly(){
	// check id to know which message this fragments belong to

	// put the fragments into the corresponding id's pool

	// check whether you have received all fragments needed

	// check whether time to live < 0, if yes, drop all fragments in this pool

	// check more_fragment bit, if no more, assembly


}

void* clean_message_pool(void* a){
	// loop through vector< vector<IP_packet*> >message_pool,
	//clear the space if expires

}

int main(int argc, char* argv[]){

	int test_msg_length = 5000;
	char msg[65535];

	for(int i = 0;i < test_msg_length;i++){
		msg[i] = 'a';
	}

	int msg_length = strlen(msg);

	printf("Test msg length is %d bytes\n",msg_length);

	if(msg_length > IP_PACKET_MAX_PAYLOAD){
		fragmentation(msg,msg_length);
	}

	pthread_t clean_thread;

	if(pthread_create(&clean_thread, NULL, clean_message_pool, NULL)) {
		fprintf(stderr, "Error creating clean thread\n");
		return 0;
	}





}
