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

void Start(List *list);
int Insert(List *list, struct dirent *dir, int h);
int StatInsert(List *list, struct dirent *dir, int h);
int RevInsert(List *list, struct dirent *dir, int h);
int RevStatInsert(List *list, struct dirent *dir, int h);
int status(List *list, Node *current, FILE* file);
int Statprint(List *list, FILE *file);
int ht_ls(char *url);

#endif
