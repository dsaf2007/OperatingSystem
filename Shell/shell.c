#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>

#define MAX_HIST 40
#define MAX_LINE 80

typedef char* string;
string* history[MAX_HIST];
int history_index;

string* setArgs(string str);
void dispHistory();
void addHistory(string hist);
int main()
{
	string text1="exit\n", text2=NULL;
	history_index=0;
	
	while(1)
	{
	//	char input[MAX_LINE/2+1];
		string input;
		input = (char*)malloc(sizeof(char)*(MAX_LINE/2 +1));
		fflush(stdout);
		read(0,input,MAX_LINE/2+1);

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

		switch(fork())
		{
			case -1:
				perror("fork error");
				break;
			case 0:
				input[strlen(input)]='\0';
				printf("%s",input);
				addHistory(input);
				if(strcmp(input,"history\0")==0)
				{
					printf("s");
					dispHistory();
					exit(0);
				}
				else
				{

					text1=strtok(input," ");
					text2=strtok(NULL,"\0");
					char* args={"ls","-al"};
					execvp(args[0],args);
					exit(0);
				}
				wait(NULL);


		}
	}
	return 0;
}


void addHistory(string hist)
{
	history_index++;
	history[history_index]=(char**)malloc(sizeof(char)*strlen(hist));
	strcpy(history[history_index],hist);
}

void dispHistory()
{
	if(history_index<11)
	{
		for(int i = history_index;i>0;i++)
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

string setArgs(string str)
{
	int lenght=strlen(str);
	int space=0,index=0;
}