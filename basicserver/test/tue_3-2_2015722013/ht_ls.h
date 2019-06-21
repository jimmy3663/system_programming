//////////////////////////////////////////////////////////////////////
//HTML_LS header file 												//
//==================================================================//
//header file for ht_ls.c file. 									//
//////////////////////////////////////////////////////////////////////

#ifndef HT_LS_H
#define HT_LS_H
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
//Start list 										 //
//================================================== //
//Input : list -> cnt = 0 							 //
//	      list -> head = NULL 						 //
//Purpose : to initialize a list 					 // 
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

void Start(List *list); // initialize node 
int ht_ls(char *url); // function call for ht_ls
int Insert(List *list, struct dirent *dir); //insert list 
int status(List *list, Node *current, FILE* file, char* path);// print out current file's information 
int Statprint(List *list, FILE *file, int flag, char* path); // format of printing out information

#endif

