#include<stdio.h>

/*---https://www.geeksforgeeks.org/fork-system-call/---*/ 
#include<sys/types.h>
#include<unistd.h>
/*---https://www.geeksforgeeks.org/fork-system-call/---*/
#define size 200

void forking(char argument[])
{
//	printf("argument %s \n", argument);

        int fd[2];
	pipe(fd);
        char buffer_memory[200];
        int bytes_from_buffer;

	int f=fork();
        if(f==-1)
	{
		perror("fork error");
//		exit(0);
	}
	if(f==0)
	{
		//in child process	
		//printf("In Child  Process \n");
		printf("Child PID =  %d \n", getpid());

		//to close the output end of the pipe we write:
		close(fd[1]);
	
	       // to read from a pipe and print it we write:
		bytes_from_buffer=read(fd[0], buffer_memory, sizeof(buffer_memory));
                if(strstr(argument, "echo")!=NULL)
		{
			printf("Hello World! \n");
		}
		else
		{ 
			printf("%s\n", buffer_memory);
		}
	}
	else
	{
		//in parent process
		//printf("In Parent Process \n");
		printf("Parent PID = %d \n", getpid());
		
		//to close the input side of the pipe we write:
		close(fd[0]);

		//to write in the pipe we write:
		//write(fd[1], execl(argument, "date", 0, 0), strlen(execl(argument, "date", 0, 0));
		dup2(fd[1],1); //stackoverflow.com/questions/7292642/grabbing-output-from-exec
                close(fd[1]);
 		execl(argument, "",0);
	}
}
int main(int argc, char *argv[])
{
        forking(argv[1]);
	return 0;
}
