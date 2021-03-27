#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

//옮길파일 ,옮겨질 위치 
int main(int argc, char* argv [])
{
	int rfd,wfd,n;
	char buf[10000];
	mode_t mode;
	
	mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

	rfd = open(argv[1], O_RDONLY);
	if(rfd == -1)
	{
		perror(argv[1]);
		exit(1);
	}

	wfd = open(argv[2] , O_CREAT | O_WRONLY | O_TRUNC ,mode);
	if(wfd == -1)
	{
		perror(argv[2]);
		exit(1);
	}

	while((n = read(rfd,buf,6)) >0)
		if(write(wfd,buf,n) !=n)perror("Write");

	if(n == -1) perror("Read");

	close(rfd);
	close(wfd);

	return 0;
}
