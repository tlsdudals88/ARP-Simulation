#include "server.h"

server::server(int cast, char* ip, char* mac)
{
	this->cast = cast;
	strcpy_s(this->IP, ip);
	strcpy_s(this->MAC, mac);
	this->tableinit();
}

void server::tableinit(void)
{
	for(int i = 0 ; i < 10 ; i++)
	{
		this->table[i].index = i+1;
		this->table[i].attempt = 0;
		strcpy_s(this->table[i].ip, "\0");
		strcpy_s(this->table[i].mac, "\0");
		strcpy_s(this->table[i].state , "FREE\0");
		this->table[i].time_out = 0;
		this->table[i].queue = 0;

	}
}

int server::search_field(char* IP)
{
	for(int i = 0 ; i < 10 ; i++)
	{
		if(strcmp(this->table[i].ip , IP) == 0)
		{
			return this->table[i].index; // TRUE
		}
	}
	return 0; // FALSE
}


int server::output(char *IP, server a, server b, server c)
{
	int index;
	index = this->search_field(IP);
	int que;
	que = rand();
	que = que%10 +1;

	if(index == 0)
	{
		index = this->search_field("\0");
		this->table[index-1].attempt = 1;
		strcpy_s(this->table[index-1].ip, IP);
		strcpy_s(this->table[index-1].mac, "\0");
		strcpy_s(this->table[index-1].state , "PENDING\0");
		this->table[index-1].time_out = 0;
		this->table[index-1].queue = que;
	}
		return 1;
} // Broadcasting

int server::input(char* ip, server a, server b, server c)
{

	int index;
	index = this->search_field(ip);
	if(this->search_obj(index-1, a, b, c).cast !=0)
	{
		strcpy_s(this->table[index-1].state, "RESOLVED\0");
		this->table[index-1].time_out = 300;
		strcpy_s(this->table[index-1].mac, this->search_obj(index-1, a, b, c).MAC);
		return this->search_obj(index-1, a, b, c).cast;
	}
	return 0;

}


server server::search_obj(int i, server a, server b, server c)
{
	static server null(0, "\0", "\0");
	if(strcmp(this->table[i].ip, a.IP) == 0)
	{
		return a;
	}

	else if(strcmp(this->table[i].ip, b.IP) == 0)
	{
		return b;
	}

	else if(strcmp(this->table[i].ip, c.IP) == 0)
	{
		
		return c;
	}
	return null;
}

void server::cache_update(int time)
{
	for(int i = 0; i < 10 ; i++)
	{
		if(strcmp(this->table[i].state, "RESOLVED\0") == 0)
		{
			this->table[i].time_out = this->table[i].time_out - time;
			if(this->table[i].time_out == 0)
			{
				this->table[i].attempt = 0;
				strcpy_s(this->table[i].ip, "\0");
				strcpy_s(this->table[i].mac, "\0");
				strcpy_s(this->table[i].state , "FREE\0");
				this->table[i].time_out = 0;
				this->table[i].queue = 0;
			}
		}

		if(strcmp(this->table[i].state, "PENDING\0") == 0)
		{
			if(this->table[i].attempt == 5)
			{
				this->table[i].attempt = 0;
				strcpy_s(this->table[i].ip, "\0");
				strcpy_s(this->table[i].mac, "\0");
				strcpy_s(this->table[i].state, "FREE\0");
				this->table[i].time_out = 0;
				this->table[i].queue = 0;
				return;
			}

			this->table[i].attempt++;
		}

	}
}



			



		