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

int fd[2];
int main()
{
	string text1="exit\n", text2="history\n";
	string front =NULL,back =NULL;
	history_index=0;
	string argv[4];
	string argv2[5];
	while(1)
	{
	//	char input[MAX_LINE/2+1];
		//string input;
		//input = (char*)malloc(sizeof(char)*(MAX_LINE/2 +1));
		char input[MAX_LINE/2+1];
		//string input;
		fflush(stdin);
	//	scanf("%s",input);
	//	getchar();
	//	read(0,input,MAX_LINE/2+1);
		fgets(input,MAX_LINE/2+1,stdin);
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
			front = strtok(input,"|");
			back = strtok(NULL,"|");
			strcat(front,"\0");
			strcat(back,"\0");
			setArgs(front,argv);
			setArgs(back,argv2);
			if(pipe(fd)==-1)//파이프 생성
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
					if(close(1)==-1)perror("1");
					if(dup(fd[1]) != 0);//출력파이프 연결
					if(close(fd[0])==-1 || close(fd[1]) ==-1)
					{
						perror("2");
					}
					execvp(argv[0], argv);
					exit(0);
			}
			switch(fork())//back
			{
				case -1:
					perror("fork error");
					break;
				case 0:
					if(close(1)==-1)perror("3");
					if(dup(fd[0]) != 0);//출력파이프 연결
					if(close(fd[0])==-1 || close(fd[1]) ==-1)
					{
						perror("4");
					}
					execvp(argv2[0], argv2);
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
			printf("%d %s",i,history[i]);
		}
	}
	else
	{
		for(int i = history_index;i>history_index-10;i--)
		{
			printf("%d %s",i,history[i]);
		}
	}
}

void setArgs(string str,char** argv)
{
	int i = 0;
    string pch = strtok(str," ");
    while(pch!=NULL && i<3)
    {
        argv[i]=pch;
        pch=strtok(NULL," ");
        i++;
    }
   argv[i]=(char*)0;
    printf("%d\n",i);
}
