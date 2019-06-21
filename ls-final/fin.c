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

typedef struct Flag{
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

	lflag<<=4;
	aflag<<=3;
	sflag<<=1;

	f.f1 = lflag+aflag;
	f.f2 = sflag+rflag;
	f.f3 = hflag;

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

	new->name = dir->d_name; // new node's name is directory's name 
	lstat(new->name, &buf);
	new->size = (int)buf.st_size;

	new->next = NULL; // new node's next pointer is NULl 
	
	if(list->head == NULL) // in case of new is first node in list 
		list->head = new;
	else{ // else 
		current = list->head; 
		while(current!=NULL){
			if(current->size < new->size){ // if current's name is bigger than new's name 
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
			if(current->size > new->size){ // if current's name is bigger than new's name 
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

   
int status(List *list, Node *current, int h){
        unsigned int tmp;
        struct stat buf;
        struct passwd *pwd = NULL;
        struct group *gr = NULL;
        struct tm *mktm= NULL;
        char mon[15];
        lstat(current->name,&buf);
        int pw = pow(1024,4);

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
				printf("\t%dG",(int)current->size/pw);
			else
				printf("\t%dk",(int)current->size/1024);
		}
		else
			printf("\t%d",(int)current->size);
		printf("\t%s  %d  %d:%d",mon,mktm->tm_mday,mktm->tm_hour,mktm->tm_min);
		printf("\t%s\n",current->name);

		return 0;
}


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

int wildcard(int argc, char *wild){
	int wflag=0;
	int len=strlen(wild);

	for(int i=0 ; i<len ; i++){
		if(wild[i]=='*')
			wflag=1;
		else if(wild[i] == '?' || wild[i] == '[')
			wflag=2;
	}

	return wflag;
}

int match(List *list, char *wild, Flag f, int w){
	Node *current = list->head;
	DIR *dirp;
	struct dirent *dir;
	char tmp[1024];
	char cur[1024];
	getcwd(cur,1024);
	List *newlist=(List *)malloc(sizeof(List));
	struct stat buf;
	lstat(current->name,&buf);
	

	for(int i=0 ; i<list->cnt; i++){
			chdir(current->name);
			
			if(!fnmatch(wild,getcwd(tmp,1024),FNM_PATHNAME)){
				if(f.f1 != 8){
					if(strncmp(current->name, ".",1)){
						if(f.f1 == 0)
							printf("%s\n",current->name);
						else if(f.f1 == 16){
							status(list, current, f.f3);
						}
					}
				}
				else {
					if(f.f1 == 24)
						status(list, current, f.f3);
					
					else 
						printf("%s\n",current->name);
				}
			
		}
		current=current->next;
	}
	if(w==1){
		current= list->head;

		for(int i=0; i<list->cnt; i++){
			if(strncmp(current->name,".",1)){
				dirp=opendir(current->name);
				if(dirp!=NULL){
					chdir(current->name);
					printf("directory path: %s\n",getcwd(tmp,1024));
					Start(newlist);
					while((dir=readdir(dirp))!=NULL){

						Insert(newlist,dir,f.f3);
					}
					Print(newlist, f);
				}
			}
			

			current=current->next;
			chdir(cur);
		}
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
	wflag = wildcard(argc, argv[argc-1]);

	int st = strlen(argv[argc-1]);
	char *hum=(char *)malloc(sizeof(char));
	strcpy(hum,argv[argc-1]);
	hum[st-1]='.';
	printf("%s\n",hum);

	while(argv[loop]!=NULL){
		dirp=opendir(argv[loop]);
		if(strncmp(argv[loop],"-",1)){
				if(errno == ENOENT)//not existing directory or file error code
					printf("Cannot access %s : No such file or directory\n", argv[loop]);
				else if(errno == ENOTDIR) // not directory error code 
					printf("%s\n",argv[loop]);
		}
		if(wflag==1){
			if(loop==argc-1)
				break;
			loop++;
		}
		else
			loop++;
	} // print all files which are not existing directory or file 


	if(argc==1){ // if only one input 
		dirp=opendir("."); // open current directory 
		if(dirp!=NULL){
            Start(rlist); // start linked list 
            while((dir=readdir(dirp))!=NULL){
            	Insert(rlist,dir,t.f3); // insert node 
        	}
			if(t.f1==8 || t.f1==0) // if option flag is 1 or 4
               	Print(rlist, t); 
			else // if option flag is 2 or 3 
				Statprint(rlist, t);
        }

	}

	else  { // if argc is more than 1 
		if(t.f1==8||t.f1==0){ // if option flag is 1 or 4 == a or x
			if(argc>optind){ 
				int i=1;	
				while(argv[i]!=NULL){
					if(strncmp(argv[i],"-",1)){ 
						if(wflag==0)
							dirp=opendir(argv[i]);
						else if(wflag == 1){
							dirp=opendir(hum);
						}
						else if(wflag==2)
							dirp=opendir(".");
						if(dirp!=NULL){
	         				        Start(rlist);
	                				while((dir=readdir(dirp))!=NULL){
	                        				if(t.f2 == 1)
	                        					RevInsert(rlist,dir,t.f3);
	                        				else
	                        					Insert(rlist,dir,t.f3);
	               					 }
	               				if(wflag == 0)
	                				Print(rlist, t);
	                			else{
	                				if(argc>2){
	                					match(rlist, argv[argc-1],t,wflag);
	                				}
	                				else 
	                					match(rlist, argv[argc-1],t,wflag);
	                			}
	        				}
					}
					i++;
				}
			} // print all file include hidden file 
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
		else if(t.f1==16||t.f1==24) { // if option flag is 2 or 3  ==> l or al
			if(argc>optind){  
				int i=1;
				while(argv[i]!=NULL){
					if(strncmp(argv[i],"-",1)&&strncmp(argv[argc-1],"'",1)){
						dirp=opendir(argv[i]);
						if(dirp!=NULL){
							chdir(argv[i]); // to change directory to get path 
							printf("directory path : %s\n", getcwd(pth,1024));
							Start(rlist);
							while((dir=readdir(dirp))!=NULL){
								if(t.f2 == 1)
									RevInsert(rlist, dir,t.f3);
								else if(t.f2 == 2)
									StatInsert(rlist,dir,t.f3);
								else if(t.f2 == 3)
									RevStatInsert(rlist,dir,t.f3);
								else 
									Insert(rlist, dir,t.f3);
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
	
	
	closedir(dirp);
	return 0;
	
}		

