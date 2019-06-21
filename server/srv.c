#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define BUFFSIZE 1024
#define PORTNO 40226

int main(){
	struct sockaddr_in server_addr, client_addr;
	int socket_fd, client_fd;
	int len, len_out;
	char buf[BUFFSIZE];

	if((socket_fd = socket(PF_INET, SOCK_STREAM, 0))<0){
		printf("Server : Can't open stream socket.");
		return 0; 
	}

	int opt = 1; 
	setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

}