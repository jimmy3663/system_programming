#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#include "ht_ls.h"

#define URL_LEN 256
#define BUFSIZE 1024
#define PORTNO 40000
#define _GNU_SOURCE

typedef struct RNODE{
	char *ip;  // node 의 이름 
	int no;
	int pi;
	int port;
	int time;
	struct NODE *next; // 다음 node를 가르키고 있는 포인터 
}RNode;


typedef struct RLIST{
	int cnt; // List의 갯수 
	int req;
	Node *head; // List의 시작 지점을 가르키는 포인터 
}RList;

void RStart(RList *list){
	list->cnt=0; // list 의 갯수를 0으로 초기화 한다. 
	list->req=;
	list->head=NULL; // list 의 첫번째 노드를 NULL로 초기화 한다.
}

int RInsert(RList *list, char *ipn, int num, int p, int po, int ti){
	RNode *new = (Node *)malloc(sizeof(Node));
	RNode *current = NULL;
	RNode *prev = NULL;

	new -> ip = ipn;
	new -> no = num;
	new -> pi = p;
	new -> port = po;
	new -> time = ti;
	list->req = num;

	if(list->Head == NULL){
		list->Head = new;
	}
	else{
		current = list->Head;
		if(cnt > 10){
			int min=current->no;
			while(current!=NULL){
				if(min> current->no){
					min = current->no;
				}
				prev = current;
				current = current->next;
			}

			while(current!=NULL){
				if(current->no == min){
					prev -> next = new;
					new -> next = current->next;
				}
				prev = current;
				current=current->next;
			}
		}
		else{
			while(current!=NULL){
				prev = current;
				current=current->next;
			}
			if(current == NULL)
				prev -> next = current;
		}
	}

	list->cnt++:
	return 0;
}

void RPrint(List *list){
	RNode *current = list->Head;

	printf("=========== Connection History =========\n")
	printf("number of requeste(s): %d\n", list->req);
	printtf("NO.\tIP\tPID\tPORT\tTIME\n");
	while(current!=NULL){
		printf("%d\t%s\t%d\t%d\t%d\n", current->no, current->ip, current->pi, current->port, current->time);
		current=current->next;
	}

}

int main(){
	struct sockaddr_in server_addr, client_addr;
	int socket_fd, client_fd;
	int len, len_out;
	int opt = 1;

	struct dirent *dir;
	if((socket_fd = socket(PF_INET, SOCK_STREAM,0))<0){ // to create socket
		printf("Server : Can't open stream socket\n");
		return 0; 
	}

	setsockopt(socket_fd,SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));// get socket information

	bzero((char*)&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;// to get address family 
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);// to get ip address 
	server_addr.sin_port = htons(PORTNO);// to get port number 

	if(bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr))<0){// binnding socket with socket information
		printf("Server : Can't bind local address\n");
		return 0;
	}
	listen(socket_fd, 5);// listen until client accept


//////////////////////////////// accepting until user input ^c /////////////////////////
	int cnt = 1;
	while(1){
		struct in_addr inet_client_address;
		char buf[BUFSIZE] = {0, };// for client_fd buffer 
		char tmp[BUFSIZE] = {0, };
		char response_header[BUFSIZE] = {0, };
		char *response_message = NULL;
		char url[URL_LEN] = {0, };
		char method[20] = {0, };
		char *tok =NULL;
		char root[1024];
		char *fbuf;
		char cur[1024]={0,};
		char mime[255]={0,};
		len = sizeof(client_addr);
		client_fd= accept(socket_fd, (struct sockaddr*)&client_addr, &len);
		if(client_fd<0){
			printf("Server : accept failed\n");
			return 0;
		}

		inet_client_address.s_addr = client_addr.sin_addr.s_addr;
		if(strcmp(inet_ntoa(inet_client_address), "0.0.0.0")!=0){
			if(read(client_fd, buf, BUFSIZE) <= 0){
				printf("Buffer : %s\n", buf);
				close(client_fd);
				continue;
			}


			strcpy(tmp,buf);
			printf("cnt : %d\n", cnt);
			printf("======= New Client ======\n");
	        printf("IP: %s \nPort : %d\n",inet_ntoa(inet_client_address),client_addr.sin_port);
	        printf("=========================\n\n");

	        int status;
	        pid_t pid;
	        if((pid=fork())!=0){

				tok=strtok(tmp," ");
				strcpy(method, tok);
				if(strcmp(method, "GET")==0){
					tok = strtok(NULL," ");
					strcpy(url, tok);
				}	


				int func=1;



				if(strcmp(url,"/favicon.ico")==0){
					close(client_fd);
					continue;
				}

				
				#define	FNM_CASEFOLD	(1 << 4)// for using FNM_CASEFOLD
				if(fnmatch("*.txt",url, 0)==0||fnmatch("*.c",url, 0)==0||fnmatch("*.h",url, 0)==0){// if file is text file
					printf(".txt\n");
					strcpy(mime,"text/plain");
					func=1;
				}
				else if(fnmatch("*.html", url, 0)==0){// if file is html file 
					printf(".html\n");
					strcpy(mime, "text/html");
					func=0;
				}
				else if(fnmatch("*.jpg",url,FNM_CASEFOLD)==0||fnmatch("*.png",url,FNM_CASEFOLD)==0||fnmatch("*.jpeg",url,FNM_CASEFOLD)==0){
					printf("image\n");
					strcpy(mime,"image/*");
					func=2;
				}



				int filesize;
				FILE *file;

				ht_ls(url); 
				file = fopen("html_ls.html","r"); // open html file 
				if(file==NULL){ // if can't open continue
					printf("file error\n");
					close(client_fd);
					continue;
				}

		        fseek(file, 0, SEEK_END);
		        filesize= ftell(file); // filesize of html file 
		        fseek(file, 0, SEEK_SET);

				response_message = (char*)malloc(sizeof(char)*(filesize+1));// dynamic allocation sizeof filesize
				memset(response_message, 0 ,filesize+1); // reset response message 
				if(fread(response_message,1, filesize, file)<0){// get infromation from html file 
					printf("error\n");
				}
				

				if(func==1){
					if(fread(response_message,1, filesize, file)<0){
		                                printf("error\n");
		                       	} // read file by response message 
					sprintf(response_header,
						"HTTP/1.0 200 OK \r\n"
						"Server: 2019 simple web server\r\n"
						"Content-length: %d\r\n"
						"Content-type: %s\r\n\r\n",filesize,mime);
					write(client_fd, response_header, strlen(response_header));
					write(client_fd, response_message, filesize);
				}
				else if(func==0){
					printf("func==0\n");
					sprintf(response_header,
		                                "HTTP/1.0 200 OK \r\n"
		                                "Server: 2019 simple web server\r\n"
		                                "Content-length: %lu\r\n"
		                                "Content-type: text/html\r\n\r\n",strlen(response_message));
		                        write(client_fd, response_header, strlen(response_header));
		                        write(client_fd, response_message, strlen(response_message));
				}
				else if(func==2){
					if(fread(response_message,1, filesize, file)<0){
		                    printf("error\n");
		          	}
		                        sprintf(response_header,
		                                "HTTP/1.0 200 OK \r\n"
		                                "Server: 2019 simple web server\r\n"
		                                "Content-length: %d\r\n"
		                                "Content-type: %s\r\n\r\n",filesize,mime);
		                        write(client_fd, response_header, strlen(response_header));//strlen(response_header));
		                        write(client_fd, response_message, filesize);//strlen(response_message));
		        }


				free(response_message);//free dynamic allocation
				//printf("[%s:%d] client was disconnected\n",inet_ntoa(inet_client_address),client_addr.sin_port);
				memset(mime,0,strlen(mime)); // reset mime type 
				fclose(file); // close file 
				printf("here? %d\n", cnt);
				wait(&status);
				// close client
			}
			else if((pid=fork())==0){
				close(client_fd); 
				printf("===== Disconnected Client =====\n");
	       		printf("IP: %s \nPort : %d\n",inet_ntoa(inet_client_address),client_addr.sin_port);
	       		printf("===============================\n\n");
	       		cnt++;
			}
		}
		else {
			continue;
		}
	}

	close(socket_fd); // close socket
	return 0;
}
