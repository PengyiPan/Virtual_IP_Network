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





int main(int argc, char* argv[]){

//	char* str = "asdasdaf";
//	int size = strlen(str);
//	int sum = ip_sum(str,size);

	struct ip* header = new ip;

	struct in_addr addr1;

	char* in_addr1 = "192.168.0.4";
	char* in_addr2 = "4.0.168.192";

	// store this char* IP address in addr1:
	inet_pton(AF_INET, in_addr1, (void*)&addr1);

	//get str addr from in_addr addr1
	char str[50];
	inet_ntop(AF_INET, &addr1, str, INET_ADDRSTRLEN);
	printf("addr from in_addr: %s\n",str);


	struct in_addr addr2;
	inet_pton(AF_INET, in_addr2, (void*)&addr2);


	//strcmp: given two in_addr, put into two char[]. then compare
	char addr1_char[50];
	char addr2_char[50];
	inet_ntop(AF_INET, &addr1,  addr1_char, INET_ADDRSTRLEN);
	inet_ntop(AF_INET, &addr2,  addr2_char, INET_ADDRSTRLEN);

	printf("strcmp: %d\n",strcmp(addr1_char,addr2_char));


	//in_addr to uint32_t
	uint32_t addr_32 = htonl(addr1.s_addr);

	// uint32_t
	struct in_addr addr3;
	addr3.s_addr = ntohl(addr_32);

	//get addr out (same code as above)
	//get str addr from in_addr
	char str2[50];
	inet_ntop(AF_INET, &addr3, str2, INET_ADDRSTRLEN);
	printf("addr from uint32 convert back: %s\n",str2);

	header->ip_src = addr1;

	int sum = ip_sum((char*)header,sizeof(*header));

	printf("sum is: %d\n",sum);



}
