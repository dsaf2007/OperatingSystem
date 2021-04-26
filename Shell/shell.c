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
	string text1="exit", text2="history";
	string front =NULL,back =NULL;//명령어(앞),(뒤)
	history_index=0;//히스토리
	string argv[4];//명령어(앞)
	string argv2[4];//명령어(뒤)
    int fd[2];//파이프
    int fdr;//리다이렉션
	while(1)
	{
		char input[MAX_LINE/2+1];
		int* ampersand =-1;
        printf("osh>");
		fgets(input,MAX_LINE/2+1,stdin);
        fflush(stdin);

		input[strlen(input)-1] ='\0';

		// if(strchr(input,'&')!=NULL)
		// {
		// 	for(int i =0;i<strlen(input);i++)
		// 	{
		// 		if(input[i]=='&')
		// 			input[i]=NULL;
		// 	}	
		// 	ampersand =0;
		// }
        
		if(strcmp(text1,input)==0)
		{
			printf("exit shell");
			break;
		}
		if(input[0]=='!')//히스토리 관련 기능
		{
			if(history_index == 0)
			{
				printf("No Command in history\n");
				continue;
			}
			else if(input[1] == '!')//이전 명령어
			{
				strcpy(input,history[history_index]);
			}
			else
			{
				int num = (int)(input[1]-'0');
				if(num>history_index)//지정 숫자 명령어
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
		if(strstr(input,"history")==NULL)addHistory(input);//히스토리에 명령어 추가

		if(strchr(input,'|')!=NULL)//파이프 사용
		{
			front = strtok(input,"|");
			back = strtok(NULL,"|");
			strcat(front,"\0");
			strcat(back,"\0");
			setArgs(front,argv);
			setArgs(back,argv2);
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
                    dup2(fd[1],1);//출력 파이프 연결
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
                    dup2(fd[0],0);//입력 파이프 연결
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
        else if(strchr(input,'>')!=NULL)//리다이렉션 >
        {
            front = strtok(input,">");
            back = strtok(NULL,">");
            strcat(front,"\0");
			strcat(back,"\0");
            setArgs(front,argv);
			setArgs(back,argv2);
            switch(fork())
            {
                case -1:
                    perror("fork fail");
                    break;
                case 0:
                    fdr=open(argv2[0], O_WRONLY | O_CREAT | O_TRUNC,0644);//쓰기 전용
                    if(fdr==-1)
                    {
                        perror("failed to create file");exit(1);
                    }
                    if(dup2(fdr,1)==-1)
                    {
                        perror("fdr dup2 error");
                    }
                    close(fdr);
                    execvp(argv[0],argv);
                    exit(0);
                    break;
                default:
                    wait(NULL);
            }
        }
        else if(strchr(input,'<')!=NULL)//리다이렉션 <
        {
            front = strtok(input,"<");
            back = strtok(NULL,"<");
            strcat(front,"\0");
			strcat(back,"\0");
            setArgs(front,argv);
			setArgs(back,argv2);
            switch(fork())
            {
                case -1:
                    perror("fork fail");
                    break;
                case 0:
                    fdr=open(argv2[0], O_RDONLY);//읽기 전용
                    if(fdr==-1)
                    {
                        perror("failed to create file");exit(1);
                    }
                    if(dup2(fdr,0)==-1)
                    {
                        perror("fdr dup2 error");
                    }
                    close(fdr);
                    execvp(argv[0],argv);
                    exit(0);
                    break;
                default:
                    wait(NULL);
            }
        }
		else//일반 명령어
		{
			 setArgs(input,argv);
            if(strcmp(argv[0],"cd")==0)//디렉토리 변경
			{
				chdir(argv[1]);
			}
            else if(strcmp(argv[0],"history")==0)
			{
				dispHistory();
			}
            else
			{
            switch(fork())
			{
				case -1:
					perror("fork error");
					break;
				case 0:
						execvp(argv[0],argv);
						exit(0);
					break;
				default:
				//if(ampersand ==0)
					wait(NULL);
			}
            }
		}
	}
	return 0;
}
void addHistory(string hist)//히스토리 추가
{
	history_index++;
	history[history_index]=(string)malloc(sizeof(char)*strlen(hist));
	strcpy(history[history_index],hist);
}
void dispHistory()//히스토리 출력
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
void setArgs(string str,char** argv)//입력 문자열 파싱
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
        return;
}
