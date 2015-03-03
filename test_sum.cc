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

	char* in_addr = "192.168.0.0";

	// store this IP address in sa:
	inet_pton(AF_INET, in_addr, (void*)&addr);

	//addr = inet_addr(in_addr);

	header->ip_src = addr;

	int sum = ip_sum((char*)header,sizeof(*header));






//	string test_str("askldjghasjdhg");
//	int size = test_str.size();
//	int sum = ip_sum((char*)&test_str,size);

	//this doesn't work!!!!!


	printf("sum is: %d\n",sum);


}
