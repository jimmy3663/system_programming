#include "ht_ls.h"

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
int Insert(List *list, struct dirent *dir){
	Node *new = (Node *)malloc(sizeof(Node)); // making new node 
	Node *current = NULL; // current node is null 
	Node *prev = NULL; // pointer for previous node is null 
	struct stat buf; 
	//struct tm *mktm=NULL;
		
	new->name = dir->d_name; // new node's name is directory's name 
	lstat(new->name, &buf);
	new->size = (int)buf.st_size;
	//new->stime = localtime(&buf.st_mtime);
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

///////////////////////////////////////////////////////////////////////
//Printing a file's status 											 //
//===================================================================//
//Input : current node in list, option flag 						 //
//Output : file status 												 //
//Purpose : to print a current node's full status 					 //
///////////////////////////////////////////////////////////////////////	
int status(List *list, Node *current, FILE* file, char* path){
        DIR *dirp;
        struct dirent *dir;
        unsigned int tmp;
        struct stat buf;
        struct passwd *pwd = NULL;
        struct group *gr = NULL;
        struct tm *mktm= NULL;
        char mon[15];
        long int pw = pow(1024,4);
        char cur[1024];
        char kk[1024];
	char href[4096] = {0};
	//printf("status path: %s\n",path);
	getcwd(cur,1024);
	//printf("status()-default path:%s\n", cur);
	if(lstat(current->name, &buf)<0)
		//printf("lstat error\n");
      	//printf("name: %s\n",current->name); 
        if(S_ISDIR(buf.st_mode)){
		if(strcmp(current->name, ".") == 0){
        		fprintf(file,"<tr style=color:blue><td><a href=%s>%s\n</a></td>",path,current->name); //if directory color it with blue 
		}
		else if(strcmp(current->name, "..")==0){
			char furl[255]={0, };
			for(int i=0; i<strlen(path)-1; i++){
                        	furl[i]=path[i];
			}
		//	printf("furl: %s\n",furl);
		//	printf("path: %s\n",path);
			sprintf(href, "%s/%s",furl, current->name);
        		fprintf(file,"<tr style=color:blue><td><a href=%s>%s\n</a></td>",href,current->name); //if directory color it with blue 
			
                }
		else{
			sprintf(href, "%s/%s", path, current->name);
		//	printf("href : %s\n", href);
        		fprintf(file,"<tr style=color:blue><td><a href=%s>%s\n</a></td>",href,current->name); //if directory color it with blue 
		}
		//printf("directory in\n");
        }
        else if(S_ISLNK(buf.st_mode)){
		sprintf(href, "%s/%s", path, current->name);
        	fprintf(file,"<tr style=color:green><td><a href=%s>%s\n</a></td>",href,current->name); // if link, color it with green
        }
        else{
		sprintf(href, "%s/%s", path, current->name);
	//	printf("href : %s \n", href);
        	fprintf(file,"<tr style=color:red><td><a href=%s>%s\n</a></td>",href,current->name);// rest, color it with red 
        }
		if(S_ISREG(buf.st_mode)){ // if file's mode is regular file 
			printf("in lstat\n");
			fprintf(file,"<td>-");
		}
		else if(S_ISDIR(buf.st_mode)){//if file's mode is directory
			 fprintf(file,"<td>d");
			printf("d\n");
		}
		else if(S_ISCHR(buf.st_mode))//if file's mode is character device 
			 fprintf(file,"<td>c");
		else if(S_ISBLK(buf.st_mode))//if file's mode is block device 
			 fprintf(file,"<td>b");
		else if(S_ISLNK(buf.st_mode))//if file's mode is symbolic link 
			  fprintf(file,"<td>l");
		else if(S_ISSOCK(buf.st_mode))//if file's mode is socekt 
			  fprintf(file,"<td>s");
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
		//printf("status() Test\n");
		mktm = localtime(&buf.st_mtime);
		int m = mktm->tm_mon;
		switch(m){
			case 0 : strcpy(mon ,"Jan"); break;
			case 1 : strcpy(mon, "Feb"); break;
			case 2 : strcpy(mon, "Mar"); break;
			case 3 : strcpy(mon, "Apr"); break;
			case 4 : strcpy(mon, "May"); break;
			case 5 : strcpy(mon, "Jun"); break;
			case 6 : strcpy(mon, "Jul"); break;
			case 7 : strcpy(mon, "Aug"); break;
			case 8 : strcpy(mon, "Sep"); break;
			case 9 : strcpy(mon, "Oct"); break;
			case 10 : strcpy(mon, "Nov"); break;
			case 11 : strcpy(mon, "Dec"); break;
		}
		pwd=getpwuid(buf.st_uid); //user name 
		gr=getgrgid(buf.st_gid); // group nae 
		char *tim=ctime(&buf.st_mtime);
		fprintf(file,"<td>%d</td>",(int)buf.st_nlink);
		fprintf(file,"<td>%s</tb>",pwd->pw_name);
		fprintf(file,"<td>%s</tb>",gr->gr_name);
		fprintf(file,"<td>%d</td>",(int)current->size);
		fprintf(file,"<td>%s</td>",tim);
		fprintf(file,"</tr>"); // end of the table row 
		printf("2\n");
		return 0;
}

///////////////////////////////////////////////////////////////////////////////////
//print file's status 															 //
//===============================================================================//
//Input : list,  flag 															 //
//Output : if option flag is 0 -> print root directory with option l 			 // 
// 		   if option flag is 1 -> print child directory with option al  		 //
//Purpose :  to print root or to print child directory 					 		 //
///////////////////////////////////////////////////////////////////////////////////
int Statprint(List *list, FILE *file, int flag,char *path){
	Node *current = list->head;
	unsigned int tmp;
	struct stat buf;
	struct passwd *pwd = NULL;
	struct group *gr = NULL;
	struct tm *mktm= NULL;
	char mon[15];
	int blsize=0;

	printf("statprint in\n");
	if(flag==0){
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
		fprintf(file,"<h><b>total : %d\n</b></h><br>",(int)blsize);	//print tottal in head with bold words
		fprintf(file,"<body>"); // start of body 
		fprintf(file,"<TABLE>"); // mark start of table 
		fprintf(file,"<table border=1>"); // table's type 
		fprintf(file, "<tr><th>File Name</th><th>Permission</th><th>Link</th><th>Owner</th><th>Group</th><th>Size</th><th>Last Modified</th></tr>"); // table's head 
		
		current=list->head;
	
		for(int i=0; i<list->cnt;  i++){ 
			if(strncmp(current->name,".",1)&&strcmp(current->name,"html_ls.html")){ // print except for hidden file and 
				printf("status:%s\n", current->name);
				status(list, current, file,path);
			}
			current= current->next;
		} // printing file status 
		fprintf(file,"</TABLE>"); // mark end of table 
		fprintf(file,"</body>"); // mark end of body 
	}
	else if(flag==1){
		for(int i=0; i<list->cnt; i++){
			if(lstat(current->name, &buf)==0){
				printf("lstat:%s\n", current->name);
				blsize=blsize+buf.st_blocks;
			} // caclulating all files's block size including hidden file 
			current=current->next;
		}
		current = list->head;
		fprintf(file,"<h><b>total : %d\n</b></h><br>", blsize/2); // print total size of file in head area 
		fprintf(file,"<body>"); // mark start of the body 
		fprintf(file,"<TABLE>"); // start of the table 
		fprintf(file,"<table border=1>"); // table's type 
		fprintf(file, "<tr><th>File Name</th><th>Permission</th><th>Link</th><th>Owner</th><th>Group</th><th>Size</th><th>Last Modified</th></tr>"); // printf table headers
		
		for(int i=0; i<list->cnt; i++){
			if(strcmp(current->name,"html_ls.html")!=0){ // print except for html file. 
				status(list, current, file, path);
			}
			current=current->next;
		}
		fprintf(file,"</TABLE>"); // end of thetalbe: 
		fprintf(file,"</body>");// end of the body 
			
	}
	return 0;
}

//////////////////////////////////////////////////////
//free linked list 									//
//==================================================//
//Input : first node 								//
//Purpose : to free linked list in case of memory 	//
//			leakage									//
//////////////////////////////////////////////////////
void free_list(Node *first){
	Node *temp, *current;
	for ( temp=first ; temp != NULL ; temp=current ){
		printf("free: %s\n",temp->name);
   		current = temp->next;
   		free (temp);
	}
}

//////////////////////////////////////////////////////////////////////
//ht_ls function 													//
//==================================================================//
//Input : url -> url from main.c 									//
//Output : if url is / then print out root directory 				//
//			if not then print our child directory or file 			//
//			if file then write on html_ls.html file 				//
//Purpose : ht_ls function determines root or other. And function 	//
//			call for other printing functions. 						//
//////////////////////////////////////////////////////////////////////
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
	char path[1024] = {0};
	char furl[255] = {0};
	getcwd(cur,1024); // get current directory's path 
	int flag=0;
	char *buffer =NULL;

	FILE *file = fopen("html_ls.html","w"); // write on html_ls.html file 

	if(file==0){	// if file open error 
        printf("ERROR\n");
        return -1;
    }
	
	if(strcmp(url, "/") != 0){ // if url is not / then child directory or file 
		for(int i=1; i<strlen(url); i++){ // cutting url to get rid of / 
			furl[i-1]=url[i];
		}

		if(chdir(furl)<0){
			dirp=opendir(furl); // open url 
			
			if(errno==ENOTDIR){ // if not directory but file 
				FILE *nfile = fopen(furl,"r"); // open url file 
				fseek(nfile,0, SEEK_END);
				int fs=ftell(nfile); // get file size 
				fseek(nfile,0,SEEK_SET);
				buffer = (char*)malloc(sizeof(char)*(fs+1)); // dynamic allocation for file buffer 
				memset(buffer,0,fs+1);
				if(fread(buffer,1, fs, nfile)<0){ // read file content on buffer 
					printf("in html.c file read error\n");
					return -1;
				}

				fwrite(buffer, 1,fs,file); // write buffer on html_ls.html file 
				fclose(nfile);
				closedir(dirp);
				fclose(file);
				return 0;
			}

			dirp=opendir(furl); // if not file and directory 
			if(errno!=ENOTDIR){ // not existing file or directory print out 404 not found 
				fprintf(file, "<h1>NOT FOUND </h1><br><h>The request URL /%s was not found on this server<br>HTTP 404 - Not Page Found</h>",furl);
				closedir(dirp);
				fclose(file);
				printf("no path\n");
				return -2;
			}
			return -1;
		}
		fprintf(file, "<table border=1>"); // make table type 
        	fprintf(file,"</h1>");
        	printf("pth: %s\n", getcwd(pth,1024));
		flag=1;
		printf("in ht_ls url : %s \n", url);
		dirp=opendir(pth);
		fprintf(file,"<h1>System Programming HTTP</h1><br><br>");
		fprintf(file,"<title>%s</title>",pth); // make title with current working directory 
		fprintf(file,"<h><b>directory path : %s</b></h><br>",pth);
		if(dirp!=NULL){ // if directory 
			Start(rlist); // start linked list 
			while((dir=readdir(dirp))!=NULL){ 
				Insert(rlist,dir); 
			}
			Statprint(rlist,file,flag,url); 
		}
		else if(dirp==NULL){ // if not directory print error 
			printf("open failed\n"); 
			return -1;
		}
		chdir(cur); // change directory to root directory
	}
	else{
		fprintf(file, "<table border=1>"); // make table type 
        	fprintf(file,"</h1>");
		printf("current path:%s\n", cur);
		dirp=opendir("."); // open current directory 
		fprintf(file,"<h1>Welcome to System Programming HTTP</h1><br><br>");
		fprintf(file,"<title>%s</title>",getcwd(pth,1024)); // make title with current working directory 
		fprintf(file,"<h><b>directory path : %s</b></h><br>",getcwd(pth,124));
		if(dirp!=NULL){
        		Start(rlist); // start linked list 
       			while((dir=readdir(dirp))!=NULL){
        			Insert(rlist,dir); // insert node 
      			} 
			Statprint(rlist, file,flag,path);
   	 	}
	}
	
	Node *first = rlist->head; // to free from the start set first to head of list 
	free_list(first); // free linked list 
	closedir(dirp);
	fclose(file);
	return 0;
	
}		

