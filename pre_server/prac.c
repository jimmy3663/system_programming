#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define PORTNO 40000
#define BUFFSIZE 1024

static int maxNchildren;
static pid_t *pids;
static char *buf;

pid_t child_make(int i, int socketfd, int addrlen, struct sockaddr_in client_addr);
void child_main(int i, int socketfd, int addrlen, struct sockaddr_in client_addr);

int main(int argc, char *argv[]){
	struct sockaddr_in server_addr, client_addr;
	int socketfd, addrlen, i, opt = 1;

	buf = (char*)malloc(BUFFSIZE+1);

	if((socketfd = socket(PF_INET, SOCK_STREAM, 0))<0){
		printf("server cannot open stream socket\n");
		return 0;
	}

	setsockopt(socketfd,SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));// get socket information

	bzero((char*)&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;// to get address family 
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);// to get ip address 
	server_addr.sin_port = htons(PORTNO);// to get port number 

	if(bind(socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr))<0){// binnding socket with socket information
		printf("Server : Can't bind local address\n");
		return 0;
	}
	listen(socketfd, 5);

	maxNchildren = 5;
	pids = (pid_t *)malloc(maxNchildren*sizeof(pid_t));
	addrlen = sizeof(client_addr);

	for(int i=0; i<maxNchildren; i++){
		pids[i] = child_make(i, socketfd, addrlen, client_addr);
	}

	for(; ;)
		pause();

}

pid_t child_make(int i, int socketfd, int addrlen, struct sockaddr_in client_addr){
	pid_t pid; 

	if((pid = fork())>0){
		return(pid);
	}

	child_main(i, socketfd, addrlen, client_addr);
}

void child_main(int i, int socketfd, int addrlen, struct sockaddr_in client_addr){
	int client_fd, len_out;
	char buf[BUFFSIZE];
	socklen_t clilen;

	//client_addr = (struct sockaddr*)malloc(addrlen);

	printf("child %ld starting\n", (long)getpid());

	while(1){
		clilen = addrlen;
		client_fd = accept(socketfd, (struct sockaddr*)&client_addr, &clilen);
		if(client_fd<0){
			printf("server: accept failed\n");
			return;
		}

		while((len_out=read(client_fd, buf, BUFFSIZE))>0){
			write(STDOUT_FILENO, buf, len_out);
			write(client_fd, buf, len_out);
			printf("child %ld processing request\n\n",(long)getpid());
		}
		close(client_fd);
	}
}