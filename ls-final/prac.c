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

/*int Flag2(int argc, char *argv[]){
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

	lflag<<=5;
	aflag<<=4;
	hflag<<=3;
	sflag<<=2;
	rflag<<=1;

	t=lflag+aflag+hflag+sflag+rflag;
	
	return t;
}

int main(int argc, char *argv[]){
	int i=0;
	int cnt=0;
	
	cnt = strlen(argv[argc-1]);
	char *tmp = (char*)malloc(sizeof(char)*cnt);

	for(int i=0 ; i<cnt-2; i++){
		tmp[i]=argv[argc-1][i+1];
	}

	printf("%s\n",tmp);

	return 0;
}*/


int main(int argc, char *argv[]){
	if(argc <3){
		printf("Error\n");
		return 0;
	}
	if(!fnmatch(argv[2],argv[1],FNM_PATHNAME)){
		printf("Match\n");
	}
	else 
		printf("not match\n");


	printf("%s\n",argv[2]);
}