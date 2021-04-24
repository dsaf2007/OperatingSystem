#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>

#define MAX_HIST 40


typedef char* string;
string* history[MAX_HIST];
int history_index;

void dispHistory();
void addHistory(string hist);
int main()
{
	string text1=NULL, text2=NULL;
	history_index=-1;
}


void addHistory(string hist)
{
	history_index++;
	history[history_index]=&hist;
	//trcpy(history[history_index],hist);
}
