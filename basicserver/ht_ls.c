#include "ht_ls.h"

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


/*Flag Flag2(int argc, char *argv[]){
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
}*/



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


int status(List *list, Node *current, FILE* file){
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
        	fprintf(file,"<tr style=color:blue><td><a href=%s>%s\n</a></td>",getcwd(kk,1024),current->name); //if directory color it with blue 
        	chdir(cur);
        }
        else if(S_ISLNK(buf.st_mode)){    	
    		chdir(current->name);
        	fprintf(file,"<tr style=color:green><td><a href=%s/%s>%s\n</a></td>",getcwd(kk,1024),current->name,current->name); // if link, color it with green
        }
        else{
        	chdir(current->name);
        	//printf("%s/%s\n",getcwd(kk,1024),current->name);
        	fprintf(file,"<tr style=color:red><td><a href=%s/%s>%s\n</a></td>",getcwd(kk,1024),current->name,current->name);// rest, color it with red 
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
		fprintf(file,"<td>%d</td>",(int)current->size);
		fprintf(file,"<td>%s  %d  %d:%d</td>",mon,mktm->tm_mday,mktm->tm_hour,mktm->tm_min);
		
		fprintf(file,"</tr>"); // end of the table row 
		return 0;
}

/*int Print(List *list, Flag f, FILE *file){
	char cur[1024];
	Node *current = list->head;
	fprintf(file,"<body>"); // start body 
	fprintf(file,"<TABLE>"); // mark start of the table 
	fprintf(file, "<table border=1>"); // type of the table 
	fprintf(file, "<tr><th>File Name</th></tr>"); // table head 
	if(f.f1==8 || f.f1==24){ // if option flag is 1 or 3 
		for(int i=0 ; i<list->cnt; i++){
			if(strcmp(current->name,"html_ls.html")!=0){
				fprintf(file,"<tr><td><a href=%s>%s</a></td></tr>\n",getcwd(cur,1024),current->name); // print directory or file's name and hyper link it with absolute path.
			}  
			current=current->next;
		}
	}
	else if(f.f1==16||f.f1==0) { // if option flag is 2 or 4 
		for(int i=0; i<list->cnt; i++){
			if(strncmp(current->name,".",1)&&strcmp(current->name,"html_ls.html")!=0) {  // comparing first letter with "."
				fprintf(file,"<tr><td><a href=%s/%s>%s</a></td></tr>\n",getcwd(cur,1024),current->name,current->name); // print directory or file's name and hyper link with absolute path. 
			}
			current=current->next;
		}
	}
	fprintf(file,"</TABLE>"); // end of table 
	fprintf(file,"</body>"); // end of body 
	return 0;
}*/


int Statprint(List *list, FILE *file){
	Node *current = list->head;
	unsigned int tmp;
	struct stat buf;
	struct passwd *pwd = NULL;
	struct group *gr = NULL;
	struct tm *mktm= NULL;
	char mon[15];
	int blsize=0;

	//else if(f.f1==16){
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
//		fprintf(file,"<div>");
		fprintf(file,"<h><b>total : %d\n</b></h><br>",(int)blsize);	//print tottal in head with bold words
		//fprintf(file, "<table border=1>");
		fprintf(file,"<body>"); // start of body 
		fprintf(file,"<TABLE>"); // mark start of table 
		fprintf(file,"<table border=1>"); // table's type 
		fprintf(file, "<tr><th>File Name</th><th>Permission</th><th>Link</th><th>Owner</th><th>Group</th><th>Size</th><th>Last Modified</th></tr>"); // table's head 
		
		current=list->head;
	
		for(int i=0; i<list->cnt;  i++){ 
			if(strncmp(current->name,".",1)&&strcmp(current->name,"html_ls.html")!=0) // print except for hidden file and 
				status(list, current, file);
			current= current->next;
		} // printing file status 
		fprintf(file,"</TABLE>"); // mark end of table 
		fprintf(file,"</body>"); // mark end of body 
	//}

	/*else if(f.f1==24){
		for(int i=0; i<list->cnt; i++){
			if(lstat(current->name, &buf)==0){
				blsize=blsize+buf.st_blocks;
			} // caclulating all files's block size including hidden file 
			current=current->next;
		}
		current = list->head;
		fprintf(file,"<div>");
		fprintf(file,"<h>total : %d\n</h>", blsize/2); // print total size of file in head area 
		//fprintf(file, "<table border=1>");
		fprintf(file,"<body>"); // mark start of the body 
		fprintf(file,"<TABLE>"); // start of the table 
		fprintf(file,"<table border=1>"); // table's type 
		fprintf(file, "<tr><th>File Name</th><th>Permission</th><th>Link</th><th>Owner</th><th>Group</th><th>Size</th><th>Last Modified</th></tr>"); // printf table headers
		
		
		for(int i=0; i<list->cnt; i++){
			if(strcmp(current->name,"html_ls.html")!=0) // print except for html file. 
				status(list, current, f.f3,file);
			current=current->next;
		}
		fprintf(file,"</TABLE>"); // end of thetalbe 
		fprintf(file,"</body>");// end of the body 
		fprintf(file,"</div>"); // end of division 
			
	}*/
	fprintf(file,"<br>"); // make a new line 
	return 0;
}

/*int wildcard(int argc, char *wild){
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
}*/


/*int match(char *hum, List *list, char *wild, Flag f, int w, FILE *file){
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
	fprintf(file,"<body>"); // mark start of the body 
        fprintf(file,"<TABLE>"); // mark start of the table 
        fprintf(file,"<table border=1>"); // table type 

	if(f.f1==16||f.f1==24){
                fprintf(file, "<tr><th>File Name</th><th>Permission</th><th>Link</th><th>Owner</th><th>Group</th><th>Size</th><th>Last Modified</th></tr>"); // table's headers
        }
        else if(f.f1==0 || f.f1==8)
                fprintf(file,"<tr><th>File Name</th></tr>"); // table's head 

		for(int i=0; i<list->cnt; i++){
			dirp=opendir(current->name);
			if(!fnmatch(wild,current->name,0)){ // if current->name and wild is matched 
				if(dirp==NULL){ 
				if(f.f1 != 8 && f.f1 != 24){ // depending on option flag
				if(strncmp(current->name,".",1)){ // without hidden file 
                    if(f.f1 !=16){
						//printf("%s\n", current->name); // print only name 
					 fprintf(file,"<tr><td><a href=%s/%s>%s\n</a></td></tr>",getcwd(tmp,1024),current->name,current->name); // print directory or file's name and hyper link it with absolute path
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
						 fprintf(file,"<tr><td><a href=%s/%s>%s/%s\n</a></td></tr>",getcwd(tmp,1024),current->name,current->name,current->name); // print directory of file's name and hyper link it with absolute path 
					}
				}wcnt++;
			}
			}
			current=current->next; // move to next node 
		 
		}
	fprintf(file,"</body>"); // mark end of body 
	fprintf(file,"</TABLE>"); // mark end of table 
	if(w==5&&strlen(wild)==1){ // if wild card is only * 
		current= list->head; // current is list head 
		for(int i=0; i<list->cnt; i++){
			if(strncmp(current->name,".",1)){ // except for hidden file 
				dirp=opendir(current->name);
				if(dirp!=NULL){
					chdir(current->name); // change directory to current->name's path
					//printf("directory path: %s:\n",getcwd(tmp,1024)); // print current directory path 
					fprintf(file,"<h><b>directory path : %s\n</b></h></br>", getcwd(tmp,1024)); // print directory's path on header area 
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

	fprintf(file,"<body>"); // mark start of body area 
	fprintf(file,"<TABLE>"); // mark start of table 
	fprintf(file,"<table border=1>"); // table's type 
	if(f.f1==16||f.f1==24){
		fprintf(file, "<tr><th>File Name</th><th>Permission</th><th>Link</th><th>Owner</th><th>Group</th><th>Size</th><th>Last Modified</th></tr>");// print directory or file's information's name on the table's header 
	}
	else if(f.f1==0 || f.f1==8)
		fprintf(file,"<tr><th>File Name</th>");  // print file's name on talbe header 

/////////////////////////////// if there is match print /////////////////////////////
	
	for(int i=0 ; i<list->cnt; i++){
			getcwd(cur,1024); // change direcotry to original path 
			if(!fnmatch(wild,current->name,0)){ 
				if(f.f1 != 8){
					if(strncmp(current->name, ".",1)){
						dirp = opendir(current->name);
						if(dirp==NULL){
							if(f.f1 == 0){
								fprintf(file,"<tr><td><a href=%s/%s>%s\n</a></td></tr>",getcwd(tmp,1024),current->name,current->name); // hyper link with absolute path 
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
						fprintf(file,"<td><a href=%s/%s>%s\n</a></td>",getcwd(tmp,1024),current->name,current->name); // hyper link with absolute path 
					}
				}
		wcnt++;	
		}chdir(cur);
		current=current->next;
	}
	fprintf(file,"</TABLE>"); // mark end of talbe 
	fprintf(file,"</body>"); // mark end of body 
	fprintf(file,"<body>");	 // mark start of body 

///////////////////////// printing directory which is in directory /////////////////////

		current= list->head;
		for(int i=0; i<list->cnt; i++){
			if(strncmp(current->name,".",1)){
				dirp=opendir(current->name);
				if(dirp!=NULL){
					chdir(current->name);
					fprintf(file,"<td><b>directory path: %s:\n</b></td>",getcwd(tmp,1024)); // print directory's path on header 
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
	fprintf(file,"</body>"); // mark end of body 
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
                    printf( "Cannot access %s : No such file or directory\n", argv[loop]); // print error message on header erea
            }
		}
		loop++;
	}
	return 0;
}*/

int ht_ls(char *url){
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
	//t = Flag2(argc, argv);


	FILE *file = fopen("html_ls.html","w"); // write on html_ls.html file 
	fprintf(file, "<table border=1>"); // make table type 
	

	fprintf(file,"</h1>");

	if(file==0){
		printf("ERROR\n");
		return -1;
	}
	fprintf(file,"<title>%s</title>",getcwd(cur,1024)); // make title with current working directory 

//////////////////if there is only one argv then open . directroy ///////////////////

	 
	dirp=opendir(url); // open current directory 
	if(dirp!=NULL){
        Start(rlist); // start linked list 
        while((dir=readdir(dirp))!=NULL){
        	Insert(rlist,dir); // insert node 
      	} 
		Statprint(rlist, file);
    }

	


	check(argc, argv, wc);
	
	closedir(dirp);
	fclose(file);
	return 0;
	
}		







