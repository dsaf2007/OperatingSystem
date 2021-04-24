
 #include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

//pipe
#include <sys/types.h>

#include <fcntl.h>
//signal
#include <signal.h>

int main(){
 char str[1024];//입력받은 문자열
 char *text1=NULL;
 char *text2=NULL;
 char *command1[4];//앞쪽명령어
 char *command2[5];//뒤쪽명령어
 int i ;
 char *pch;//문장나눌때 임시 저장
 //pipe
 int fd[2]; // 파이프 처리
 int fdr;// '>', '<'
 pid_t pid;

 //signal

 sigset_t blockset;// 막아놓을 시그널 목록
 sigemptyset(&blockset);
 sigaddset(&blockset, SIGINT);//목록에 해당 ^C시그널 추가
 sigaddset(&blockset, SIGQUIT);//^\시그널 추가
 sigprocmask(SIG_BLOCK, &blockset, NULL);
 
 while(1){
  
  for(i=0 ; i < 1024 ; i++){
   str[i]='\0';
  }
  command1[0]=NULL;command1[1]=NULL;command1[2]=NULL;command1[3]=NULL;
  command2[0]=NULL;command2[1]=NULL;command2[2]=NULL;

  printf("$");

  fgets(str,sizeof(str),stdin);
  if (feof(stdin)){
   printf("Ctrl+D exit \n");
   exit(0);
                }
  str[strlen(str)-1] ='\0';
  fflush( stdin );

  if(strchr(str,'|')!=NULL){//파이프 사용하는 경우
   text1 = strtok (str,"|");
   text2 = strtok (NULL, "|");
   //printf("cmd1 :%s\n cmd2 :%s\n",text1,text2);
   strcat(text1,"\0");
   strcat(text2, "\0");
   i=0;//초기화
   pch = strtok (text1," ");
   while (pch != NULL && i<3)
   {
     command1[i]=pch;
       pch = strtok (NULL, " ");
    //printf ("command1[%d]:%s\n",i,command1[i]);
    i++;
   }
   command1[i]=(char*)0;
   i=0;//초기화
   pch = strtok (text2," ");
   while (pch != NULL && i<3)
   {
     command2[i]=pch;
       pch = strtok (NULL, " ");
    //printf ("command2[%d]:%s\n",i,command2[i]);
    i++;
   }
   command2[i]=(char*)0;

   if(pipe(fd) == -1){//파이프 생성   fd[0] 읽기용, fd[1] 쓰기용
    printf("fail to call pipe()\n");
    exit(1);
   }
   switch(fork())//앞쪽 명령어프로세스 생성
   {
    case -1 : perror("fork error"); break;
    case 0 :
     if(close(1)==-1) perror("close1");
     if(dup(fd[1]) != 0);//표준출력 파이프 연결
     if(close(fd[0]) == -1 || close(fd[1]) == -1){
       perror("close2");
     }
     execvp(command1[0], command1);
     printf("command not found \n");     
     exit(0);  
   }
   switch(fork())//뒤쪽명령어 프로세스 생성
   {
    case -1 : perror ("fork"); break;
    case 0 :
     if(close(0) == -1) perror("close3");
     if(dup(fd[0]) != 0);//표준입력 파이프 연결
     if(close(fd[0]) == -1 || close(fd[1]) == -1) perror("close4");
     execvp(command2[0], command2);
     printf("command not found \n");     
     exit(0);  
   }
 
   if(close(fd[0]) == -1 || close(fd[1]) == -1) perror("close5");
   while(wait(NULL) != -1);
  }else if(strchr(str,'>')!=NULL){// '>'사용된 경우
   text1 = strtok (str,">");
   text2 = strtok (NULL, ">");
   i=0;//초기화
   pch = strtok (text1," ");
   while (pch != NULL && i<3)
   {
     command1[i]=pch;
       pch = strtok (NULL, " ");
    //printf ("command1[%d]:%s\n",i,command1[i]);
    i++;
   }
   command1[i]=(char*)0;
   i=0;//초기화
   pch = strtok (text2," ");
   while (pch != NULL && i<3)
   {
     command2[i]=pch;
       pch = strtok (NULL, " ");
    //printf ("command2[%d]:%s\n",i,command2[i]);
    i++;
   }
   command2[i]=(char*)0;
   
   switch(fork())//앞쪽명령어 프로세스 생성, 뒤쪽 파일 생성
   {
    case -1 : perror ("fork"); break;
    case 0 :
     fdr = open(command2[0], O_WRONLY | O_CREAT | O_TRUNC, 0644);
     if(fdr==-1) {
      perror("파일 새로생성 오류");exit(1);
     }
     if( dup2(fdr, 1) == -1){
      perror("fdr dup error");
     }
     close(fdr);
     execvp(command1[0], command1);
     printf("command not found \n");     
     exit(0);        
      break;
    default : wait(NULL);
   }
  }else if(strchr(str,'<')!=NULL){// '<' 사용된 경우
   text1 = strtok (str,"<");
   text2 = strtok (NULL, "<");
   i=0;//초기화
   pch = strtok (text1," ");
   while (pch != NULL && i<3)
   {
     command1[i]=pch;
       pch = strtok (NULL, " ");
    //printf ("command1[%d]:%s\n",i,command1[i]);
    i++;
   }
   command1[i]=(char*)0;
   i=0;//초기화
   pch = strtok (text2," ");
   while (pch != NULL && i<3)
   {
     command2[i]=pch;
       pch = strtok (NULL, " ");
    //printf ("command2[%d]:%s\n",i,command2[i]);
    i++;
   }
   command2[i]=(char*)0;
   switch(fork())//앞쪽 파일 생성,뒤쪽 프로세스 생성
   {
    case -1 : perror ("fork"); break;
    case 0 :
     fdr = open(command1[0], O_WRONLY | O_CREAT | O_TRUNC, 0644);
     if(fdr==-1) {
      perror("파일 새로생성 오류");exit(1);
     }
     if( dup2(fdr, 1) == -1){
      perror("fdr dup error");
     }
     close(fdr);
     execvp(command2[0], command2);
     printf("command not found \n");     
     exit(0);        
      break;
    default : wait(NULL);
   }
  }else{       //단일 명령문
   str[strlen(str)]='\0';
   //printf("onlycmd1:%s \n",str);
   i=0;//초기화
   pch = strtok (str," ");
   while (pch != NULL && i<3)
   {
     command1[i]=pch;
       pch = strtok (NULL, " ");
    //printf ("command1[%d]:%s\n",i,command1[i]);
    i++;
   }
   command1[i]=(char*)0;
   //printf ("command1[0]:%s\n",command1[0]);
   if(command1[0]!=NULL){   
    if(strcmp(command1[0],"logout")==0){//명령어 logout
     exit(0);
    }else if(strcmp(command1[0],"cd")==0){//내장명령어 cd
     chdir(command1[1]);//현재 디렉토리 변경
    }else if(fork()==0){
     execvp(command1[0], command1);
     printf("command not found \n");     
     exit(0);  
    }
    wait(NULL);
   }
   
  }
  
 }
 return 0;
}