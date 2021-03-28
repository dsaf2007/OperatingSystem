#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

//옮길파일 ,옮겨질 위치 
int main(int argc, char* argv [])
{
	int rfd,wfd,n;//readfile_dir, writefile_dir,data size
	char buf[10000];//buffer
	mode_t mode;
	
	mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;//flag

	rfd = open(argv[1], O_RDONLY);//readfile open
	if(rfd == -1)//error message if error in readfile
	{
		perror(argv[1]);
		exit(1);
	}

	wfd = open(argv[2] , O_CREAT | O_WRONLY | O_TRUNC ,mode);
	if(wfd == -1)//error message if error in writefile
	{
		perror(argv[2]);
		exit(1);
	}

	while((n = read(rfd,buf,6)) >0)//write read buffer to writefile
		if(write(wfd,buf,n) !=n)perror("Write");

	if(n == -1) perror("Read");

	close(rfd);
	close(wfd);

	return 0;
}
