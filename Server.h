#include <string.h>
#include <stdlib.h>

#define attemp_max 10

struct cache
{
	int index;
	char state[10];
	int queue;
	int attempt;
	int time_out;
	char ip[16];
	char mac[18];
};

class server
{
private :
	char IP[16];
	char MAC[18];
	int cast;

public :
	cache table[10];
	server(int, char*, char*);
	void tableinit(void);
	int search_field(char*);
	int input(char* ip, server, server, server);
	int output(char*, server, server, server);
	void cache_update(int time);
	server search_obj(int, server, server, server); 
};

