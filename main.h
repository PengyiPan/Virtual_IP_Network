#define MAX_MSG_LENGTH (1400)
#define MAX_BACK_LOG (5)
#define RECEIVE_BUFSIZE (65535)
#define IP_PACKET_MAX_PAYLOAD (1336)




char** argv_in;
int u_socket;                         /* our socket */
pthread_mutex_t ft_lock;

bool init_finished = false;
bool update_table_changed = false;

uint16_t my_port;

struct interface_t{
	uint16_t unique_id;
	uint16_t my_port;
	uint16_t remote_port;
	struct in_addr my_VIP_addr;
	struct in_addr remote_VIP_addr;
	int status; //1 is up and 0 is down
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





vector<interface*> my_interfaces(0);
vector<FTE*> my_forwarding_table(0);


int ifconfig();
int routes(bool print_ttl);
void update_forwarding_table(RIP_packet* RIP, struct in_addr new_next_hop_VIP_addr);
void RIP_packet_handler(RIP_packet* RIP, struct in_addr new_next_hop_VIP_addr);
struct RIP_packet* create_RIP_response_packet(interface* cur_interface);

bool in_addr_compare(struct in_addr addr1,struct in_addr addr2);

int send(struct in_addr des_VIP_addr,char* mes_to_send,int msg_length,bool msg_encapsulated,bool msg_is_RIP);

void triggered_RIP_response_sending();

void triggered_RIP_request_sending();

void forward_or_print(IP_packet* packet);

void handle_packet(IP_packet* ip_packet);

void merge_route(entry new_entry , int next_hop_interface_id, in_addr next_hop_VIP_addr);

void RIP_packet_handler(RIP_packet* RIP, struct in_addr new_next_hop_VIP_addr);

void update_forwarding_table(RIP_packet* RIP, struct in_addr new_next_hop_VIP_addr);

struct RIP_packet* create_RIP_response_packet(interface* cur_interface);

void* periodic_send_rip_response(void* a);

void* clean_forwarding_table(void* a);

void* start_receive_service(void* a);

void read_in();

void* node (void* a);

int ifconfig();

int routes(bool print_ttl);

int up_interface(int interface_id);

int down_interface(int interface_id);



