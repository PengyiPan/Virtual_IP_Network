#define MAX_MSG_LENGTH (1400)
#define MAX_BACK_LOG (5)
#define RECEIVE_BUFSIZE (65535)
#define IP_PACKET_MAX_PAYLOAD (1336)




char** argv_in;
int u_socket;                         /* our socket */
pthread_mutex_t ft_lock;

bool init_finished = false;
bool update_table_changed = false;
