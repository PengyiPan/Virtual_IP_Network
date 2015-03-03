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

	// store this IP address in sa:
	inet_pton(AF_INET, in_addr, (void*)&addr);


	//get it back
	char str[50];

	inet_ntop(AF_INET, &addr, str, INET_ADDRSTRLEN);

	header->ip_src = addr;

	int sum = ip_sum((char*)header,sizeof(*header));

	printf("sum is: %d\n",sum);
	printf("addr back: %s\n",str);


}
