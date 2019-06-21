/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// File Name   : ls.c                                                                                      //
// Date        : 2019 / 04 / 05                                                                            // 
// OS	       : Ubuntu 16.04 LTS 64bits                                                                   // 
// Author      : JIN SU LEE                                                                                // 
// Studien ID  : 2015722013                                                                                // 
// ------------------------------------------------------------------------------------------------------- //
// Title       : System programming Assignment #2-1 (simple ls)                                            //
// Description : This assignment is about making simpls ls. This program doesn't need to print out         //
//               hidden files. Addition to that this program should sort file name by order of alphabets.  //
//               If input file is not directroy then it shows exception sign.                              //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv){
	DIR *dirp;
	struct dirent *dir;
	char **arr;
	int cnt=0;
	int tmp=0;
	int k=0;

/////////////////// when printing current directory ///////////////////	
	if(argc == 1){
		dirp = opendir(".");
	
		while((dir=readdir(dirp)) != NULL){
			if(dir->d_name[0] == '.')
				continue;
			cnt++;	
		}
	}
///////////////////////////////////////////////////////////////////////

////////////////////// if input is more than 2 ///////////////////////
	else if (argc >2){
		printf("Only one directory path can be processed\n");
	}
//////////////////////////////////////////////////////////////////////

////////////////////// when printing path directroy /////////////////
	else{
		dirp = opendir(argv[1]);
		
		if(dirp == NULL){
			printf("'%s': No such directory\n",argv[1]);
			k++;
		}		
		else{	
		while((dir=readdir(dirp)) != NULL){
			if(dir->d_name[0]=='.')
				continue;
			cnt++;
		}
	   }
	}
/////////////////////////////////////////////////////////////////////	

/////////// when input is one or less and file is directory ////////

	if(argc<=2&&k==0){
//////////// making new array to store directory's list ///////////
	
	arr = (char **)malloc(sizeof(char*)*cnt);
	
	for(int i=0; i<cnt; i++)
	 arr[i]=(char *)malloc(sizeof(char)*256);
	
	rewinddir(dirp);

		while((dir=readdir(dirp))!=NULL){
			if(dir->d_name[0]=='.')
				continue;
			arr[tmp]=dir->d_name;
			tmp++;
		}
//////////////////////////////////////////////////////////////////	

/////////////////////////// Sorting ///////////////////////////////

	for(int i=0; i<cnt;i++){
		for(int j=i;j<cnt;j++){
			int s;
			s=strcasecmp(arr[i],arr[j]);
			if(s>0){		
				char *a;
				a=arr[i];
				arr[i]=arr[j];
				arr[j]=a;
			}
		}
		printf("%s\n",arr[i]);
	  }

//////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
	}
	closedir(dirp);
	
}
