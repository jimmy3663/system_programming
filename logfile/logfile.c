///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//File Name     : tue_4-3_2015722013.c	 	                                                 					 //
//Date          : 2019/6/14                                                                                   	 //
//Os            : Ubuntu 16.04. LTS 64bits                                                                       //
//Author        : JIN SU LEE                                                                                     //
//Student ID    : 2015722013																					 //
//---------------------------------------------------------------------------------------------------------------//
//Title         : System Programming Assignment #4-3 (Log file and semaphore)     								 // 
//Discription 	: 이번 과제는 저번 과제의 연장선으로 server_log.txt라는 파일에 콘솔에 출력 되었던 log를 파일에 출력하는 것이다. 여러 프로 //
//					세서에서 한 file을 접근 해야 해서 semaphore를 필연적으로 사용해야 한다. 									 //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <semaphore.h>                                                                                	 
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
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "ht_ls.h"

#define URL_LEN 256
#define BUFFSIZE 1024
#define BUFSIZE 1024
#define PORTNO 40226
#define _GNU_SOURCE
#define KEY_NUM 40226
#define MEM_SIZE 700000

pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER; // initialize mutex 

static int idlecnt;
static int loopi=0;
static int maxNchildren; // the number of children that is pre forked. 
static pid_t *pids; // pre forked processor's pids
static char *buf;
static int MaxChilds; // number of max children 
static int MaxIdleNum; // idle count shouldn't be bigger than max idle number 
static int MinIdleNum; // idle count shouldn't be smaller than min idle number 
static int StartServers; // number of preforked processor 
static int MaxHistory; // number of history that can be printed 
static pid_t *pids2; 
sem_t *mysem;
const char *server_log = "server_log.txt";

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

	if((list)->head != NULL){ // if head of the list is not null 
		new->next = (list)->head; // next of new node is head of the list 
	}
	(list)->head = new; // new node becomes head of the list 
	(list)->cnt++; 
	(list)->req++; 
	
	if((list)->cnt > 10){ // if list counter is bigger than 10 
		current = (list)->head;
		while(current->next != NULL){
			prev = current;
			current = current->next;
		}
		prev->next = NULL; // next of the tail of the list is null 
		free(current); // free node which is tail of the list 
		(list)->cnt--;// discount the count 
	}
	current = (list)->head;
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
void RPrint(RList *list, void *shm_addr){
	RNode *current = (list)->head; // from head of the list 
	int i=0;
	while(current!=NULL){
		sprintf((char*)shm_addr,"%d\t%s\t%d\t%d\t%s", i+1, current->ip, current->pi, current->port, current->time);
		current = current->next;
		i++;
	}
	printf("%s",(char*)shm_addr);

}

RList *list[10]; // Declare list as global varialbe 

///////////////////////////////////////////////////////
//doit1 function for thread using shared memory 	 //
//===================================================//
//Input : vptr -> null 								 //
//Output : increase counter in shared memory 		 //
//Purpose : to care shared memory using lock and 	 //
//			unlock 									 //
///////////////////////////////////////////////////////
void *doit1(void *vptr){
	int i, val, shm_id;
	void *shm_addr;
	if((shm_id = shmget((key_t)KEY_NUM, MEM_SIZE, IPC_CREAT|0666))==-1){
		printf("shmget fail\n");
		return NULL;
	}
	if((shm_addr = shmat(shm_id, (void*)0, 0)) == (void*)-1){
		printf("shmat fail\n");
		return NULL;
	}
	pthread_mutex_lock(&counter_mutex);
	*((int*)shm_addr) += 1;
	pthread_mutex_unlock(&counter_mutex);
	idlecnt = *((int*)shm_addr);
	return NULL;
}

///////////////////////////////////////////////////////
//Signalhandler for set reaction to Signal 	    	 //
//===================================================//
//input : signal 									 //
//Output : if SIGALRM -> Print Connection history    //
// 		   if SIGUSR2 -> print idle counter  		 //
//Purpose : to set reaction to signal (to change the //
// 			default action)							 //
/////////////////////////////////////////////////////// 
void signalHandler(int signal){
    int status, shm_id;
    pid_t pid;
	void *shm_addr;
	FILE *logfile; // open file for server_log.txt 
 	if((shm_id=shmget((key_t)KEY_NUM, MEM_SIZE, IPC_CREAT|066))==-1){ // create shared memory 
                printf("shmget fail\n");
		exit(0);
             
        }
	if((shm_addr = shmat(shm_id, (void*)0, 0))==(void*)-1){ // combine shared memory as processor's own memory 
		printf("shmat fail\n");
		exit(0);
	}
	if(signal == SIGALRM){ // if signal is signal alarm 
		printf("=========== Connection History =========\n");
		printf("NO.\tIP\t\tPID\tPORT\tTIME\n");
		logfile = fopen(server_log, "a"); // open logfile in appen mode 
		fprintf(logfile,"=========== Connection History =========\n");
        fprintf(logfile,"NO.\tIP\t\tPID\tPORT\tTIME\n"); // print on logfile 
		int cnt=1; //counter for history number 
		for(int i=MaxHistory-1; i>=0; i--){
			if(strlen(((char*)shm_addr+4+i*1000))>10){ // if shared memory's has value 
				printf("%d\t%s",cnt ,((char*)shm_addr+4+i*1000)); // print shared memory's value 
				fprintf(logfile,"%d\t%s",cnt, ((char*)shm_addr+4+i*1000)); // print on logfile 
				cnt++;
			}
		}
		fclose(logfile);
	}

	if(signal == SIGUSR1){
		int l =0;
		while(pids[l] != (pid_t)0){
			l++;
		}
		//printf("in sigusr1 l :%d\n", l);
		for(int i=0; i<MaxChilds; i++){
                	RPrint(list[i], shm_addr+4+i*500); // print list
                }
	}

	if(signal == SIGUSR2){ // if signal is signal child 
		time_t now_time=time(NULL); // get current time 
		char *tim = ctime(&now_time); // make current time to character form 
		char mytime[24]; 
	
		for(int i=0; i<24; i++){
			mytime[i] = tim[i]; // to get rid of ENTER at the end of ctime 
		}
		sem_t *mysem = sem_open("PORTNO", O_RDWR); // open semaphore file in mode of read write 
		sem_wait(mysem); // lock semaphore 
		pthread_mutex_lock(&counter_mutex); // lock shared memory not to use from other processor 
		logfile = fopen(server_log, "a"); // open logfile in append. mode 
		printf("[%s] idlecount: %d\n",mytime, *((int*)shm_addr)); // print shared memory's counter 	
		fprintf(logfile,"[%s] idlecount: %d\n",mytime, *((int*)shm_addr)); // print shared memory's counter 	
		pthread_mutex_unlock(&counter_mutex); // unlock shared memory to be used by other processor 
		fclose(logfile);// close logfile 
		sem_post(mysem); // unlock semaphore file 
		sem_close(mysem); // close semaphore file 
	}			
}

///////////////////////////////////////////////////////
//verify the ip address 			  				 //
//===================================================//
//Input : sockaddr_in structure 		     		 //
//Output : if match -> 1, if not 0 		     		 //
//Purpose : to verifiy the ip address in "accessibe. //
//	usr." 		       			     				 //
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
//Signal handler for SIGINT 			    		  //
//====================================================//
//Input : signal 				      				  //
//Output: if sigint -> kill forked preforked processor//
// 	and print out which processeor is terminated  	  //
//		and delete shared memory 					  //
//Purpose : to kill every forked processor when signal//
// 			comes in.		      					  //
////////////////////////////////////////////////////////
void sig_int(int signal){
	int shm_id; 
	void *shm_addr;
	pthread_t tid;
	FILE *logfile; 
	if((shm_id=shmget((key_t)KEY_NUM, MEM_SIZE, IPC_CREAT|066))==-1){
                printf("shmget fail\n");
                exit(0);
    }
	if((shm_addr = shmat(shm_id, (void*)0, 0)) == (void*)-1){
		printf("shmat fail\n");
	}
	
	time_t now_time;
	time(&now_time);
	char *tim = ctime(&now_time);
	char mytime[24];
	for(int i=0; i<strlen(tim)-1 ; i++){
		mytime[i] = tim[i]; // to fit the format when printing 
	}
	int l=0;
	while(pids[l]!=(pid_t)0){
		l++;
	}
	logfile = fopen(server_log, "a"); // open logfile in append mode 
	for(int i=0; i<l; i++){
		*((int*)shm_addr)-=1;
		kill(pids[i],SIGTERM); // give signal SIGTERM every pids to kill 
		printf("\n[%s] %d process is terminated\n[%s] idlecount : %d",mytime,(int)pids[i],mytime, *((int*)shm_addr)); // print out which process is died. 
		fprintf(logfile,"\n[%s] %d process is terminated\n[%s] idlecount : %d",mytime,(int)pids[i],mytime, *((int*)shm_addr)); // print on logfile 
	}
	printf("\n");
	fprintf(logfile, "\n"); // print on logfile 
	printf("[%s] Server is terminated\n",mytime);
	fprintf(logfile,"[%s] Server is terminated\n", mytime); // print on logfile 
	fclose(logfile); // close logfile 
	if(shmctl(shm_id,IPC_RMID,0)==-1){ // delete all shared memory 
                printf("shmctl fail\n");
    }
	while(wait(NULL)>0)//parent wait for children to die and get information about children 
	
	exit(0);
}

void child_main(int i, int sockdet_fd, int addrlen, struct sockaddr_in client_addr,RList* list, pid_t pid, pthread_t tid); // predeclation for child_main


//////////////////////////////////////////////////////////////////////
//function for making child processeor 				    			//
//==================================================================//
//Input: i -> child number 					  						//
//. 	 socket_fd -> socket file descriptor 			    		//
//		 addrlen -> size of client address 		    				//
//		 client_addr -> client address 			    				//
//		 list -> linked list for child information 	    			//
//Output : fork process 					    					//
//Purpose : to make child process 				    				//
//////////////////////////////////////////////////////////////////////
pid_t child_make(int i, int socket_fd, int addrlen, struct sockaddr_in client_addr, RList** list){
	pid_t pid; 
	pthread_t tid;
	if((pid = fork())>0){ // making fork porcessor 
		return(pid);
	}

	child_main(i, socket_fd, addrlen, client_addr, list[i], pid, tid); 
}


//////////////////////////////////////////////////////////////////////
//function for child processor to handle requeset 		    		//
//==================================================================//
//Input : Same as child_make() 					    				//
//Output : handle request from client 				    			//
//			decrease shared memory if connected 					//
//			increase shared memory if disconnected 					//
//Purpose : to handle rqueset from client 			    			//		
//////////////////////////////////////////////////////////////////////
void child_main(int i, int socket_fd, int addrlen, struct sockaddr_in client_addr, RList* list, pid_t pid, pthread_t tid){
	int client_fd, len_out; // client file discriptor and len_out 
	char buf[BUFFSIZE]; 
	socklen_t clilen;

	time_t now_time=time(NULL); // get current time 
    char *tim = ctime(&now_time); // make current time to character form 
	char mytime[24]; 

	for(int i=0; i<24; i++){
		mytime[i] = tim[i]; // to get rid of ENTER at the end of ctime 
	}
	
	int shm_id;
	void *shm_addr;
	if((shm_id = shmget((key_t)KEY_NUM, MEM_SIZE, IPC_CREAT|0666))==-1){ // create shared memory 
		printf("shmget fail\n");
		exit(0);
	}
	if((shm_addr = shmat(shm_id, (void*)0, 0)) == (void*)-1){ // combine shared memory as if processor's own memory 
		printf("shmat fail\n");
		exit(0);
	}

//////////////////////////////////////////// using semaphore  //////////////////////////////////////////////
	mysem = sem_open("PORTNO", O_RDWR); // open semaphore file in read write mode 
	sem_wait(mysem); // lock semaphore 
	pthread_create(&tid, NULL, &doit1, NULL); // created thread doing doit1 function 
	pthread_join(tid, NULL); // wait for thread's termination 
	printf("[%s] %ld process is forked\n",mytime,(long)getpid());
	FILE *logfile = fopen(server_log, "a"); // file open logfile in append mode  
	fprintf(logfile,"[%s] %ld process is forked\n",mytime,(long)getpid());	
	kill(getppid(), SIGUSR2); // giving SIGUSR2 to parent processor to print shared memory counter 
	fclose(logfile); // close logfile 
	sem_post(mysem); // unlock sempahore 
	sem_close(mysem); // close semaphore
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

	RStart(list); // start list 
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
       		   		//continue; 
       			}
			
			
			if(pid==0){
				//RInsert(list,inet_ntoa(inet_client_address), getpid(), client_addr.sin_port, timi); // insert client's infromation to list 
				//sprintf(shm_addr+4+i*1000,"%s\t%d\t%d\t%s",inet_ntoa(inet_client_address), getpid(), client_addr.sin_port, timi);
				clock_t start, end;
				mysem = sem_open("PORTNO", O_RDWR);
				tok=strtok(tmp," ");
				strcpy(method, tok);
				if(strcmp(method, "GET")==0){
					tok = strtok(NULL," ");
					strcpy(url, tok);
				}	
				if(strcmp(url,"/favicon.ico")==0){ // if url is favicon.io 
					close(client_fd); // close client and continue
					exit(0);
                }
				time_t in_time;
				time(&in_time);
				char *timi = ctime(&in_time);
  		      	printf("======= New Client ======\n[%s]\n",mytime);// print format for new connection  
                printf("IP: %s \nURL: %s\nPort : %d\nPID: %d\n",inet_ntoa(inet_client_address),url,client_addr.sin_port,(int)getpid());
                printf("=========================\n");
				start = clock();
				pthread_mutex_lock(&counter_mutex); // lock thread not to use resource from other process 
				*((int*)shm_addr)-=1; // decrease shared memory counter 
				pthread_mutex_unlock(&counter_mutex); // unlock thread to get used by other process which was waiting 
	///////////////////////////////////////////////// using semaphore //////////////////////////////////////////////
				sem_wait(mysem); // lock semaphore 
				FILE *logfile = fopen(server_log, "a"); // open log file 
				fprintf(logfile,"======= New Client ======\n[%s]\n",mytime);// print format for new connection  
                fprintf(logfile,"IP: %s \nURL: %s\nPort : %d\nPID: %d\n",inet_ntoa(inet_client_address),url,client_addr.sin_port,(int)getpid());
                fprintf(logfile,"=========================\n");
				kill(getppid(), SIGUSR2); // giving parent process to print shared memory counter 
				fclose(logfile); // close log file 
				sem_post(mysem); // unlock semaphore 
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				sprintf(shm_addr+4+i*1000,"%s\t%d\t%d\t%s",inet_ntoa(inet_client_address), getpid(), client_addr.sin_port, timi);
				
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
				end = clock();
				sleep(5); // sleep 5 second before disconnecting client 
				pthread_mutex_lock(&counter_mutex); // lock thread not to be used by other process 
				*((int*)shm_addr)+=1; // increase shared memory counter 
				printf("===== Disconnected Client =====\n[%s]\n",mytime); // output format of disconnected client 
		        printf("IP: %s \nPort : %d\n",inet_ntoa(inet_client_address),client_addr.sin_port);
                printf("IP: %s \nURL: %s\nPort : %d\nPID: %d\n",inet_ntoa(inet_client_address),url,client_addr.sin_port,(int)getpid());
               	printf("CONNECTING TIME : %d(us)\n", (int)(end-start));
                printf("===============================\n");
				logfile = fopen(server_log, "a");
		////////////////////////////////////////////////// using semaphore ////////////////////////////////////////////
				sem_wait(mysem); // lock semaphore 
				fprintf(logfile,"===== Disconnected Client =====\n[%s]\n",mytime); // output format of disconnected client 
                fprintf(logfile,"IP: %s \nPort : %d\n",inet_ntoa(inet_client_address),client_addr.sin_port);
				fprintf(logfile,"IP: %s \nURL: %s\nPort : %d\nPID: %d\n",inet_ntoa(inet_client_address),url,client_addr.sin_port,(int)getpid());
				fprintf(logfile,"CONNECTING TIME : %d(us)\n", (int)(end-start));
				fprintf(logfile,"===============================\n");
				kill(getppid(), SIGUSR2); 
				fclose(logfile); // close logfile 
				sem_post(mysem); // unlock semaphore 
				sem_close(mysem); // close sempahore file 
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				pthread_mutex_unlock(&counter_mutex); // unlock thread 

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
//		  hfile -> file for "httpd.conf" 							 //
//Output : print out new connection's ip address and port number 	 //
//		   print out disconnected client's ip address and port number//
//		   increas processor if idle counter is less than MinIdleNum //
//		   decrease processor if idle counter is bigger than 		 //
//			MaxIdleNum												 //
///////////////////////////////////////////////////////////////////////
int main(){
	FILE *hfile; 
	FILE *logfile; // 
    hfile = fopen("httpd.conf","r"); // open file "httpd.conf"

    char hbuf[255];
    memset(hbuf, 0, 255);
    fread(hbuf, 1, 255, hfile); // read content in hfile on hbuf 

    char *result = strtok(hbuf, " \n"); // tokenizing hbuf 
    char *arr[5];
   	memset(arr, 0, 5);
    int f =0;
    int a=0;
    while(result!=NULL){
     	result = strtok(NULL, " \n"); // tokenizing by space and enter 
       	if((f+1)%2==1){
       		arr[a] = result; // store in arr 
       		a++;
       	}
       	f++;
   	}	

   	MaxChilds = atoi(arr[0]); // idle counter and number of child process should not be bigger than MaxChilds 
    MaxIdleNum = atoi(arr[1]); // idle counter should not be bigger than MaxIdleNum 
    MinIdleNum = atoi(arr[2]); // idle counter should not be smaller than MinIdleNum 
    StartServers = atoi(arr[3]); // number of preforked process 
    MaxHistory = atoi(arr[4]); // maximum number of printed history 
	maxNchildren = StartServers;

	for(int i=0; i<MaxChilds; i++){
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


	pids = (pid_t *)malloc(MaxChilds*sizeof(pid_t)); // make array for pids 
	memset(pids, 0, MaxChilds-1);
	int addrlen = sizeof(client_addr);
	
	time_t now_time;
    time(&now_time);
    char *tim = ctime(&now_time);
    char mytime[24];
    for(int i=0; i<strlen(tim)-1 ; i++){
      	mytime[i] = tim[i];
    }
    ////////////////////////// making semaphore ///////////////////////
	mysem = sem_open("PORTNO", O_CREAT|O_EXCL, 0777,1); // create semaphore file 
	sem_close(mysem); // close semaphore 
	logfile = fopen(server_log, "w"); // open file log file in mode of write 
	signal(SIGUSR2, signalHandler); // get signal from chld processor about SIGUSR2 
	printf("[%s] Server is started\n", mytime);
	fprintf(logfile,"[%s] Server is started\n", mytime); // print on log file 
	fclose(logfile); // close log file 

	for(int i=0; i<maxNchildren; i++){
		pids[i] = child_make(i, socket_fd, addrlen, client_addr, &list[i]); // get pid from child processor 
		sleep(1);
	}
	
	int shm_id;
	void *shm_addr;
	if((shm_id=shmget((key_t)KEY_NUM, MEM_SIZE, IPC_CREAT|066))==-1){ // create shared memory 
		printf("shmget fail\n");
		exit(0);
	}
	if((shm_addr = shmat(shm_id, (void*)0, 0))==(void*)-1){ // combine shared memory as processor's own memory 
		printf("shmat fail\n");
		exit(0);
	}

	signal(SIGINT, sig_int); // signal handlfer for SIGINT
	signal(SIGALRM, signalHandler); // signal handler for SIGALRM 
	
	for(; ;){
		alarm(10); // do alarm for 10 sec 
		if(MinIdleNum > *((int*)shm_addr)){ // if shared memroy counter is smaller than MinIdleNum 
			int l=0;
			while(pids[l]!=(pid_t)0){  
				l++; // increase until pids is NULL 
			}
			for(int i=0; i<=MinIdleNum - *((int*)shm_addr); i++){ 
				if(i+l<10){ 
					pids[i+l] = child_make(i+l, socket_fd, addrlen, client_addr, &list[i]); // make child processor until shared memory counter is bigger than MinIdleNum 
				}
				else
					continue;
			}
			sleep(1);
		}
		if(MaxIdleNum <= *((int*)shm_addr)){ // if shared memory counter is bigger than MaxIdleNum 
			int sub = *((int*)shm_addr) - MaxIdleNum; // get gap between shared memroy counter and MaxIdleNum 
			int l=0; 
			FILE *logfile=fopen(server_log, "a"); 
			while(pids[l]!=(pid_t)0)
				l++;
			
			for(int i=l-1; i>=l-sub-1 ; i--){ 
				*((int*)shm_addr)-=1; // decrease shared memory counter until shared memory counter gets smaller than MaxIdleNum 
				printf("[%s] %d process is terminated\n",mytime, (int)pids[i]); 
				fprintf(logfile, "[%s] %d process is terminated\n",mytime, (int)pids[i]);
				raise(SIGUSR2);//give myself SIGUSR2 signal 
				kill(pids[i], SIGTERM); // terminated processor 
				pids[i] = (pid_t)0; // set pids as NULL 
			}
			fclose(logfile);
		}	
		if(*((int *)shm_addr) == StartServers){
			pause();
		}
		pause();
	}
    	fclose(hfile);
	close(socket_fd); // close socket
	sem_unlink("PORTNO");
	return 0;
}





