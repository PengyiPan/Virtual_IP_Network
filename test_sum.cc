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

	struct in_addr addr;

	char* in_addr = "192.168.0.4";
	char* in_addr2 = "4.0.168.192";

	// store this IP address in sa:
	inet_pton(AF_INET, in_addr, (void*)&addr);

	//get str addr from in_addr
	char str[50];
	inet_ntop(AF_INET, &addr, str, INET_ADDRSTRLEN);
	printf("addr from in_addr: %s\n",str);

	//strcmp
	struct in_addr addr2;
	inet_pton(AF_INET, in_addr2, (void*)&addr2);

	char addr1_char[50];
	char addr2_char[50];
	inet_ntop(AF_INET, &addr,  addr1_char, INET_ADDRSTRLEN);
	inet_ntop(AF_INET, &addr2,  addr2_char, INET_ADDRSTRLEN);

	printf("strcmp: %d\n",strcmp(addr1_char,addr2_char));

	//to uinit32 and back
	uint32_t addr_32 = htonl(addr.s_addr);
	struct in_addr addr3;
	addr3.s_addr = ntohl(addr_32);
	//get addr out (same code as above)
	//get str addr from in_addr
	char str2[50];
	inet_ntop(AF_INET, &addr3, str2, INET_ADDRSTRLEN);
	printf("addr from uint32 convert back: %s\n",str2);

	header->ip_src = addr;

	int sum = ip_sum((char*)header,sizeof(*header));

	printf("sum is: %d\n",sum);



}
