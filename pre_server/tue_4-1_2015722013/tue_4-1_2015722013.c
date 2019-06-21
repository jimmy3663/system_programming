///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//File Name     : Preforked server 		 	                                                 					 //
//Date          : 2019/5/31                                                                                   	 //
//Os            : Ubuntu 16.04. LTS 64bits                                                                       //
//Author        : JIN SU LEE                                                                                     //
//Student ID    : 2015722013																					 //
//---------------------------------------------------------------------------------------------------------------//
//Title         : System Programming Assignment #4-1 (preforked server)            								 // 
//Discription 	: 이번 과제는 저번 과제의 연장 선으로써 미리 5개의 자식 프로제서를 만들어 놓은 상태에서 클라이언트의 request를 받는 것이다. //
//				  저번에는 request를 받을 떄 마다 fork 즉 자식 프로세스를 생성하여 request 처리를 해주었지만 이번에는 이미 있는 다섯개의 //
// 				  자식 프로세스에서 처리해 준다. 자식을 종료 시킬 떄는 자식이 아닌 부모에게 signal을 줘서 프로세서에서 자식들을 죽이는 형태를 //
//				  띈다. 																							 //
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
#define BUFFSIZE 1024
#define BUFSIZE 1024
#define PORTNO 40226
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

	int i=0;
	while(current!=NULL){
		printf("%d\t%s\t%d\t%d\t%s", i+1, current->ip, current->pi, current->port, current->time);
		current = current->next;
		i++;
	}

}

RList *list[5]; // Declare list as global varialbe 
static int maxNchildren; // the number of children that is pre forked. 
static pid_t *pids; // pre forked processor's pids
static char *buf; 

///////////////////////////////////////////////////////
//Signalhandler for set reaction to Signal 			 //
//===================================================//
//input : signal 									 //
//Output : if SIGALRM -> Print Connection history's  //
// 			title and give signal SIGUSR1 to all pids//
//		   if SIGCHLD -> wait for childern 			 //
// 		   if SIGUSR1 -> print list 				 //
//Purpose : to set reaction to signal (to change the //
// 			default action)							 //
/////////////////////////////////////////////////////// 
void signalHandler(int signal){
        int status;
        pid_t pid;

	if(signal == SIGALRM){ // if signal is signal alarm 
		printf("=========== Connection History =========\n");
		printf("NO.\tIP\t\tPID\tPORT\tTIME\n");
		for(int i=0; i<5; i++){
			kill(pids[i], SIGUSR1);
		}
	}

	if(signal == SIGUSR1){
		for(int i=0; i<5; i++){
                	RPrint(list[i]); // print list
                }
	}

	if(signal == SIGCHLD) // if signal is signal child 
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

////////////////////////////////////////////////////////
//Signal handler for SIGINT 						  //
//====================================================//
//Input : signal 									  //
//Output: if sigint -> kill forked preforked processor//
// 		  and print out which processeor is terminated//
//Purpose : to kill every forked processor when signal//
// 			comes in.								  //
////////////////////////////////////////////////////////
void sig_int(int signal){
	
	time_t now_time;
	time(&now_time);
	char *tim = ctime(&now_time);
	char mytime[24];
	for(int i=0; i<strlen(tim)-1 ; i++){
		mytime[i] = tim[i]; // to fit the format when printing 
	}
	for(int i=0; i<maxNchildren; i++){
		kill(pids[i],SIGTERM); // give signal SIGTERM every pids to kill 
		printf("\n[%s] %d process is terminated",mytime,(int)pids[i]); // print out which process is died. 
	}
	printf("\n");
	printf("[%s] Server is terminated\n",mytime);
	
	while(wait(NULL)>0)//parent wait for children to die and get information about children 
	
	exit(0);
}

void child_main(int i, int sockdet_fd, int addrlen, struct sockaddr_in client_addr,RList** list, pid_t pid); // predeclation for child_main


//////////////////////////////////////////////////////////////////////
//function for making child processeor 								//
//==================================================================//
//Input: i -> child number 											//
//. 	 socket_fd -> socket file descriptor 						//
//		 addrlen -> size of client address 							//
//		 client_addr -> client address 								//
//		 list -> linked list for child information 					//
//Output : fork process 											//
//Purpose : to make child process 									//
//////////////////////////////////////////////////////////////////////
pid_t child_make(int i, int socket_fd, int addrlen, struct sockaddr_in client_addr, RList** list){
	pid_t pid; 

	if((pid = fork())>0){ // making fork porcessor 
		return(pid);
	}

	child_main(i, socket_fd, addrlen, client_addr, list, pid); 
}


//////////////////////////////////////////////////////////////////////
//function for child processor to handle requeset 					//
//==================================================================//
//Input : Same as child_make() 										//
//Output : handle request from client 								//
//Purpose : to handle rqueset from client 							//		
//////////////////////////////////////////////////////////////////////
void child_main(int i, int socket_fd, int addrlen, struct sockaddr_in client_addr, RList** list, pid_t pid){
	int client_fd, len_out; // client file discriptor and len_out 
	char buf[BUFFSIZE]; 
	socklen_t clilen;
	
	time_t now_time=time(NULL); // get current time 
    char *tim = ctime(&now_time); // make current time to character form 
	char mytime[24]; 
	
	for(int i=0; i<24; i++){
		mytime[i] = tim[i]; // to get rid of ENTER at the end of ctime 
	}
	printf("[%s] %ld process is forked\n",mytime,(long)getpid());
	RStart(*list); // start list 

	while(1){
		clilen = addrlen; 
		client_fd = accept(socket_fd, (struct sockaddr*)&client_addr, &clilen); // client file descriptor accepts socket 
		if(client_fd<0){
			printf("server: accept failed\n");
			return;
		}

		struct in_addr inet_client_address; // client address 
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
		if(client_fd<0){
			printf("Server : accept failed\n");
			exit(0);
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
				exit(0);
				continue;
			}
		
			strcpy(tmp,buf);

			if(strcmp(url,"/favicon.ico")==0){ // if url is favicon.io 
				close(client_fd); // close client and continue
       		   	 	exit(0);
       		   		continue; 
       			}
			
			time_t in_time;
			time(&in_time);
			char *timi = ctime(&in_time);

            printf("======= New Client ======\n[%s]\n",mytime);// print format for new connection  
            printf("IP: %s \nPort : %d\n",inet_ntoa(inet_client_address),client_addr.sin_port);
            printf("=========================\n");

			//RInsert(list,inet_ntoa(inet_client_address), getpid(), client_addr.sin_port, tim); // insert client's infromation to list 

			if(pid==0){
				RInsert(*list,inet_ntoa(inet_client_address), getpid(), client_addr.sin_port, timi); // insert client's infromation to list 
				tok=strtok(tmp," ");
				strcpy(method, tok);
				if(strcmp(method, "GET")==0){
					tok = strtok(NULL," ");
					strcpy(url, tok);
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
		                    	sprintf(response_header, "HTTP/1.0 200 OK \r\n"
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
				printf("===== Disconnected Client =====\n[%s]\n",mytime); // output format of disconnected client 
                printf("IP: %s \nPort : %d\n",inet_ntoa(inet_client_address),client_addr.sin_port);
                printf("===============================\n\n");
				signal(SIGUSR1, signalHandler);
			}
		}
	}	
}

///////////////////////////////////////////////////////////////////////
//Main Function 													 //
//===================================================================//
//Input : socket_fd -> to get domain type, port number and binding 	 //
//						associated address 							 //
//		  client_fd -> to accept from server and to get information  //
// 		  				from server 								 //
//		  pids[i] -> return type storage of child_make function 	 //
//Output : print out new connection's ip address and port number 	 //
//		   print out disconnected client's ip address and port number//
///////////////////////////////////////////////////////////////////////
int main(){
	maxNchildren = 5;
	for(int i=0; i<maxNchildren; i++){
		list[i] = (RList*)malloc(sizeof(RList)); // make list 
	}
	struct sockaddr_in server_addr, client_addr;
	int socket_fd, client_fd;
	int len, len_out;
	int opt = 1;
	

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
		close(socket_fd); // close socket
		return 0;
	}

	listen(socket_fd, 5);// listen until client accept


	pids = (pid_t *)malloc(maxNchildren*sizeof(pid_t));
	int addrlen = sizeof(client_addr);

	time_t now_time;
    time(&now_time);
    char *tim = ctime(&now_time);
    char mytime[24];
    for(int i=0; i<strlen(tim)-1 ; i++){
        mytime[i] = tim[i];
    }

	printf("[%s] Server is started\n", mytime);

	for(int i=0; i<maxNchildren; i++){
		pids[i] = child_make(i, socket_fd, addrlen, client_addr, &list[i]); // get pid from child processor 
	}

	signal(SIGINT, sig_int); // signal handlfer for SIGINT
	signal(SIGALRM, signalHandler); // signal handler for SIGALRM 
	signal(SIGCHLD, signalHandler); // signalhandler for signal child 
	
	for(; ;){
		alarm(10); // do alarm for 10 sec 
		pause();
	}
	close(socket_fd); // close socket
	return 0;
}



