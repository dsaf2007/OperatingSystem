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

string* setArgs(string str);
void dispHistory();
void addHistory(string hist);

int fd[2];
int main()
{
	string text1="exit\n", text2="history\n";
	string front =NULL,back =NULL;
	history_index=0;
	string* argv;
	string* argv2;
	while(1)
	{
	//	char input[MAX_LINE/2+1];
		//string input;
		//input = (char*)malloc(sizeof(char)*(MAX_LINE/2 +1));
	//	char input[MAX_LINE/2+1];
		string input;
	//	fflush(stdout);
	//	scanf("%s",input);
	//	getchar();
		read(0,input,MAX_LINE/2+1);
	//	fgets(input,MAX_LINE/2+1,stdin);
	//	printf("%s\n",input);
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
			printf("pipe\n");
			front = strtok(input,"|");
			back = strtok(NULL,"|");
			strcat(front,"\0");
			strcat(back,"\0");
			argv = setArgs(front);
			argv2 = setArgs(back);
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
					argv = setArgs(input);
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

string* setArgs(string str)
{
	int length=strlen(str);
	int space=0,index=0;
	string temp_str=str;
	for(int i=0;i<length;i++)
	{
	//	printf("%c\n",str[i]);
		if(str[i]==' ')space++;
	}
	string* temp=(string*)calloc(space+1,sizeof(string));
	string pch;
	pch=strtok(temp_str,"\n ");
//	printf("%s\n",pch);
	index = 0;
	while(pch !=NULL)
	{
		temp[index]=pch;
		pch = strtok(NULL,"\n ");
		index++;
	//	temp[index]=strtok(NULL," ");
	}
//	if(index ==1)
	//	temp[index]=NULL;
	return temp;
	
}
