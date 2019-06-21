///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//File Name     : ls.c                                                                      					 //
//Date          : 2019/5/03                                                                                   	 //
//Os            : Ubuntu 16.04. LTS 64bits                                                                       //
//Author        : JIN SU LEE                                                                                     //
//Student ID    : 2015722013                                                                                	 //
//-------------------------------------------------------------------------------------------------------------- //
//Title         : System Programming Assignment #3-1 (HTML - ls)             									 // 
//Discription 	: 이번 과제는 이전까지 구현 했떤 ls 기능을 출력을 콘솔창에 출력 하는 것이 아니라 html 파일에 출력을 하여서 브라우저 위에서  //
//				  볼 수 있게 하는것이다. 각 파일이나 디렉토리는 하이퍼링크를 통해서 해당 파일을 open 혹은 디렉토리로 이동 할 수 있어야 한다 //
//				  link라면 초록색으로, directory면 파란색으로 나머지는 모두 빨간색으로 구분 한다. 
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <grp.h>
#include <time.h>
#include <pwd.h>
#include <errno.h>
#include <fnmatch.h>
#include <math.h>


typedef struct NODE{
	char *name;  // node 의 이름 
	int size;
	struct NODE *next; // 다음 node를 가르키고 있는 포인터 
}Node;

typedef struct LIST{
	int cnt; // List의 갯수 
	Node *head; // List의 시작 지점을 가르키는 포인터 
}List;


typedef struct Flag{ // flag의 return 값을 3개로 나눈다. 
	int f1;
	int f2;
	int f3;
}Flag;


void Start(List *list){
	list->cnt=0; // list 의 갯수를 0으로 초기화 한다. 
	list->head=NULL; // list 의 첫번째 노드를 NULL로 초기화 한다.
}


Flag Flag2(int argc, char *argv[]){
	Flag f;
	int aflag=0, lflag=0, hflag=0, rflag=0, sflag=0;
	int c=0;
	int t;

	while((c=getopt(argc,argv, "alhrs"))!=-1){//get option flag 
		switch(c){
			case 'a': // if c is a then aflag is 1 
			aflag=1;
			break;
			case 'l': // if c is l then lflag is 1
			lflag=1;
			break;
			case 'h':
			hflag=1;
			break;
			case 'r':
			rflag=1;
			break;
			case 's':
			sflag=1;
			break;
		}
	}

	lflag<<=4; // bit shifting 1flag to make it 16
	aflag<<=3; // bit shifting aflag to make it 8
	sflag<<=1; // bit shifting sflag to make it 2

	f.f1 = lflag+aflag; // first return type. determine a l flag
	f.f2 = sflag+rflag; // second return type, determine r s flag 
	f.f3 = hflag; // third return type, dtermine h flag 

	return f;
}


int Insert(List *list, struct dirent *dir, int h){
	Node *new = (Node *)malloc(sizeof(Node)); // making new node 
	Node *current = NULL; // current node is null 
	Node *prev = NULL; // pointer for previous node is null 
	struct stat buf; 

	
	new->name = dir->d_name; // new node's name is directory's name 
	lstat(new->name, &buf);
	new->size = (int)buf.st_size;

	new->next = NULL; // new node's next pointer is NULl 
	
	if(list->head == NULL) // in case of new is first node in list 
		list->head = new;
	else{ // else 
		current = list->head; 
		while(current!=NULL){
			if(strcasecmp(current->name,new->name)>0){ // if current's name is bigger than new's name 
				if(current == list->head){ // if current node is first of the list 
					list->head=new;
				}
				else { // if current node is middle of the list 
					prev->next = new; // new node goes into the current's previous node 
				}
				new->next = current; // new node's next is current node 
				break; 
			}
			else{ //if new node's name is bigger than current node's name
				prev=current;  // current node previous node is now current 
				current = current->next; 
			}
		}
		
		if(current == NULL){
			prev->next = new;
		}
	}
	list->cnt++; // increasing list's cnt 
	return 0;
}


int StatInsert(List *list, struct dirent *dir, int h){
	Node *new = (Node *)malloc(sizeof(Node)); // making new node 
	Node *current = NULL; // current node is null 
	Node *prev = NULL; // pointer for previous node is null 
	struct stat buf;

	new->name = dir->d_name; // new node's name is direc tory's name 
	lstat(new->name, &buf); // get info about new->name 
	new->size = (int)buf.st_size; // new->name's file size 

	new->next = NULL; // new node's next pointer is NULl 
	
	if(list->head == NULL) // in case of new is first node in list 
		list->head = new;
	else{ // else 
		current = list->head; 
		while(current!=NULL){
			if(current->size < new->size){ // if current's size is smaller than new's size 
				if(current == list->head){ // if current node is first of the list 
					list->head=new;
				}
				else { // if current node is middle of the list 
					prev->next = new; // new node goes into the current's previous node 
				}
				new->next = current; // new node's next is current node 
				break; 
			}
			else if(current->size==new->size){ // if lasting current->size is same as the new->size sort by the name 
				if(strcasecmp(current->name,new->name)>0){ // if current's name is bigger than new's name 
					if(current == list->head){ // if current node is first of the list 
						list->head=new;
					}			
					else { // if current node is middle of the list 
						prev->next = new; // new node goes into the current's previous node 
					}
					new->next = current; // new node's next is current node 
					break; 
				}
				else{ //if new node's size is bigger than current node's size
					prev=current;  // current node previous node is now current 
					current = current->next; 
				}
			}
			else{ //if new node's name is bigger than current node's name
				prev=current;  // current node previous node is now current 
				current = current->next; 
			}
		}
		
		if(current == NULL){
			prev->next = new;
		}
	}
	list->cnt++; // increasing list's cnt 
	return 0;
}


int RevInsert(List *list, struct dirent *dir, int h){
	Node *new = (Node *)malloc(sizeof(Node)); // making new node 
	Node *current = NULL; // current node is null 
	Node *prev = NULL; // pointer for previous node is null 
	struct stat buf; 

	
	new->name = dir->d_name; // new node's name is directory's name 
	lstat(new->name, &buf);
	new->size = (int)buf.st_size;

	new->next = NULL; // new node's next pointer is NULl 
	
	if(list->head == NULL) // in case of new is first node in list 
		list->head = new;
	else{ // else 
		current = list->head; 
		while(current!=NULL){
			if(strcasecmp(current->name,new->name)<0){ // if current's name is smaller than new's name 
				if(current == list->head){ // if current node is first of the list 
					list->head=new;
				}
				else { // if current node is middle of the list 
					prev->next = new; // new node goes into the current's previous node 
				}
				new->next = current; // new node's next is current node 
				break; 
			}
			else{ //if new node's name is smaller than current node's name
				prev=current;  // current node previous node is now current 
				current = current->next; 
			}
		}
		
		if(current == NULL){
			prev->next = new;
		}
	}
	list->cnt++; // increasing list's cnt 
	return 0;

}


int RevStatInsert(List *list, struct dirent *dir, int h){
	Node *new = (Node *)malloc(sizeof(Node)); // making new node 
	Node *current = NULL; // current node is null 
	Node *prev = NULL; // pointer for previous node is null 
	struct stat buf;

	new->name = dir->d_name; // new node's name is directory's name 
	lstat(new->name, &buf);
 
	new->size = (int)buf.st_size;
	new->next = NULL; // new node's next pointer is NULl 
	
	if(list->head == NULL) // in case of new is first node in list 
		list->head = new;
	else{ // else 
		current = list->head; 
		while(current!=NULL){
			if(current->size > new->size){ // if current's size is bigger than new's name 
				if(current == list->head){ // if current node is first of the list 
					list->head=new;
				}
				else { // if current node is middle of the list 
					prev->next = new; // new node goes into the current's previous node 
				}
				new->next = current; // new node's next is current node 
				break; 
			}
			else if(current->size==new->size){
				if(strcasecmp(current->name,new->name)<0){ // if current's name  is smaller than new's name 
					if(current == list->head){ // if current node is first of the list 
						list->head=new;
					}			
					else { // if current node is middle of the list 
						prev->next = new; // new node goes into the current's previous node 
					}
					new->next = current; // new node's next is current node 
					break; 
				}
				else{ //if new node's name is smaller than current node's name 
					prev=current;  // current node previous node is now current 
					current = current->next; 
				}
			}
			else{  // if current node's size is smaller than new's size 
				prev=current;  // current node previous node is now current 
				current = current->next; 
			}
		}
		
		if(current == NULL){
			prev->next = new;
		}
	}
	list->cnt++; // increasing list's cnt 
	return 0;
}


int status(List *list, Node *current, int h, FILE* file){
        DIR *dirp;
        struct dirent *dir;
        unsigned int tmp;
        struct stat buf;
        struct passwd *pwd = NULL;
        struct group *gr = NULL;
        struct tm *mktm= NULL;
        char mon[15];
        lstat(current->name,&buf);
        long int pw = pow(1024,4);
        char cur[1024];
        char kk[1024];
        getcwd(cur,1024);
      	
        if(S_ISDIR(buf.st_mode)){
    		dirp=opendir(current->name);    	
    		chdir(current->name);
        	fprintf(file,"<tr style=color:blue><td><a href=%s>%s\n</a></td>",getcwd(kk,1024),current->name);
        	chdir(cur);
        }
        else if(S_ISLNK(buf.st_mode)){    	
    		chdir(current->name);
        	fprintf(file,"<tr style=color:green><td><a href=%s/%s>%s\n</a></td>",getcwd(kk,1024),current->name,current->name);
        }
        else{
        	chdir(current->name);
        	//printf("%s/%s\n",getcwd(kk,1024),current->name);
        	fprintf(file,"<tr style=color:red><td><a href=%s/%s>%s\n</a></td>",getcwd(kk,1024),current->name,current->name);
        }
       
		if(lstat(current->name,&buf)==0){ // if current's name is written on buf correctly 
			if(S_ISREG(buf.st_mode)) // if file's mode is regular file 
				 fprintf(file,"<td>-");
			else if(S_ISDIR(buf.st_mode))//if file's mode is directory
				 fprintf(file,"<td>d");
			else if(S_ISCHR(buf.st_mode))//if file's mode is character device 
				 fprintf(file,"<td>c");
			else if(S_ISBLK(buf.st_mode))//if file's mode is block device 
				 fprintf(file,"<td>b");
			else if(S_ISLNK(buf.st_mode))//if file's mode is symbolic link 
				  fprintf(file,"<td>l");
			else if(S_ISSOCK(buf.st_mode))//if file's mode is socekt 
				  fprintf(file,"<td>s");
		}
		tmp=buf.st_mode; //current name path's authority 
		for(int i=0; i<3; i++){
            if(tmp & S_IRUSR) // comparing read authority 
       	        fprintf(file,"r");     
            else
        		fprintf(file,"-");
			if(tmp & S_IWUSR) // comparing write authority 
				fprintf(file,"w");
			else
				fprintf(file,"-");
			if(tmp & S_IXUSR) // comparing execute authority 
				fprintf(file,"x");
			else
				fprintf(file,"-");   
			tmp<<=3; // 3bit shifting to left 
		}
		fprintf(file,"</td>");

		mktm=localtime(&buf.st_mtime);
		int m = mktm->tm_mon;
		switch(m){
			case 0 :
				strcpy(mon ,"Jan");
				break;
			case 1 :
				strcpy(mon, "Feb");
				break;
			case 2 :
				strcpy(mon, "Mar");
				break;
			case 3 :
				strcpy(mon, "Apr");
				break;
			case 4 :
				strcpy(mon, "May");
				break;
			case 5 :
				strcpy(mon, "Jun");
				break;
			case 6 :
				strcpy(mon, "Jul");
				break;
			case 7 :
				strcpy(mon, "Aug");
				break;
			case 8 :
				strcpy(mon, "Sep");
				break;
			case 9 :
				strcpy(mon, "Oct");
				break;
			case 10 :
				strcpy(mon, "Nov");
				break;
			case 11 :
				strcpy(mon, "Dec");	
				break;
		}
		
		pwd=getpwuid(buf.st_uid); //user name 
		gr=getgrgid(buf.st_gid); // group nae 
		char *tim=ctime(&buf.st_mtime);
		fprintf(file,"<td>%d</td>",(int)buf.st_nlink);
		fprintf(file,"<td>%s</tb>",pwd->pw_name);
		fprintf(file,"<td>%s</tb>",gr->gr_name);
		if(h==1){
			if(buf.st_size<1024)
				fprintf(file,"<td>%d</td>",(int)current->size);
			else if(buf.st_size>pw)
				fprintf(file,"<td>%liG</td>",(int)current->size/pw);
			else
				fprintf(file,"<td>%dk</td>",(int)current->size/1024);
		}
		else
			fprintf(file,"<td>%d</td>",(int)current->size);
		fprintf(file,"<td>%s  %d  %d:%d</td>",mon,mktm->tm_mday,mktm->tm_hour,mktm->tm_min);
		
		fprintf(file,"</tr>");

		//chdir(cur);
		//closedir(dirp);
		return 0;
}


int Print(List *list, Flag f, FILE *file){
	char cur[1024];
	Node *current = list->head;
	fprintf(file,"<body>");
	fprintf(file,"<TABLE>");
	fprintf(file, "<table border=1>");
	fprintf(file, "<tr><th>File Name</th></tr>");
	if(f.f1==8 || f.f1==24){ // if option flag is 1 or 3 
		for(int i=0 ; i<list->cnt; i++){
			if(strcmp(current->name,"simple_ls.html")!=0){
				fprintf(file,"<tr><td><a href=%s>%s</a></td></tr>\n",getcwd(cur,1024),current->name);
			}  
			current=current->next;
		}
	}
	else if(f.f1==16||f.f1==0) { // if option flag is 2 or 4 
		for(int i=0; i<list->cnt; i++){
			if(strncmp(current->name,".",1)&&strcmp(current->name,"simple_ls.html")!=0) {  // comparing first letter with "."
				fprintf(file,"<tr><td><a href=%s/%s>%s</a></td></tr>\n",getcwd(cur,1024),current->name,current->name); // print all file except for hidden file
			}
			current=current->next;
		}
	}
	fprintf(file,"</TABLE>");
	fprintf(file,"</body>");
	return 0;
}


int Statprint(List *list,  Flag f,FILE *file){
	Node *current = list->head;
	unsigned int tmp;
	struct stat buf;
	struct passwd *pwd = NULL;
	struct group *gr = NULL;
	struct tm *mktm= NULL;
	char mon[15];
	int blsize=0;

	if(f.f1 == 8 || f.f1==0)
		return 0;
	else if(f.f1==16){
		for(int i=0; i<list->cnt; i++){ 
			if(strncmp(current->name,".",1)){ 
				if(lstat(current->name,&buf)==0){
					blsize = blsize + (int)buf.st_blocks/2; 
				} // calculating all files's total bloack size except for hidden file 
			else 
				continue;
			}
			current = current->next;
		}
		fprintf(file,"<div>");
		fprintf(file,"<h>total : %d\n</h><br>",(int)blsize);	
		//fprintf(file, "<table border=1>");
		fprintf(file,"<body>");
		fprintf(file,"<TABLE>");
		fprintf(file,"<table border=1>");
		fprintf(file, "<tr><th>File Name</th><th>Permission</th><th>Link</th><th>Owner</th><th>Group</th><th>Size</th><th>Last Modified</th></tr>");
		
		current=list->head;
	
		for(int i=0; i<list->cnt;  i++){ 
			if(strncmp(current->name,".",1)&&strcmp(current->name,"simple_ls.html")!=0)
				status(list, current, f.f3, file);
			current= current->next;
		} // printing file status 
		fprintf(file,"</TABLE>");
		fprintf(file,"</body>");
		fprintf(file,"</div>");
	}
	else if(f.f1==24){
		for(int i=0; i<list->cnt; i++){
			if(lstat(current->name, &buf)==0){
				blsize=blsize+buf.st_blocks;
			} // caclulating all files's block size including hidden file 
			current=current->next;
		}
		current = list->head;
		fprintf(file,"<div>");
		fprintf(file,"<h>total : %d\n</h>", blsize/2);
		//fprintf(file, "<table border=1>");
		fprintf(file,"<body>");
		fprintf(file,"<TABLE>");
		fprintf(file,"<table border=1>");
		fprintf(file, "<tr><th>File Name</th><th>Permission</th><th>Link</th><th>Owner</th><th>Group</th><th>Size</th><th>Last Modified</th></tr>");
		
		
		for(int i=0; i<list->cnt; i++){
			if(strcmp(current->name,"simple_ls.html")!=0)
				status(list, current, f.f3,file);
			current=current->next;
		}
		fprintf(file,"</TABLE>");
		fprintf(file,"</body>");
		fprintf(file,"</div>");
			
	}
	return 0;
}


int wildcard(int argc, char *wild){
	int wflag=0;
	int len=strlen(wild);
	for(int i=0; i<len ; i++){
		if(wild[i]=='/')
			wflag=1;
	}

	for(int i=0 ; i<len ; i++){
		if(wflag == 0 && (wild[i] == '?' || wild[i] == '['))
			wflag=2;
		else if(wflag==1 && (wild[i] == '?' || wild[i]=='['))
			wflag = 4;
		else if(wflag==1&&wild[i]=='*')
			wflag=3;
		else if(wflag== 0 && wild[i]=='*')
			wflag=5;
	}

    return wflag;
}


int match(char *hum, List *list, char *wild, Flag f, int w, FILE *file){
	Node *current = list->head; // current node is list's head node 
	DIR *dirp;
	struct dirent *dir;
	char tmp[1024];
	char cur[1024]; 
	chdir(hum);
	getcwd(cur,1024); // get current directory's path 
	List *newlist=(List *)malloc(sizeof(List));
	struct stat buf; // status buffer 
	lstat(current->name,&buf); // get current->name's status 
	int wcnt=0; 
	fprintf(file,"<body>");
        fprintf(file,"<TABLE>");
        fprintf(file,"<table border=1>");

	if(f.f1==16||f.f1==24){
                fprintf(file, "<tr><th>File Name</th><th>Permission</th><th>Link</th><th>Owner</th><th>Group</th><th>Size</th><th>Last Modified</th></tr>");
        }
        else if(f.f1==0 || f.f1==8)
                fprintf(file,"<tr><th>File Name</th></tr>");

		for(int i=0; i<list->cnt; i++){
			dirp=opendir(current->name);
			if(!fnmatch(wild,current->name,0)){ // if current->name and wild is matched 
				if(dirp==NULL){ 
				if(f.f1 != 8 && f.f1 != 24){ // depending on option flag
				if(strncmp(current->name,".",1)){ // without hidden file 
                    if(f.f1 !=16){
						//printf("%s\n", current->name); // print only name 
					 fprintf(file,"<tr><td><a href=%s/%s>%s\n</a></td></tr>",getcwd(tmp,1024),current->name,current->name);
					}
                    else if(f.f1 == 16) {
						status(newlist, current, f.f3, file); // print status of the file 
					}
				}
				}
				else if(f.f1 == 8 || f.f1 ==24){
					if(f.f1 == 24){
						status(newlist, current, f.f3,file);}
					else{
						//printf("%s\n",current->name);}
						 fprintf(file,"<tr><td><a href=%s/%s>%s/%s\n</a></td></tr>",getcwd(tmp,1024),current->name,current->name,current->name);
					}
				}wcnt++;
			}
			}
			current=current->next; // move to next node 
		 
		}

	if(w==5&&strlen(wild)==1){ // if wild card is only * 
		current= list->head; // current is list head 
		for(int i=0; i<list->cnt; i++){
			if(strncmp(current->name,".",1)){ // except for hidden file 
				dirp=opendir(current->name);
				if(dirp!=NULL){
					chdir(current->name); // change directory to current->name's path
					printf("directory path: %s:\n",getcwd(tmp,1024)); // print current directory path 
					Start(newlist); // making new list 
					while((dir=readdir(dirp))!=NULL){  
						if(f.f2 == 1) // depending on option flag
	                        RevInsert(newlist,dir,f.f3); // descending sort
	                   	else if(f.f2 == 2)
							StatInsert(newlist, dir, f.f3);// size ascending sort
						else if(f.f2 == 3)
							RevStatInsert(newlist,dir,f.f3); // reverse size  descending sort
	                    else{	
	                        Insert(newlist,dir,f.f3);}// ascending sort
					}
					if(f.f1!=16 &&f.f1!=24)
						Print(newlist, f, file);
					else 
						Statprint(newlist,f,file);
				}
			}
			
			current=current->next;
			chdir(cur);//change path to original path 
		}
	}
	return wcnt;
}

int match2(char *hum, List *list, char *wild, Flag f, int w, FILE *file){
	Node *current = list->head;
	DIR *dirp;
	struct dirent *dir;
	char tmp[1024];
	char cur[1024];
	chdir(hum);
	List *newlist=(List *)malloc(sizeof(List));
	int wcnt =0;

	fprintf(file,"<h> </h>");
	fprintf(file,"<body>");
	fprintf(file,"<TABLE>");
	fprintf(file,"<table border=1>");
	if(f.f1==16||f.f1==24){
		fprintf(file, "<tr><th>File Name</th><th>Permission</th><th>Link</th><th>Owner</th><th>Group</th><th>Size</th><th>Last Modified</th></tr>");
	}
	else if(f.f1==0 || f.f1==8)
		fprintf(file,"<tr><th>File Name</th>");////////////////////
	for(int i=0 ; i<list->cnt; i++){
			getcwd(cur,1024); // change direcotry to original path 
			if(!fnmatch(wild,current->name,0)){ 
				if(f.f1 != 8){
					if(strncmp(current->name, ".",1)){
						dirp = opendir(current->name);
						if(dirp==NULL){
							if(f.f1 == 0){
								fprintf(file,"<tr><td><a href=%s/%s>%s\n</a></td></tr>",getcwd(tmp,1024),current->name,current->name);
							}
							else if(f.f1 == 16||f.f1==24){
								status(list, current, f.f3,file);
							}
						}
					}
				}
				else if(f.f1==8) {
					dirp=opendir(current->name);
					if(dirp==NULL){
						fprintf(file,"<td><a href=%s/%s>%s\n</a></td>",getcwd(tmp,1024),current->name,current->name);	
					}
				}
		wcnt++;	
		}chdir(cur);
		current=current->next;
	}
	fprintf(file,"</TABLE>");
	fprintf(file,"</body>");
	fprintf(file,"<body>");	
		current= list->head;
		for(int i=0; i<list->cnt; i++){
			if(strncmp(current->name,".",1)){
				dirp=opendir(current->name);
				if(dirp!=NULL){
					chdir(current->name);
					fprintf(file,"<td>directory path: %s:\n</td>",getcwd(tmp,1024));
					Start(newlist);
					while((dir=readdir(dirp))!=NULL){
						if(f.f2 == 1) // depending on option flag
	                        RevInsert(newlist,dir,f.f3); // descending sort
	                   	else if(f.f2 == 2)
							StatInsert(newlist, dir, f.f3);// size ascending sort
						else if(f.f2 == 3)
							RevStatInsert(newlist,dir,f.f3); // reverse size  descending sort
	                    else{	
	                        Insert(newlist,dir,f.f3);}// ascending sort
					}
					
					if(f.f1 != 8){
						if(f.f1==0)
							Print(newlist, f, file);
						else if(f.f1==16 || f.f1 == 24)
							Statprint(newlist,f,file);
					}
					else if(f.f1 == 8)
						Print(newlist,f,file);
				}
			}
			//fprintf(file,"</tr>");
			current=current->next;
			chdir(cur); // change to original path 
		}
	fprintf(file,"</body>");
	return wcnt;
}


int check(int argc, char *argv[], int m){
	int w = wildcard(argc, argv[argc-1]);
	int loop = 1;
	DIR *dirp;
	struct dirent *dir;

	while(argv[loop]!=NULL){
		dirp=opendir(argv[loop]);
		if(strncmp(argv[loop],"-",1)){
			 if(errno == ENOTDIR) // not directory error code 
				printf("%s\n",argv[loop]);
			else if(dirp==NULL){//if cannot open directory which means not directory 
                if(m == 0) // if there is no matching 
                    printf("Cannot access %s : No such file or directory\n", argv[loop]);
            }
		}
		loop++;
	}
	return 0;
}

int main(int argc, char *argv[]){
	DIR *dirp;
	struct dirent *dir;
	struct stat buf;
	Flag t;
	int loop=1,c=0, s=0;
	unsigned int tmp;
	List *rlist = (List *)malloc(sizeof(List));	
	char pth[1024];
	char cur[1024];
	getcwd(cur,1024); // get current directory's path 
	int wflag=0;
	t = Flag2(argc, argv);
	int wc=0;
	int st = strlen(argv[argc-1]);

	FILE *file = fopen("simple_ls.html","w");
	fprintf(file, "<table border=1>");
	

	fprintf(file,"<h1>");
	for(int i=0; i<argc; i++){
		fprintf(file,"%s ",argv[i]);
	}
	fprintf(file,"</h1>");

	if(file==0){
		printf("ERROR\n");
		return -1;
	}

	if(argc==1){ // if only one input 
	dirp=opendir("."); // open current directory 
	if(dirp!=NULL){
            Start(rlist); // start linked list 
            while((dir=readdir(dirp))!=NULL){
            	Insert(rlist,dir,t.f3); // insert node 
        	}
			if(t.f1==8 || t.f1==0) // depending on flag option a or l 
               	Print(rlist, t, file); 
			else 
				Statprint(rlist, t,file);
        }

	}

	else  { // if argc is more than 1 
		if(t.f1==8||t.f1==0){ // depending on option flag a, l 
			if(argc>optind){ 
				int i=1;	
				while(argv[i]!=NULL){
					if(strncmp(argv[i],"-",1)){ 

						int z=0; 
						char *hum=(char*)malloc(sizeof(char)); // hum for directoy's definite path
        					char *sym=(char*)malloc(sizeof(char)); // sym for argv's wild card 
						
						for(int j=0; j<strlen(argv[i]); j++){
		        	       			if(argv[i][j]=='/')
		                	       			z=j;
	     				  	}	
	       					if(z>1){
	             			   		for(int j=0; j<z; j++){
	                     	   				hum[j]=argv[i][j];
	              			 		}
	        				}
	        				else // there is no / 
	                				strcpy(hum,argv[i]);

						for(int j=z+1; j<strlen(argv[i]);j++){
							sym[j-z-1]=argv[i][j];  // containing wild card symbol to sym array 
						}

						wflag = wildcard(argc, argv[i]);// determine which wild card 
						if(wflag==1||wflag==0) // depending on wild card value opend directory 
							dirp=opendir(argv[i]);
						else if(wflag == 5){
							dirp=opendir(".");
						}
						else if(wflag==2)
							dirp=opendir(".");
						else if(wflag == 3){
							dirp=opendir(hum);
						}
						else if(wflag == 4){
							dirp=opendir(hum);
						}
						if(dirp!=NULL){
	         				        Start(rlist);
	                				while((dir=readdir(dirp))!=NULL){
	                        				if(t.f2 == 1)
	                        					RevInsert(rlist,dir,t.f3);
	                        				else if(t.f2 == 2)
												StatInsert(rlist, dir, t.f3);
											else if(t.f2 == 3)
												RevStatInsert(rlist,dir,t.f3);
	                        				else{	
	                        					Insert(rlist,dir,t.f3);}
	               					 }
	               				if(wflag == 0 || wflag==1){
	                				Print(rlist, t, file);
								printf("\n");
								}

	                			else if(wflag == 5 || wflag==2){
	                				wc=match(hum,rlist, argv[i],t,wflag,file);
									printf("\n");
	                			}
	                			else if(wflag == 3){
	                				wc=match2(hum,rlist,sym,t,wflag,file);
							//wc = match(hum,rlist,sym,t,wflag);
									printf("\n");
								}
								else if(wflag == 4){
									wc = match(hum, rlist, sym, t, wflag,file);
								}

	        			}
					free(hum); // freeing hum 
					free(sym); // freeing sym 
					}
					i++;
				}
			} 

			else if(argc==optind){ // if there is only option 
				dirp=opendir("."); // open current directory 
				Start(rlist);
				while((dir=readdir(dirp))!=NULL){
					if(t.f2 == 1)
						RevInsert(rlist, dir, t.f3);
					else if(t.f2 == 2)
						StatInsert(rlist, dir, t.f3);
					else if(t.f2 == 3)
						RevStatInsert(rlist,dir,t.f3);
					else 
						Insert(rlist, dir, t.f3);
				}
				Print(rlist,t,file);
			}
		}	

		else if(t.f1==16||t.f1==24) { // depending on option flag a or l 
			if(argc>optind){  
				int i=1;
				while(argv[i]!=NULL){
					if(strncmp(argv[i],"-",1)){

						int z=0;
						char *hum=(char*)malloc(sizeof(char));
        					char *sym=(char*)malloc(sizeof(char));
							for(int j=0; j<strlen(argv[i]); j++){
		        	       			if(argv[i][j]=='/')
		                	       			z=j;
	     				  		}	
	       						if(z>1){
	             			   			for(int j=0; j<z; j++){
	                     	   					hum[j]=argv[i][j];
	              			 			}
	        					}
	        					else
	                					strcpy(hum,argv[i]);
		
							for(int j=z+1; j<strlen(argv[i]);j++){
								sym[j-z-1]=argv[i][j];
							}

						dirp=opendir(argv[i]);
						wflag = wildcard(argc, argv[i]);
						if(wflag==0||wflag==1)
							dirp=opendir(argv[i]);
						else if(wflag == 5){
							dirp=opendir(".");
						}
						else if(wflag==2)
							dirp=opendir(".");
						else if(wflag == 3)
							dirp=opendir(hum);
						else if(wflag ==4)
							dirp=opendir(hum);
						if(dirp!=NULL){ 
							chdir(argv[i]); // to change directory to get path 	
							//fprintf(file,"<h>directory path : %s\n</h></br>", getcwd(pth,1024));
							Start(rlist);
							while((dir=readdir(dirp))!=NULL){ // inserting including hidden file 
								if(t.f2 == 1)
									RevInsert(rlist, dir,t.f3);
								else if(t.f2 == 2)
									StatInsert(rlist,dir,t.f3);
								else if(t.f2 == 3)
									RevStatInsert(rlist,dir,t.f3);
								else 
									Insert(rlist, dir,t.f3);
							}
							if(wflag==0||wflag==1){
								fprintf(file,"<h>directory path : %s\n</h></br>", getcwd(pth,1024));
								Statprint(rlist, t,file);
								printf("\n");
							}
							else if(wflag == 5 || wflag ==2){
								wc=match(hum,rlist,argv[argc-1],t,wflag,file);
								printf("\n");
							}
							else if(wflag==3){ 
								wc=match2(hum,rlist,sym,t,wflag,file);
								printf("\n");
							}
							else if(wflag == 4){
								wc=match(hum,rlist,sym,t,wflag,file);
							}
						}
					}//print all files's status 
				i++; chdir(cur);// come back to current directory
				}
			}	
		
			else if(argc==optind){ // if there is only option 
				dirp=opendir(".");
				fprintf(file,"<h>directory path : %s\n</h></br>",getcwd(pth,1024));
				Start(rlist);
				while((dir=readdir(dirp))!=NULL){
					if(t.f2 == 1)
						RevInsert(rlist,dir,t.f3);
					else if(t.f2 == 2)
						StatInsert(rlist,dir,t.f3);
					else if(t.f2 == 3)
						RevStatInsert(rlist,dir,t.f3);
					else 
						Insert(rlist, dir,t.f3);
				}
				Statprint(rlist, t,file);
			}
	    }
  
	}

	check(argc, argv, wc);
	
	closedir(dirp);
	fclose(file);
	return 0;
	
}		







