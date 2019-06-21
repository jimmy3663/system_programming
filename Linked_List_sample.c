#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

typedef struct _NODE{
	char *f_name;
	struct _NODE *next;
}Node;

typedef struct _F_LIST{
	int count;
	Node *head;
}f_list;

void Init(f_list *flptr){
	flptr->count = 0;
	flptr->head = NULL;
}

int F_Insert(f_list *flptr, struct dirent *dir){
	
	Node *new = (Node *) malloc(sizeof(Node));
	Node *cur = NULL;
	Node *pre = NULL;
	
	new->f_name = dir->d_name;
	new->next = NULL;
	
	if (flptr->head == NULL){
		flptr->head = new;
	}
	else{
		// Insert Node
		cur = flptr->head;
		
		
		while(cur != NULL){
			if(strcasecmp(cur->f_name, new->f_name) > 0){ // new 가 current 보다 작다 
				if(cur == flptr->head){
					flptr->head = new;
				}
				else{
					pre->next = new;
				}
				new->next = cur;
				break;
			}
			else{ // new 가 current 보다 크다
				pre = cur;
				cur = cur->next;
			}
		}
		
		if(cur == NULL){
			pre->next = new;
		}
	}
	flptr->count++;
	
	return 0;
}


void F_Print(f_list *flptr){
	Node *cur = flptr->head;
	
	for(int i = 0; i < flptr->count; i++){
		if(strncmp(cur->f_name, ".", 1)){
			printf("%s\n", cur->f_name);
		}
		cur = cur->next;
	}
}



int main(int argc, char *argv[]){
	DIR *dirp;						// The pointer of the directory stream
	struct dirent *dir;				// The information of the directory
	f_list *list;					// 
	list = (f_list *) malloc(sizeof(f_list));
	
	// If there are more than one argument, print the error message and exit the program
	if (argc > 2){
		printf("%s: only one directory path can be processed\n", (argv[0]+2));
		return -1;
	}
	
	// Opens a directory stream corresponding to the directory name or the path
	if (argv[1] == NULL){
		dirp = opendir(".");
	}
	else {
		dirp = opendir(argv[1]);
	}
	
	// If the pointer of the directory stream is or not
	if (dirp != NULL){
		Init(flptr);
		
		//*** Insert and Sorting
		while((dir = readdir(dirp)) != NULL){
			F_Insert(flptr, dir);
		}
		
		F_Print(list);
	}
	else {
		// Error
	}
	
	// Free()

	// Close the pointer of the directory stream
	closedir(dirp);

	return 0;
}
