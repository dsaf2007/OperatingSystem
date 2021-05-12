#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#define P 10
#define E 10

sem_t Fork[P];

void *philosopher(void *ptr)
{
	int i, k = *((int*)ptr);

	for(int i = 1; i <= E;i++)
	{
		printf("%*c%d: Hungry \n",k * 11, ' ',k+1);

		if((k%2))
		{
			sem_wait(&Fork[k]);
			sem_wait(&Fork[(k+1)%P]);
		}
		else
		{
			sem_wait(&Fork[(k+1)%P]);
			sem_wait(&Fork[k]);
		}

		printf("%*c%d : Eat(%d)\n", k*11,' ',k+1,i);
		sleep((rand()%3 +1));
		sem_post(&Fork[k]);
		sem_post(&Fork[(k+1)%P]);

		printf("%*c%d : Think\n",k*11,' ',k+1);
		sleep(rand()%3+1);
	}
	pthread_exit(0);
	return "";
}

int main()
{
	int i, target[P];
	pthread_t thread[P];
	srand((unsigned)time(NULL));

	for(int i = 0; i < P; i++)
	{
		sem_init(&Fork[i],0,1);
		target[i] = i;
		pthread_create(&thread[i],NULL,philosopher,(void *)&target[i]);
	}
	for(int i = 0; i < P; i++)
		pthread_join(thread[i],NULL);
	for(int i=0;i<P;i++)sem_destroy(&Fork[i]);
}
