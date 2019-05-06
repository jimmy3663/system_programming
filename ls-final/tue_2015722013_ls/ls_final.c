//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//File Name 	: tue_2-3_2015722013_ls.c 																		//
//Date			: 2019/4/28 																					//
//Os			: Ubuntu 16.04. LTS 64bits																		//
//Author		: JIN SU LEE																					//
//Student ID 	: 2015722013																					//
//--------------------------------------------------------------------------------------------------------------//
//Title 		: System Programming Assignment #2-3 (ls - final)												//
//Description 	: 이번 과제는 #2-1 과 #2-2의 연장 선으로써 wild card와 r, s, h 라는 option이 추가 되었다. wild card는 특수 문자//
//				  로써 *을 입력하면 해당 파일에 있는 모든 파일과 디렉토리들을 출력하며 디렉토리라면 해당 디렉토리의 파일들 까지 다 출력해준다.//
// 				  ?는 예를 들어 ?.txt면 한글자.txt를 모두 출력하게 된다. 마지막으로 sequence가 있는데 [1-3].txt면 1~3.txt의 파일이// 
//				  모두 출력되게 된다. -r은 이름 정렬을 내림차순으로 정력 해주고 -s는 file의 size 크기 별로 오름차순으로 정렬 해서 보여준 //
// 				  다. -h는 file의 size를 사람이 보기 좋은 단위 즉 G, K로 표현 해서 보여준다.									//		
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////			
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

/////////////////////////////////////////////////////////
//Struct Node 										   //
//===================================================  //
//Input : Node* -> name 							   //
//		  Node* -> next 							   //
//		  Node* -> size 							   //
//Purpose : create a node that stores directory's name //
//			and size 								   // 
/////////////////////////////////////////////////////////

typedef struct NODE{
	char *name;  // node 의 이름 
	int size;
	struct NODE *next; // 다음 node를 가르키고 있는 포인터 
}Node;

///////////////////////////////////////////////////////
//Struct List 										 //
//================================================== //
//Input : List -> cnt 								 //
//	      List* -> head node 						 //
//Purpose : create a linked list about Node 		 // 
///////////////////////////////////////////////////////
typedef struct LIST{
	int cnt; // List의 갯수 
	Node *head; // List의 시작 지점을 가르키는 포인터 
}List;

///////////////////////////////////////////////////////
//Struct Flag 										 //
//================================================== //
//Input : f1 -> a or l flag 						 //
//	      f2 -> r or s flag  						 //
// 		  f3 -> h Flag 								 //
//Purpose : to make 3 return value from flag function// 
///////////////////////////////////////////////////////
typedef struct Flag{ // flag의 return 값을 3개로 나눈다. 
	int f1;
	int f2;
	int f3;
}Flag;

///////////////////////////////////////////////////////
//Start list 										 //
//================================================== //
//Input : list -> cnt = 0 							 //
//	      list -> head = NULL 						 //
//Purpose : to initialize a list 					 // 
///////////////////////////////////////////////////////
void Start(List *list){
	list->cnt=0; // list 의 갯수를 0으로 초기화 한다. 
	list->head=NULL; // list 의 첫번째 노드를 NULL로 초기화 한다.
}

////////////////////////////////////////////////////////
//Option flag2 										  //
//====================================================//
//Input : option -a , option -l, option -s, option -r,//
//		  option -h 								  //
//Output : t - 8 : -a 								  //
//		     - 16 : -l 								  //
// 			 - 2 : -s 								  //
//           - 1 : -r 								  //
// 			 - 1 : -h(but diffrent retrun value)	  // 
// 			add flag and return it 					  // 
//Purpose : to determine which flag is on 			  //
////////////////////////////////////////////////////////
Flag Flag2(int argc, char *argv[]){
	Flag f;
	int aflag=0, lflag=0, hflag=0, rflag=0, sflag=0;
	int c=0;
	int t;

	while((c=getopt(argc,argv, "alhrs"))!=-1){//get option flag 
		switch(c){
			printf("1\n");
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

//////////////////////////////////////////////////////
//Insert a Node to lsit 							//
//==================================================//
//Input : new -> store directory's name 			//
// 				 if new's directory's name is bigger//
//				 than lasting node's name then 		//
//				 insert the new node next to current//
//     			 node 								//
//Purpose : sort a new node while inserting a new no//
// 			de.	(ascending sort)					//
//////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////
//Insert a Node to lsit(sorting by size) 							//
//==================================================================//
//Input : new -> store directory's name and size 					//
// 				 if new's directory's size is bigger				//
//				 than lasting node's size then 						//
//				 insert the new node next to current 				//
//     			 node. if file's sizes are the same sort by the name//
//Purpose : sort a new node while inserting a new node 				//
// 			(ascending sort by size)								//
//////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////
//Insert a Node to lsit(descending sort) 							//
//==================================================================//
//Input : new -> store directory's name 		 					//
// 				 if new's directory's size is smaller				//
//				 than lasting node's size then 						//
//				 insert the new node next to current 				//
//     			 node. 												//
//Purpose : sort a new node while inserting a new node 				//
// 			(descending sort )										//
//////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////
//Insert a Node to lsit(sorting by size + descending sort) 			//
//==================================================================//
//Input : new -> store directory's name and size 					//
// 				 if new's directory's size is smaller				//
//				 than lasting node's size then 						//
//				 insert the new node next to current 				//
//     			 node. if file's sizes are the same sort by the name//
//Purpose : sort a new node while inserting a new node 				//
// 			(descending sort by size)								//
//////////////////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////////////////
//Printing a file's status 											 //
//===================================================================//
//Input : current node in list, option flag 						 //
//Output : file status 												 //
//Purpose : to print a current node's full status 					 //
///////////////////////////////////////////////////////////////////////	
int status(List *list, Node *current, int h){
        unsigned int tmp;
        struct stat buf;
        struct passwd *pwd = NULL;
        struct group *gr = NULL;
        struct tm *mktm= NULL;
        char mon[15];
        lstat(current->name,&buf);
        long int pw = pow(1024,4);

		if(lstat(current->name,&buf)==0){ // if current's name is written on buf correctly 
			if(S_ISREG(buf.st_mode)) // if file's mode is regular file 
				 printf("-");
			else if(S_ISDIR(buf.st_mode))//if file's mode is directory
				 printf("d");
			else if(S_ISCHR(buf.st_mode))//if file's mode is character device 
				 printf("c");
			else if(S_ISBLK(buf.st_mode))//if file's mode is block device 
				 printf("b");
			else if(S_ISLNK(buf.st_mode))//if file's mode is symbolic link 
				  printf("l");
			else if(S_ISSOCK(buf.st_mode))//if file's mode is socekt 
				  printf("s");
		}
		tmp=buf.st_mode; //current name path's authority 
		for(int i=0; i<3; i++){
            if(tmp & S_IRUSR) // comparing read authority 
       	        printf("r");     
            else
        		printf("-");
			if(tmp & S_IWUSR) // comparing write authority 
				printf("w");
			else
				printf("-");
			if(tmp & S_IXUSR) // comparing execute authority 
				printf("x");
			else
				printf("-");    
			tmp<<=3; // 3bit shifting to left 
		}

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
		printf("\t%d",(int)buf.st_nlink);
		printf("\t%s\t%s",pwd->pw_name, gr->gr_name);
		if(h==1){
			if(buf.st_size<1024)
				printf("\t%d",(int)current->size);
			else if(buf.st_size>pw)
				printf("\t%liG",(int)current->size/pw);
			else
				printf("\t%dk",(int)current->size/1024);
		}
		else
			printf("\t%d",(int)current->size);
		printf("\t%s  %d  %d:%d",mon,mktm->tm_mday,mktm->tm_hour,mktm->tm_min);
		printf("\t%s\n",current->name);

		return 0;
}

/////////////////////////////////////////////////////////////////////////////////
//Print directory's files name 												   //
//=============================================================================//
//Input : list, option flag 												   //
//Ouput : if option flag is 1 or 3 -> print all file name including hidden file//
// 		  if option flag is 2 or 4 -> print all file name except for hidden fil//
//		  e. 																   //
//Purpose : to print file name 												   //
/////////////////////////////////////////////////////////////////////////////////
int Print(List *list, Flag f){
	Node *current = list->head;
	
	if(f.f1==8 || f.f1==24){ // if option flag is 1 or 3 
		for(int i=0 ; i<list->cnt; i++){
			printf("%s\n",current->name); // print all file including hidden file 
			current=current->next;
		}
	}
	else if(f.f1==16||f.f1==0) { // if option flag is 2 or 4 
		for(int i=0; i<list->cnt; i++){
			if(strncmp(current->name,".",1)){  // comparing first letter with "."
				printf("%s\n",current->name); // print all file except for hidden file
			}
			current=current->next;
		}
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////
//print file's status 															 //
//===============================================================================//
//Input : list, option flag 													 //
//Output : if option flag is 1 or 4 -> print nothing 							 // 
// 		   if option flag is 2 -> print file status without hidden file 		 //
//         if option flag is 3 -> print file status including hidden file 		 //
//Purpose : to determine option flag to print or not print a hidden file 		 //
///////////////////////////////////////////////////////////////////////////////////
int Statprint(List *list,  Flag f){
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
		printf("total : %d\n",(int)blsize);	
		current=list->head;
	
		for(int i=0; i<list->cnt;  i++){ 
			if(strncmp(current->name,".",1))
				status(list, current, f.f3);
			current= current->next;
		} // printing file status 
	}
	else if(f.f1==24){
		for(int i=0; i<list->cnt; i++){
			if(lstat(current->name, &buf)==0){
				blsize=blsize+buf.st_blocks;
			} // caclulating all files's block size including hidden file 
			current=current->next;
		}
		current = list->head;
		printf("total : %d\n", blsize/2);
		
		for(int i=0; i<list->cnt; i++){
			status(list, current, f.f3);
			current=current->next;
		}
			
	}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////
//determine which wild card 													 //
//===============================================================================//
//Input : argc, *wild (argv[i])													 //
//Output : if there is / and ? or [] then 4. 									 //
//		   if there is / and * then 3											 //
//		   if there is only * then 5											 //
//		   if there is only ? or [] then 4 										 //
//		   if there is only / then 1 											 //
//Purpose : determining wildcard type 											 //
///////////////////////////////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////////////////////////////
//match function 1 																 //
//===============================================================================//
//Input : *wild (only wild card or path + wild card)							 //
//		  Flag value and wild card value 										 //
//Outout : print the matching node's name or status depending on option flag 	 //
//Purporse : match function for *, ?, [] 										 //
///////////////////////////////////////////////////////////////////////////////////
int match(char *hum, List *list, char *wild, Flag f, int w){
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

		for(int i=0; i<list->cnt; i++){
			dirp=opendir(current->name);
			if(!fnmatch(wild,current->name,0)){ // if current->name and wild is matched 
				if(dirp==NULL){ 
				if(f.f1 != 8 && f.f1 != 24){ // depending on option flag
				if(strncmp(current->name,".",1)){ // without hidden file 
                    if(f.f1 !=16){
						printf("%s\n", current->name); // print only name 
					}
                    else if(f.f1 == 16) {
						status(newlist, current, f.f3); // print status of the file 
					}
				}
				}
				else if(f.f1 == 8 || f.f1 ==24){
					if(f.f1 == 24){
						status(newlist, current, f.f3);}
					else{
						printf("%s\n",current->name);}
				}wcnt++;
			}
			}
			current=current->next; // move to next node 
		 
		}

	if(w==5){ // if wild card is only * 
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
						Print(newlist, f);
					else 
						Statprint(newlist,f);
				}
			}
			
			current=current->next;
			chdir(cur);//change path to original path 
		}
	}
	return wcnt;
}

///////////////////////////////////////////////////////////////////////////////////
//match function 2																 //
//===============================================================================//
//Input : *wild (only wild card or path + wild card)							 //
//		  Flag value and wild card value 										 //
//Outout : print the matching node's name or status depending on option flag 	 //
//		   if there is directory in current directory then print the directory's //
//		   content.																 //
//Purpose : match function for * with path										 //
///////////////////////////////////////////////////////////////////////////////////
int match2(char *hum, List *list, char *wild, Flag f, int w){
	Node *current = list->head;
	DIR *dirp;
	struct dirent *dir;
	char tmp[1024];
	char cur[1024];
	chdir(hum);
	List *newlist=(List *)malloc(sizeof(List));
	int wcnt =0;

/////////////////////////////// if there is match print /////////////////////////////
	for(int i=0 ; i<list->cnt; i++){
			getcwd(cur,1024); // change direcotry to original path 
			if(!fnmatch(wild,current->name,0)){ 
				if(f.f1 != 8){
					if(strncmp(current->name, ".",1)){
						dirp = opendir(current->name);
						if(dirp==NULL){
							if(f.f1 == 0)
								printf("%s/%s\n",getcwd(tmp,1024),current->name);
							else if(f.f1 == 16||f.f1==24){
								status(list, current, f.f3);
							}
						}
					}
				}
				else if(f.f1==8) {
					dirp=opendir(current->name);
					if(dirp==NULL){
						printf("%s/%s\n",getcwd(tmp,1024),current->name);
					}
				}
		wcnt++;	
		}chdir(cur);
		current=current->next;
	}
///////////////////////// printing directory which is in directory /////////////////////
		current= list->head;
		for(int i=0; i<list->cnt; i++){
			if(strncmp(current->name,".",1)){
				dirp=opendir(current->name);
				if(dirp!=NULL){
					chdir(current->name);
					printf("directory path: %s:\n",getcwd(tmp,1024));
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
							Print(newlist, f);
						else if(f.f1==16 || f.f1 == 24)
							Statprint(newlist,f);
					}
					else if(f.f1 == 8)
						Print(newlist,f);
				}
			}
			
			current=current->next;
			chdir(cur); // change to original path 
		}
	
	return wcnt;
}

///////////////////////////////////////////////////////////////////////////////////
//checing function if input is not direcort 									 //
//===============================================================================//
//Input : argc, argv, matching function return 									 //
//Outout : if it is not directory nor file and no matching with wild card then 	 //
//		   print error 															 //
//Purpose : determinig rather if it's error or not 								 //
///////////////////////////////////////////////////////////////////////////////////
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
	printf("%d\n",t.f2);
//////////////////if there is only one argv then open . directroy ///////////////////
	if(argc==1){ // if only one input 
	dirp=opendir("."); // open current directory 
	if(dirp!=NULL){
            Start(rlist); // start linked list 
            while((dir=readdir(dirp))!=NULL){
            	Insert(rlist,dir,t.f3); // insert node 
        	}
			if(t.f1==8 || t.f1==0) // depending on flag option a or l 
               	Print(rlist, t); 
			else 
				Statprint(rlist, t);
        }

	}
//////////////////////////////////////////////////////////////////////////////////////

//////////////////// if there is more than one argv //////////////////////////////////
	else  { // if argc is more than 1 
		if(t.f1==8||t.f1==0){ // depending on option flag a, l 
			if(argc>optind){ 
				int i=1;	
				while(argv[i]!=NULL){
					if(strncmp(argv[i],"-",1)){ 
////////////////////////// dividing argv by / in case current argv contains wild card ///////////////
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
						printf("1: %s\n",hum);
						printf("2: %s\n",sym);
						printf("3: %s\n",argv[i]);
//////////////////////////////////////////////////////////////////////////////////////////////////////
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
	                				Print(rlist, t);
								printf("\n");
								}
//////////////////////////////////// executing match if there is wild card symbol in argv ////////////////
	                			else if(wflag == 5 || wflag==2){
	                				wc=match(hum,rlist, argv[i],t,wflag);
									printf("\n");
	                			}
	                			else if(wflag == 3){
	                				wc=match2(hum,rlist,sym,t,wflag);
							//wc = match(hum,rlist,sym,t,wflag);
									printf("\n");
								}
								else if(wflag == 4){
									wc = match(hum, rlist, sym, t, wflag);
								}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
	        			}
					free(hum); // freeing hum 
					free(sym); // freeing sym 
					}
					i++;
				}
			} 
////////////////////// if argc == optind then there is no wild card symbol ////////////////////////////
//							no need to consider wild card  
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
				Print(rlist,t);
			}
		}	

		else if(t.f1==16||t.f1==24) { // depending on option flag a or l 
			if(argc>optind){  
				int i=1;
				while(argv[i]!=NULL){
					if(strncmp(argv[i],"-",1)){
////////////////////////// dividing argv by / in case current argv contains wild card ///////////////
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
////////////////////////////////// same as above ///////////////////////////////////////////////////
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
						if(dirp!=NULL){ 
							chdir(argv[i]); // to change directory to get path 
							printf("directory path : %s\n", getcwd(pth,1024));
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
								Statprint(rlist, t);
								printf("\n");
							}
							else if(wflag == 5 || wflag ==2){
								wc=match(hum,rlist,argv[argc-1],t,wflag);
								printf("\n");
							}
							else if(wflag==3){ 
								wc=match2(hum,rlist,argv[argc-1],t,wflag);
								printf("\n");
							}
							else if(wflag == 4){
								wc=match(hum,rlist,sym,t,wflag);
							}
						}
					}//print all files's status 
				i++; chdir(cur);// come back to current directory
				}
			}	
		
			else if(argc==optind){ // if there is only option 
				dirp=opendir(".");
				printf("directory path : %s\n",getcwd(pth,1024));
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
				Statprint(rlist, t);
			}
	    }
  
	}
	//match(hum,rlist,argv[argc-1],t,wflag);
	check(argc, argv, wc);
	
	closedir(dirp);
	return 0;
	
}		






