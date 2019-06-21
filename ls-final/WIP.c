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
	}

	return wflag;
}

int match(char *hum, List *list, char *wild, Flag f, int w){
	Node *current = list->head;
	DIR *dirp;
	struct dirent *dir;
	char tmp[1024];
	char cur[1024];
	chdir(hum);
	getcwd(cur,1024);
	List *newlist=(List *)malloc(sizeof(List));
	struct stat buf;
	lstat(current->name,&buf);
	int wcnt=0;

	if(w == 2 || w==1){
		for(int i=0; i<list->cnt; i++){
			dirp=opendir(current->name);
			if(!fnmatch(wild,current->name,0)){
				if(dirp==NULL){
				if(f.f1 != 8 || f.f1 != 24){
				if(strncmp(current->name,".",1)){
					if(f.f1 !=16){
						printf("%s\n", current->name);
					}
					else if(f.f1 == 16) {
						status(newlist, current, f.f3);
					}
				}
				}
				else if(f.f1 == 16 || f.f1 ==24){
					if(f.f1 == 24&&f.f1==16)
						status(newlist, current, f.f3);
					else 
						printf("%s\n",current->name);
				}wcnt++;
			}
			}
			current=current->next;
		 
		}
	}

	if(strcmp(hum,".")==0){
		current= list->head;
		for(int i=0; i<list->cnt; i++){
			if(strncmp(current->name,".",1)){
				dirp=opendir(current->name);
				if(dirp!=NULL){
					chdir(current->name);
					printf("directory path: %s:\n",getcwd(tmp,1024));
					Start(newlist);
					while((dir=readdir(dirp))!=NULL){
						Insert(newlist,dir,f.f3);
					}
					if(f.f1!=16&&f.f1!=24)
						Print(newlist, f);
					else 
						Statprint(newlist,f);
				}
			}
			
			current=current->next;
			chdir(cur);
		}
	}
	return wcnt;
}
int match2(char *hum, List *list, char *wild, Flag f, int w){
	Node *current = list->head;
	DIR *dirp;
	struct dirent *dir;
	char tmp[1024];
	char cur[1024];
	chdir(hum);
	List *newlist=(List *)malloc(sizeof(List));
	int wcnt =0;

	for(int i=0 ; i<list->cnt; i++){
		//	chdir(current->name);
		//	printf("%s\n",getcwd(tmp,1024));
			if(!fnmatch(getcwd(cur,1024),getcwd(tmp,1024),0/*FNM_PATHNAME*/)){
				if(f.f1 != 8){
					if(strncmp(current->name, ".",1)){
						dirp = opendir(current->name);///////////
						if(dirp==NULL){
							if(f.f1 == 0)
								printf("%s/%s\n",getcwd(tmp,1024),current->name);
							else if(f.f1 == 16){
								status(list, current, f.f3);
							}
						}
					}
				}
				else {
					dirp=opendir(current->name);
					if(dirp==NULL){
					if(f.f1 == 24)
							status(list, current, f.f3);	
						else 
							printf("%s\n",current->name);
					}
				}
		wcnt++;	
		}chdir(cur);
		current=current->next;
	}
		current= list->head;
		//chdir(current->name);
		//getcwd(tmp,1024);
		//printf("\n%s\n",getcwd(tmp,1024));
		for(int i=0; i<list->cnt; i++){
			if(strncmp(current->name,".",1)){
				dirp=opendir(current->name);
				if(dirp!=NULL){
					chdir(current->name);
					printf("directory path: %s:\n",getcwd(tmp,1024));
					Start(newlist);
					while((dir=readdir(dirp))!=NULL){
						Insert(newlist,dir,f.f3);
					}
					if(f.f1 != 16)
						Print(newlist, f);
					else if(f.f1 == 16 || f.f1 == 24)
						Statprint(newlist,f);
				}
			}
			
			current=current->next;
			chdir(cur);
		}
	
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
			else if(dirp==NULL){//not existing directory or file error code
                if(m == 0)
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
	char *hum=(char*)malloc(sizeof(char));
	char *sym=(char*)malloc(sizeof(char));
	int z=0;
	for(int i=0; i<strlen(argv[argc-1]); i++){
        	        if(argv[argc-1][i]=='/')
                	        z=i;
       	 	}
       	if(z>1){
                for(int i=0; i<z; i++){
                        hum[i]=argv[argc-1][i];
               	 }
        }
        else
                strcpy(hum,argv[argc-1]);
	
	for(int i=z; i<strlen(argv[argc-1]);i++){
		sym[i]=argv[argc-1][i];
	}
	

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
						wflag = wildcard(argc, argv[i]);
						if(wflag==0)
							dirp=opendir(argv[i]);
						else if(wflag == 1){
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
	                        				else
	                        					Insert(rlist,dir,t.f3);
	               					 }
	               				if(wflag == 0){
	                				Print(rlist, t);
								printf("\n");
								}
	                			else if(wflag == 1 || wflag==2){
	                				wc=match(hum,rlist, argv[argc-1],t,wflag);
									printf("\n");
	                			}
	                			else if(wflag == 3){
	                				wc=match2(hum,rlist,argv[argc-1],t,wflag);
									printf("\n");
								}
								else if(wflag == 4){
									wc = match(hum, rlist, sym, t, wflag);
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
						wflag = wildcard(argc, argv[i]);
						printf("%d\n",wflag);
						if(wflag==0)
							dirp=opendir(argv[i]);
						else if(wflag == 1){
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
							if(wflag==0){
								Statprint(rlist, t);
								printf("\n");
							}
							else if(wflag == 1 || wflag ==2){
								wc=match(hum,rlist,argv[argc-1],t,wflag);
								printf("\n");
							}
							else{ 
								wc=match2(hum,rlist,argv[argc-1],t,wflag);
								printf("\n");
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


