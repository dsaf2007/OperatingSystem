#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#define P 5
#define E 5

sem_t Fork[P];

void *philosopher(void *ptr)
{
	int phil_num = *((int*)ptr);//철학자 번호

	for(int i = 1; i <= E;i++)//짝수 번째:Fork[phil_num]->왼쪽,홀수 번째:Fork[(phil_num+1)%P]->오른쪽
	{
		printf("%d: Hungry \n",phil_num+1);

		if((phil_num%2))
		{
			sem_wait(&Fork[phil_num]);
			sem_wait(&Fork[(phil_num+1)%P]);
		}
		else
		{
			sem_wait(&Fork[(phil_num+1)%P]);
			sem_wait(&Fork[phil_num]);
		}
		
		//hungry상태 이후 양쪽 젓가락을 잡으면 랜덤 시간동안 먹는다.
		printf("%d : Eat(%d)\n",phil_num+1,i);
		sleep((rand()%3 +1));
		sem_post(&Fork[phil_num]);//왼쪽포크 내려놓기
		sem_post(&Fork[(phil_num+1)%P]);//오른쪽 포크 내려놓기
		
		//식사 이후에 랜덤 시간동안 think
		printf("%d : Think\n",phil_num+1);
		sleep(rand()%3+1);
	}
	pthread_exit(0);//식사 횟수를 채우면 스레드 종료
	return "";
}

int main()
{
	int target[P];
	pthread_t thread[P];
	srand(time(NULL));

	for(int i = 0; i < P; i++)//세마포어 초기화, 각 철학자 생성
	{
		sem_init(&Fork[i],0,1);
		target[i] = i;
		pthread_create(&thread[i],NULL,philosopher,(void *)&target[i]);
	}
	for(int i = 0; i < P; i++)
		pthread_join(thread[i],NULL);
	for(int i=0;i<P;i++)sem_destroy(&Fork[i]);
}
