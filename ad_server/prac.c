#include <signal.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

void signalHandler(int sig){
	if(sig == SIGINT){
		printf("signal : SIGINT\n");
		exit(0);
	}
	if(sig == SIGTSTP){
		printf("signal : SIGTSTP\n");
		exit(0);
	}
	if(sig == SIGCHLD){
		printf("the child process is dead\n");
	}
}

int main(){
	signal(SIGCHLD, signalHandler);
	printf("input CTRL + c or CTRL + z\n\n");

	int status;
	pid_t pid;
	if((pid=fork()) == 0){
		signal(SIGINT, signalHandler);
		signal(SIGTSTP, signalHandler);
		printf("child proeccessor pid : %d\n", pid);

		while(1);
	}
	else{
		signal(SIGINT, SIG_IGN);
		signal(SIGTSTP, SIG_IGN);
		printf("parent proccessor pid : %d\n",pid);
		wait(&status);
		printf("after returning the resource, the parent process also dies.\n");
	}
}
