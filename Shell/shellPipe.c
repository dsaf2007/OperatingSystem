#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>

#define MAX_HIST 40
#define MAX_LINE 80

typedef char* string;
string history[MAX_HIST];
int history_index;

void setArgs(string str,char** argv);
void dispHistory();
void addHistory(string hist);


int main()
{
	string text1="exit\n", text2="history\n";
	string front =NULL,back =NULL;
	history_index=0;
	string argv[4];
	string argv2[4];
    int fd[2];
	while(1)
	{
	//	char input[MAX_LINE/2+1];
		//string input;
		//input = (char*)malloc(sizeof(char)*(MAX_LINE/2 +1));
		char input[MAX_LINE/2+1];
		//string input;
	//	scanf("%s",input);
	//	getchar();
	//	read(0,input,MAX_LINE/2+1);
        printf("osh>");
		fgets(input,MAX_LINE/2+1,stdin);
        fflush(stdin);
	//	printf("%s\n",input);
		input[strlen(input)-1] ='\0';
        
		if(strcmp(text1,input)==0)
		{
			printf("exit shell");
			break;
		}
		if(input[0]=='!')
		{
			if(history_index == 0)
			{
				printf("No Command in history\n");
				continue;
			}
			else if(input[1] == '!')
			{
				strcpy(input,history[history_index]);
			}
			else
			{
				int num = (int)(input[1]-'0');
				if(num>history_index)
				{
					printf("number out of index\n");continue;
				}
				else if(num<=history_index)
				{
					strcpy(input,history[num]);
				}
				else
				{
					printf("No such command in history\n");
				}
			}	
		}
		if(strstr(input,"history")==NULL)addHistory(input);

		if(strchr(input,'|')!=NULL)
		{
            //printf("usepipe\n");
			front = strtok(input,"|");
			back = strtok(NULL,"|");
            //printf("%s",back);
			strcat(front,"\0");
			strcat(back,"\0");
			setArgs(front,argv);
			setArgs(back,argv2);
           // printf("%s %s",argv2[0],argv2[1]);
			if(pipe(fd) == -1)//파이프 생성
			{
				printf("fail to call pipe()\n");
				exit(1);
			}
			switch(fork())//front
			{
				case -1:
					perror("fork error");
					break;
				case 0:
					close(STDOUT_FILENO);
                    dup2(fd[1],1);
                    close(fd[0]);
                    close(fd[1]);
					execvp(argv[0], argv);
                    printf("no command");
					exit(0);
			}
			switch(fork())//back
			{
				case -1:
					perror("fork error");
					break;
				case 0:
					close(STDIN_FILENO);
                    dup2(fd[0],0);
                    close(fd[1]);
                    close(fd[0]);
					execvp(argv2[0], argv2);
                    printf("no command");
					exit(0);
			}
			if(close(fd[0])==-1 || close(fd[1])==-1)
				perror("5");
			while(wait(NULL) != -1);
		}
		else
		{
			switch(fork())
			{
				case -1:
					perror("fork error");
					break;
				case 0:
					setArgs(input,argv);
					if(strcmp(argv[0],"history")==0)
					{
						dispHistory();
						exit(0);
					}
					else
					{
						if(strcmp(argv[0],"cd")==0)
						{
							printf("change dir");
							chdir(argv[1]);
						}
						else
						{
						execvp(argv[0],argv);
						exit(0);
						}
					}
					break;
				default:
					wait(NULL);
			}

		}
	}
	return 0;
}


void addHistory(string hist)
{
	history_index++;
	history[history_index]=(string)malloc(sizeof(char)*strlen(hist));
	strcpy(history[history_index],hist);
//	printf("%d\n",history_index);
}

void dispHistory()
{
	if(history_index<11)
	{
		for(int i = history_index;i>0;i--)
		{
			printf("%d %s\n",i,history[i]);
		}
	}
	else
	{
		for(int i = history_index;i>history_index-10;i--)
		{
			printf("%d %s\n",i,history[i]);
		}
	}
}

void setArgs(string str,char** argv)
{
int length=strlen(str);
	int space=0,index=0;
	for(int i=0;i<length;i++)
	{
		if(str[i]==' ')space++;
	}
	string pch;
	pch=strtok(str,"\n ");
	index = 0;
	while(pch !=NULL)
	{
		argv[index]=pch;
		pch = strtok(NULL,"\n ");
		index++;
	}
		argv[index]=(char*)0;
        return 0;
}
