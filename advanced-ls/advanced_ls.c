//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//File Name 		: spls_advanced 																			//
//Date			: 2019/4/12																						//
//Os			: Ubuntu 16.04. LTS 64bits																		//
//Author		: JIN SU LEE																					//
//Student ID 		: 2015722013																				//
//--------------------------------------------------------------------------------------------------------------//
//Title : System Programming Assignment #2-2 (Advanced ls)														//
//Description : 이번 과제는 Assignment #2-1 의 advanced를 수행 하는 것이다. #2-1은 기본적인 ls 기능을 수행 했는데 이는 현재 폴더나 //
//				경로에 있는 폴더들을 다 출력하는 과제였다. 이번 과제인 Abvanced ls는 기본 ls 기능을 포함하고 option flag를 받아와 -a //
//				혹은 -l 혹은 -al와 같은 기능들 까지 추가 하였다. -a는 현재 디렉토리나 경로에 있는 히든 파일들 까지 다 출력 하는 것이고 -l//
//				은 현재 디렉토리나 경로에 있는 파일들의 정보까지 출력해 준다. -al은 -a 와 -l를 모두 수행 시켜 준다. 					//
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

/////////////////////////////////////////////////////////
//Struct Node 										   //
//===================================================  //
//Input : Node* -> name 							   //
//		  Node* -> next 							   //
//Purpose : create a node that stores directory's name //
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
//Option flag 										  //
//====================================================//
//Input : option -a , option -l 					  //
//Output : t - 1 : -a only							  //
//		     - 2 : -l only							  //
// 			 - 3 : -al or -la 						  //
//           - 4 : whatever 						  //
//Purpose : to determine which flag is on 			  //
////////////////////////////////////////////////////////
int Flag(int argc, char *argv[]){
	int aflag=0, lflag=0;
	int c=0;
	int t;

	while((c=getopt(argc,argv, "al"))!=-1){//get option flag 
		switch(c){
		case 'a': // if c is a then aflag is 1 
		aflag=1;
		break;
		case 'l': // if c is l then lflag is 1
		lflag=1;
		break;
		}
	}

	if(aflag == 1 && lflag == 0) 
		t=1;
	else if (lflag == 1 && aflag == 0)
		t=2;
	else if(aflag == 1 && lflag == 1)
		t=3;
	else
		t=4;
	return t;
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
// 			de.										//
//////////////////////////////////////////////////////
int Insert(List *list, struct dirent *dir){
	Node *new = (Node *)malloc(sizeof(Node)); // making new node 
	Node *current = NULL; // current node is null 
	Node *prev = NULL; // pointer for previous node is null 
	struct stat buf;

	new->name = dir->d_name; // new node's name is directory's name 
	new->size = (int)buf.st_mode;
	new->next = NULL; // new node's next pointer is NULl 
	
	if(list->head == NULL) // in case of new is first node in list 
		list->head = new;
	else{ // else 
		current = list->head; 
		while(current!=NULL){
			if(strcasecmp(current->name,new->name)<0){ // if current's name is bigger than new's name 
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

///////////////////////////////////////////////////////////////////////
//Printing a file's status 											 //
//===================================================================//
//Input : current node in list, option flag 						 //
//Output : file status 												 //
//Purpose : to print a current node's full status 					 //
///////////////////////////////////////////////////////////////////////		   
int status(List *list, Node *current){
        unsigned int tmp;
        struct stat buf;
        struct passwd *pwd = NULL;
        struct group *gr = NULL;
        struct tm *mktm= NULL;
        char mon[15];

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
		
		pwd=getpwuid(buf.st_uid); //user name 
		gr=getgrgid(buf.st_gid); // group nae 
		char *tim=ctime(&buf.st_mtime);
		//mktm=localtime(&buf.st_mtime); // last modification time 
		//int m = mktm->tm_mon;
		/*switch(m){ // converting number 0~11 to character 
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
		}*/

		printf("\t%d",(int)buf.st_nlink);
		printf("\t%s\t%s",pwd->pw_name, gr->gr_name);
		//printf("\t%d",(int)buf.st_size);
		printf("\t%d", current->size);
		printf("\t%s",tim);
		printf("\t%s\n",current->name);
		//current=current->next;

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
int Print(List *list, int f){
	Node *current = list->head;
	
	if(f==1 || f==3){ // if option flag is 1 or 3 
		for(int i=0 ; i<list->cnt; i++){
			printf("%s\n",current->name); // print all file including hidden file 
			current=current->next;
		}
	}
	else if(f==2||f==4) { // if option flag is 2 or 4 
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
int Statprint(List *list,  int f){
	Node *current = list->head;
	unsigned int tmp;
	struct stat buf;
	struct passwd *pwd = NULL;
	struct group *gr = NULL;
	struct tm *mktm= NULL;
	char mon[15];
	int blsize=0;

	if(f == 1 || f==4)
		return 0;
	else if(f==2){
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
				status(list, current);
			current= current->next;
		} // printing file status 
	}
	else if(f==3){
		for(int i=0; i<list->cnt; i++){
			if(lstat(current->name, &buf)==0){
				blsize=blsize+buf.st_blocks;
			} // caclulating all files's block size including hidden file 
			current=current->next;
		}
		current = list->head;
		printf("total : %d\n", blsize/2);
		
		for(int i=0; i<list->cnt; i++){
			status(list, current);
			current=current->next;
		}
			
	}
	return 0;
}


int main(int argc, char *argv[]){
	DIR *dirp;
	struct dirent *dir;
	struct stat buf;
	int loop=1,c=0,f=0,s=0;
	unsigned int tmp;
	List *rlist = (List *)malloc(sizeof(List));	
	char pth[1024];
	char cur[1024];
	getcwd(cur,1024); // get current directory's path 
	
	int t = Flag(argc, argv);
	
	while(argv[loop]!=NULL){
		dirp=opendir(argv[loop]);
		if(strncmp(argv[loop],"-",1)){
			if(errno == ENOTDIR) // not directory error code 
				printf("%s\n",argv[loop]);
			else if(dirp==NULL)//not existing directory or file error code
				printf("Cannot access %s : No such file or directory\n", argv[loop]);
		}
		loop++;
	} // print all files which are not existing directory or file 

	if(argc==1){ // if only one input 
		dirp=opendir("."); // open current directory 
		      if(dirp!=NULL){
                	Start(rlist); // start linked list 
                	while((dir=readdir(dirp))!=NULL){
                        	Insert(rlist,dir); // insert node 
                	}
		if(t==1 || t==4) // if option flag is 1 or 4
               	 	Print(rlist, t); 
		else // if option flag is 2 or 3 
			Statprint(rlist, t);
        	}

	}

	else  { // if argc is more than 1 
		if(t==1||t==4){ // if option flag is 1 or 4
			if(argc>optind){ 
				int i=1;	
				while(argv[i]!=NULL){
					if(strncmp(argv[i],"-",1)){ 
						dirp=opendir(argv[i]);
						if(dirp!=NULL){
	         				        Start(rlist);
	                				while((dir=readdir(dirp))!=NULL){
	                        				Insert(rlist,dir);
	               					 }
	                			Print(rlist, t);
	        				}
					}
					i++;
				}
			} // print all file include hidden file 
			else if(argc==optind){ // if there is only option 
				dirp=opendir("."); // open current directory 
				Start(rlist);
				while((dir=readdir(dirp))!=NULL){
					Insert(rlist, dir);}
				Print(rlist,t);
			}
		}	
		else if(t==2||t==3) { // if option flag is 2 or 3 
			if(argc>optind){  
				int i=1;
				while(argv[i]!=NULL){
					if(strncmp(argv[i],"-",1)){
						dirp=opendir(argv[i]);
						if(dirp!=NULL){
							chdir(argv[i]); // to change directory to get path 
							printf("directory path : %s\n", getcwd(pth,1024));
							Start(rlist);
							while((dir=readdir(dirp))!=NULL){
								Insert(rlist, dir);
							}
							Statprint(rlist, t);
						
						}
					}//print all files's status 
				i++; chdir(cur);// come back to current directory
				}
			}	
		
			else if(argc==optind){ // if there is only option 
				dirp=opendir(".");
				printf("directory path : %s\n",getcwd(pth,1024));
				Start(rlist);
				while((dir=readdir(dirp))!=NULL)
					Insert(rlist, dir);
				Statprint(rlist, t);
			}
	    }
  
	
	}
	closedir(dirp);
	return 0;
	
}		

