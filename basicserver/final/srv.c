//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//File Name     : srv.c (main.c)                                                               					 //
//Date          : 2019/5/10                                                                                   	 //
//Os            : Ubuntu 16.04. LTS 64bits                                                                       //
//Author        : JIN SU LEE                                                                                     //
//Student ID    : 2015722013                                                                                	 //
//---------------------------------------------------------------------------------------------------------------//
//Title         : System Programming Assignment #3-2 (basic server)            									 // 
//Discription 	: 이번 과제는 이전 과제에서 구현했던 html ls를 소켓 프로그래밍을 통해 다른 서버 브라우저에도 띄울 수 있게 하는 과제다. 	 //
//				 Server application이 있는 디렉토리가 root 디렉토리가 되고 하위 디렉토리로만 이동 가능하게 구현 하면 된다. root 디렉토//
//				 리만 l 옵션으로 보여주고 하위 디렉토리들은 al 옵션으로 보여 준다. text 파일은 브라우저 상에서 보여야 하고 실행 파일이나 여타//
//				 다른 파일들은 다운받는 형식으로 보내 주어야 한다. 															 //
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

#include "ht_ls.h" // include header file 

#define URL_LEN 256
#define BUFSIZE 1024
#define PORTNO 40000
#define _GNU_SOURCE
//////////////////////////////////////////////////////////////////////
//main function 						 							//
//==================================================================//
//Input : socket_fd -> to get domain type, port number and binding 	//
//						associated address 							//
//		  client_fd -> to accept from server and to get information //
// 		  				from server 								//
//Output : print html file on browser. if test file, print text file//
//		  content on browser. if image file or others then show 	//
//			question box to ask rather to download or show.			//
//Purpose : to print out anything on browser.						//
//////////////////////////////////////////////////////////////////////
int main(){
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

	setsockopt(socket_fd,SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); // get socket information

	memset(&server_addr, 0, sizeof(server_addr)); // reset server_addr
	server_addr.sin_family = AF_INET; // to get address family 
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // to get ip address 
	server_addr.sin_port = htons(PORTNO); // to get port number 

	if(bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr))<0){// binnding socket with socket information
		printf("Server : Can't bind local address\n");
		return 0;
	}

	listen(socket_fd, 5); // listen until client accept

//////////////////////////////// accepting until user input ^c /////////////////////////

	while(1){
		struct in_addr inet_client_address;
		char buf[BUFSIZE] = {0, }; // for client_fd buffer 
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
		printf("[%s : %d] client was connected\n",inet_ntoa(inet_client_address),client_addr.sin_port);
		if(read(client_fd, buf, BUFSIZE) <= 0){
			printf("Buffer : %s\n", buf);
			close(client_fd);
			continue;
		}
		strcpy(tmp,buf);
		puts("=====================================================");
		printf("Request from [%s : %d]\n", inet_ntoa(inet_client_address),client_addr.sin_port);
		puts(buf);
		puts("=====================================================");

/////////////////// getting url from tmp //////////////////

		tok=strtok(tmp," ");
		strcpy(method, tok);
		if(strcmp(method, "GET")==0){
			tok = strtok(NULL," ");
			strcpy(url, tok);
		}

////////////////////////////////////////////////////////////

		int func=1;

////////////////// continue if /favicon.ico came for url ////////////////

		if(strcmp(url,"/favicon.ico")==0){
			printf("fav\n");
			close(client_fd);
			continue;
		}

/////////////////////////////////////////////////////////////////////////

////////////////////// determine file's type ////////////////////////

		#define	FNM_CASEFOLD	(1 << 4) // for using FNM_CASEFOLD
		if(fnmatch("*.txt",url, 0)==0||fnmatch("*.c",url, 0)==0){ // if file is text file
			strcpy(mime,"text/plain");
			func=1;
		}
		else if(fnmatch("*.html", url, 0)==0){ // if file is html file 
			strcpy(mime, "text/html");
			func=0;
		}
		else if(fnmatch("*.jpg",url,FNM_CASEFOLD)==0||fnmatch("*.png",url,FNM_CASEFOLD)==0||fnmatch("*.jpeg",url,FNM_CASEFOLD)==0){ // if file is image file 
			strcpy(mime,"image/*");
			func=2;
		}

///////////////////////////////////////////////////////////////////
		printf("mime: %s\n",mime);
		int filesize;
		FILE *file;
//////////////////////////// if ht_ls(url) is working fine ///////////////////////

		if(ht_ls(url)>=0){
			file = fopen("html_ls.html","r"); // open html_ls.html file 
			if(file==NULL){ // if can not open continue
				printf("file error\n");
				close(client_fd);
				continue; 
			}
	        fseek(file, 0, SEEK_END);
	        filesize= ftell(file); // get filesize 
	        fseek(file, 0, SEEK_SET);

			response_message = (char*)malloc(sizeof(char)*(filesize+1)); // dynamic allocation sizeof filesize
			memset(response_message, 0 ,filesize+1); // reset response message 
			if(fread(response_message,1, filesize, file)<0){ // get infromation from html file 
				printf("error\n");
			}
		}

//////////////////////////////////////////////////////////////////////////////////

///////////////// if ht_ls isn't working because url is not directroy ////////////	
		else if(ht_ls(url)==-1){ 
			int loc=0; // location for / 
			for(int i=0; i<strlen(url);i++){
				if(url[i]=='/')
					loc=i+1;
			}
			char turl[255]={0, };
			for(int i=loc; i<strlen(url);i++){
				turl[i-loc]=url[i];
			} // making new url witout 
			file=fopen(turl,"r"); // open file which is not directory 
			fseek(file, 0, SEEK_END);
        	filesize= ftell(file); // get filesize 
            fseek(file, 0, SEEK_SET);
			response_message = (char*)malloc(sizeof(char)*(filesize+1));
	        memset(response_message, 0 ,filesize+1);
		}

//////////////////////////////////////////////////////////////////////////////////

/////////////// if ht_ls isn't working because url isn't existing path ///////////

		else if(ht_ls(url)==-2){
			file=fopen("html_ls.html","r");
			fseek(file, 0, SEEK_END);
			filesize = ftell(file);
			fseek(file, 0, SEEK_SET);
			response_message = (char*)malloc(sizeof(char)*(filesize+1));
			memset(response_message, 0, filesize+1);
		}

/////////////////////////////////////////////////////////////////////////////////
		
////////////////////////////// depending on fnmatch function ////////////////////////////////

		if(func==1){ // when 
			if(fread(response_message,1, filesize, file)<0){
                                printf("error\n");
                       	}
			sprintf(response_header,
				"HTTP/1.0 200 OK \r\n"
				"Server: 2019 simple web server\r\n"
				"Content-length: %d\r\n"
				"Content-type: %s\r\n\r\n",filesize,mime);
			write(client_fd, response_header, strlen(response_header));
			write(client_fd, response_message, filesize);//strlen(response_message));
		}
		else if(func==0){
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

 //////////////////////////////////////////////////////////////////////////////////////////////////
		printf("strlen(response): %lu\n",strlen(response_message));
		free(response_message);
		printf("[%s:%d] client was disconnected\n",inet_ntoa(inet_client_address),client_addr.sin_port);
		//printf("after dir: %s\n", getcwd(cur,1024));
		memset(mime,0,strlen(mime));
		fclose(file);
		close(client_fd);
	}

///////////////////////////////////////////////////////////////////////////////////////////

	close(socket_fd);
	return 0;
}
