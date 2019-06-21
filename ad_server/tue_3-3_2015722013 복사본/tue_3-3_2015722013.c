//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//File Name     : advanced srv.c (main.c) 	                                                 					 //
//Date          : 2019/5/24                                                                                   	 //
//Os            : Ubuntu 16.04. LTS 64bits                                                                       //
//Author        : JIN SU LEE                                                                                     //
//Student ID    : 2015722013																					 //
//---------------------------------------------------------------------------------------------------------------//
//Title         : System Programming Assignment #3-3 (advanced server)            								 // 
//Discription 	: 이번 과제는 저번에 구현했던 소켓 프로그램을 다중 접속을 지원하는 프로그램을 만드는 과제다. fork() 함수를 통해 만드는데 자식//
//				  프로세서가 연결 되자 마자 끊기는 형태이다. 지금까지 접속되 었던 클라이언트의 ip 번호와 port 번호 그리고 접속 했던 시간 그리//
//				  몇개의 클라이언트가 접속이 되었는지에 대한 젒속 기록을 10초에 한번씩 출력 해준다. 총 10까지 출력 가능하다. 그 이후 부터는  //
// 				  최근 접속 되었던 클라이언트 정보 10개만 출력 해 준다. accessible.usr에 있는 ip 번호와 다른 ip로 접속을 시도 했을시 	 //
//				  access denied라는 문구를 띄워 준다.																	 //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                                                                                	 
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

#include "ht_ls.h"

#define URL_LEN 256
#define BUFSIZE 1024
#define PORTNO 40000
#define _GNU_SOURCE

//////////////////////////////////////////////////////////////////////
//Node for connection History 										//
//==================================================================//
//Input :  Ip, port number,  client's pid, connection time			//
//Purpose : to contain information about client that had been connec//
//			ted.													// 		
//////////////////////////////////////////////////////////////////////  
typedef struct RNODE{
	char *ip;  // node 의 이름 
	pid_t pi;
	int port;
	char *time;
	struct RNODE *next; // 다음 node를 가르키고 있는 포인터 
}RNode;


///////////////////////////////////////////////////////
//List for connection histroy 						 //
//===================================================//
//Input : list counter, list number of request 		 //
//Purpose :to connect nodes that contains information//
//			about client.							 //
/////////////////////////////////////////////////////// 
typedef struct RLIST{
	int cnt; // List의 갯수 
	int req;
	RNode *head; // List의 시작 지점을 가르키는 포인터 
}RList;


///////////////////////////////////////////////////////
//Initialize the list 								 //
//===================================================//
//Purpose : Initialize the list count to 0 			 //
// 			Initialize the list request number to 0	 //
//			Initialize the list head to NULL 		 //
///////////////////////////////////////////////////////
void RStart(RList *list){
	list->cnt=0; // list 의 갯수를 0으로 초기화 한다. 
	list->req=0;
	list->head=NULL; // list 의 첫번째 노드를 NULL로 초기화 한다.
}

//////////////////////////////////////////////////////////////////////
//Insert node to List 												//
//==================================================================//
//Input : linked list, ip number, port number, pid and time 		//
//Purpose : to insert node that contaions client information to list//
//			Insert new node into Head of the list 					//
////////////////////////////////////////////////////////////////////// 
int RInsert(RList *list, char *ipn, pid_t p, int po, char *ti){
	RNode *new = (RNode *)malloc(sizeof(RNode));
	RNode *current = NULL;
	RNode *prev = NULL;

	new -> ip = ipn; // ip number 
	new -> pi = p; // pid 
	new -> port = po; // port number 
	new -> time = ti; // client connection time 
	new -> next = NULL; // next is null 

	if(list->head != NULL){ // if head of the list is not null 
		new->next = list->head; // next of new node is head of the list 
	}
	list->head = new; // new node becomes head of the list 
	list->cnt++; 
	list->req++; 
	
	if(list->cnt > 10){ // if list counter is bigger than 10 
		current = list->head;
		while(current->next != NULL){
			prev = current;
			current = current->next;
		}
		prev->next = NULL; // next of the tail of the list is null 
		free(current); // free node which is tail of the list 
		list->cnt--;// discount the count 
	}
	current = list->head;
	while(current!=NULL){
		current=current->next;
	}
	
	return 0;
}

///////////////////////////////////////////////////////
//Print list which connects the node 				 //
//===================================================//
//Input: list 										 //
//Output : print the all the information which is in //
//		   the node 								 //
//Purpose : print the list from the head of the list //
///////////////////////////////////////////////////////
void RPrint(RList *list){
	RNode *current = list->head; // from head of the list 

	printf("=========== Connection History =========\n");
	printf("number of requeste(s): %d\n", list->req);
	printf("NO.\tIP\t\tPID\tPORT\tTIME\n");
	
	int i=0;
	while(current!=NULL){
		printf("%d\t%s\t%d\t%d\t%s", i+1, current->ip, current->pi, current->port, current->time);
		current = current->next;
		i++;
	}
	printf("\n\n");

}

RList *list; // Declare list as global varialbe 

///////////////////////////////////////////////////////
//Signalhandler for set reaction to Signal 			 //
//===================================================//
//input : signal 									 //
//Output : if SIGALRM -> Print list and wait for 10  //
//			sec 									 //
//		   if SIGCHLD -> wait for childern 			 //
//Purpose : to set reaction to signal (to change the //
// 			default action)							 //
/////////////////////////////////////////////////////// 
void signalHandler(int signal){
        int status;
        pid_t pid;
	if(signal == SIGALRM){ // if signal is signal alarm 
		RPrint(list); // print list 
		alarm(10); // wait for 10 second 
	}
	
	else if(signal == SIGCHLD) // if signal is signal child 
		waitpid(-1, &status, WNOHANG); // wait for child processor 
}

///////////////////////////////////////////////////////
//verify the ip address 							 //
//===================================================//
//Input : sockaddr_in structure 					 //
//Output : if match -> 1, if not 0 					 //
//Purpose : to verifiy the ip address in "accessibe. //
//			usr." 									 //
///////////////////////////////////////////////////////
int Ipaddr(struct sockaddr_in client_addr){
	FILE *ifile; 
	struct in_addr inet_client_address;
	inet_client_address.s_addr = client_addr.sin_addr.s_addr; // get client's ip address 
	
	ifile = fopen("accessible.usr", "r"); // open accessible.usr file 
	if(ifile==NULL){
		printf("cannot open file\n");
	}

	char fbuf[20] = {0, };
	while(fgets(fbuf, 20, ifile)!=NULL){ // get string line by line 
		for(int i=0; i<20 ; i++){
			if(fbuf[i] <33){ // when getting the line it also gets 'enter'
				fbuf[i]='\0'; //so if not string set as NULL 
			}
		}
		if(fnmatch(fbuf,inet_ntoa(inet_client_address),0)==0){ // match the string and client's ip address
			fclose(ifile);
			return 1;
		}
	}
	fclose(ifile);
	return 0;
}
	
///////////////////////////////////////////////////////
//Function deny										 //
//===================================================//
//Input : client's address, client_fd 				 //
//Ouput : print accessible deny on web page 		 //
//Purpose : if Ipaddr fucntion returns 0 then print  //
// 			accessible deny on web page 			 //
/////////////////////////////////////////////////////// 
void deny(struct sockaddr_in client_addr, int client_fd){
	FILE *dfile;
	struct in_addr inet_client_address; 
	char response_message[BUFSIZE];
	char response_header[BUFSIZE];
	inet_client_address.s_addr = client_addr.sin_addr.s_addr; // get client's ip address 

	sprintf(response_message,
			"<h1>ACCESS Denied!</h1>"
			"<h1>Your IP : %s</h1>"
			"You have no permission to access this web server<br>"
			"HTTTP 403.6 Forbidden IP address reject<br>"
			, inet_ntoa(inet_client_address)); // print format of accessible deny 
		sprintf(response_header,
			"HTTP/1.0 200 OK \r\n"
			"Server: 2019 simple web server\r\n"
			"Content-length: %lu\r\n"
			"Content-type: text/html\r\n\r\n",strlen(response_message));
	write(client_fd, response_header, strlen(response_header)); // write response header on client_fd 
	write(client_fd, response_message, strlen(response_message)); // write response message on client_fd 
}

//////////////////////////////////////////////////////////////////////
//Main Function 													//
//==================================================================//
//Input : socket_fd -> to get domain type, port number and binding 	//
//						associated address 							//
//		  client_fd -> to accept from server and to get information //
// 		  				from server 								//
//		  in fork -> manage the client and disconnect client 		//
//Output : print out new connection's ip address and port number 	//
//		   print out disconnected client's ip address and port number//
//////////////////////////////////////////////////////////////////////
int main(){
	list = (RList*)malloc(sizeof(RList)); // make list 
	signal(SIGALRM, signalHandler); // signal handler for alarm 
	struct sockaddr_in server_addr, client_addr;
	int socket_fd, client_fd;
	int len, len_out;
	int opt = 1;
	DIR *dirp;
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
	RStart(list);
	alarm(10); // set alarm at 10 second 
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
		if(Ipaddr(client_addr)==0){ // if ip address doesn't match 
                	deny(client_addr, client_fd); // execute function deny 
			close(client_fd); // close client 
			break; // break 
        	}
		if(strcmp(inet_ntoa(inet_client_address), "0.0.0.0")!=0){ // continue if ip address is '0.0.0.0'
			if(read(client_fd, buf, BUFSIZE) <= 0){
				close(client_fd);
				continue;
			}
			strcpy(tmp,buf);

		 if(strcmp(url,"/favicon.ico")==0){ // if url is favicon.io 
			close(client_fd); // close client and continue
                        continue; 
                 }

	        int status;
	        pid_t pid;
		printf("======= New Client ======\n");// print format for new connection  
	        printf("IP: %s \nPort : %d\n",inet_ntoa(inet_client_address),client_addr.sin_port);
		printf("=========================\n");
		//RStart(list);
		time_t now_time;
		time(&now_time);
		char *tim = ctime(&now_time); // get current time for client connection time 
		RInsert(list,inet_ntoa(inet_client_address), getpid(), client_addr.sin_port, tim); // insert client's infromation to list 
	        if((pid=fork())==0){ // in child processor handle the connection 
				//close(socket_fd);
				tok=strtok(tmp," ");
				strcpy(method, tok);
				if(strcmp(method, "GET")==0){
					tok = strtok(NULL," ");
					strcpy(url, tok);
				}	

				if(strcmp(url,"/favicon.ico")==0){
                        		close(client_fd);
                        		//continue;
					exit(0);
                 		}	
				int func=1;

				
				#define	FNM_CASEFOLD	(1 << 4)// for using FNM_CASEFOLD
				if(fnmatch("*.txt",url, 0)==0||fnmatch("*.c",url, 0)==0||fnmatch("*.h",url, 0)==0){// if file is text file
					strcpy(mime,"text/plain");
					func=1;
				}
				else if(fnmatch("*.html", url, 0)==0){// if file is html file 
					strcpy(mime, "text/html");
					func=0;
				}
				else if(fnmatch("*.jpg",url,FNM_CASEFOLD)==0||fnmatch("*.png",url,FNM_CASEFOLD)==0||fnmatch("*.jpeg",url,FNM_CASEFOLD)==0){
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
					//continue;
					exit(0);
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
				memset(mime,0,strlen(mime)); // reset mime type 
				fclose(file); // close file
				close(client_fd); // close client
				exit(0);
			}
			else{ // in parent connextion 
				signal(SIGCHLD, signalHandler); // signalhandler for signal child 
				printf("===== Disconnected Client =====\n"); // output format of disconnected client 
	       			printf("IP: %s \nPort : %d\n",inet_ntoa(inet_client_address),client_addr.sin_port);
	  	     		printf("===============================\n\n");
				//sleep(10);
			}
		}
		else {
			continue;
		}
	}

	close(socket_fd); // close socket
	return 0;
}


