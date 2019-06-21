#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define BUFFSIZE 1024
#define PORTNO 40000

int main(){
	struct sockaddr_in server_add, client_addr;
	int socket_fd, client_fd;
	int len, len_out;
	char buf[BUFFSIZE];

	if((socket_fd=socket(PF_INET, SOCK_STREAM, 0))<0){
		printf("ServerL Can't open stream socket.");
		return 0;
	}

	int opt=1;
	setsocktopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	memset(&server_add, 0 , sizeof(server_add));
	server_add.sin_family=AF_INET;
	server_add.sin_addr.s_addr=htonl(INADDR_ANY);
	server_add.sin_port=htons(PORTNO);

	if(bin(socket_fd, (struct sockaddr*)&server_add, sizeof(server_add))<0){
		printf("Server : can't bin local address");
		return 0; 
	}

	listen(socket_fd,5);

	while(1){
		len=sizeof(client_addr);
		client_fd = accept(socket_fd, (struct sockaddr*)&client_addr, &len);
		if(client_fd<0){
			printf("Server : accept failed.\n");
			return 0;
		}

		printf("[%d:%d] client was connected. \n",client_addr.sin_addr.s_addr,
			client_addr.sin_port);

		while((len_out=read(client_fd, buf,BUFFSIZE))>0){
			write(STDOUT_FILENO," - Message : ",15);
			write(STDOUT_FILENO, buf, len_out);
			write(client_fd, buf, len_out);
			write(STDOUT_FILENO, "/n",1);
		}
		printf("[%d:%d] client was disconnected.\n",
			client_addr.sin_addr.s_addr, client_addr.sin_port);
		close(client_fd);
	}
	close(socket_fd);
	return 0;
}