#include <pthread.h>		
#include <semaphore.h>		
#include <stdio.h>	
#include<math.h>		

pthread_t *Students;		
pthread_t TA;		

int ChairsCount = 0;
int CurrentIndex = 0;

sem_t TA_Sleep;
sem_t Student_Sem;
sem_t ChairsSem[3];
pthread_mutex_t ChairAccess;

void *Student_Activity(void *threadID) 
{

	printf("Student %ld is doing programming assignment.\n", (long)threadID);
	sleep(3);		

	printf("Student %ld needs help from the TA\n", (long)threadID);

	pthread_mutex_lock(&ChairAccess);
	int count = ChairsCount;
	pthread_mutex_unlock(&ChairAccess);
	if(count < 3)		
	{
		if(count == 0)		
		sem_post(&TA_Sleep);

		else
			printf("Student %ld sat on a chair waiting for the TA to finish. \n", (long)threadID);
		
		
		pthread_mutex_lock(&ChairAccess);
		int index = (CurrentIndex + ChairsCount) % 3;
		ChairsCount++;
		printf("Student sat on chair.Chairs Remaining: %d\n", 3 - ChairsCount);
		pthread_mutex_unlock(&ChairAccess);

		
		sem_wait(&ChairsSem[index]);		
		printf("\t Student %ld is getting help from the TA. \n", (long)threadID);
		sem_wait(&Student_Sem);		
		printf("Student %ld left TA room.\n",(long)threadID);
	}
	
	else 
		printf("Student %ld will return at another time. \n", (long)threadID);
		
}

// ------------------------------------------------------------------------------------------

void *TA_Activity()
{
	sem_wait(&TA_Sleep);		
	printf("\n\tTA has been awakened by a student.\t\n");

	while(1)
	{
		
		pthread_mutex_lock(&ChairAccess);
		
		if(ChairsCount == 0) 
		{
			
			pthread_mutex_unlock(&ChairAccess);
			break;
		}
		
		sem_post(&ChairsSem[CurrentIndex]);
		ChairsCount--;
		printf("Student left his/her chair. Remaining Chairs %d\n", 3 - ChairsCount);
		CurrentIndex = (CurrentIndex + 1) % 3;
		pthread_mutex_unlock(&ChairAccess);

		printf("\t TA is currently helping the student.\n");
		sem_post(&Student_Sem);
		sleep(3);
	}
}

// ------------------------------------------------------------------------------------------

void main()
{
	int cstudents;		
	int i,input;

	sem_init(&TA_Sleep, 0, 0);
	sem_init(&Student_Sem, 0, 0);
	
	for(i = 0; i < 3; i++)			
	{	
		sem_init(&ChairsSem[i], 0, 1);		
	}

	
	input=rand()%10;
	pthread_mutex_init(&ChairAccess, NULL);
	
	if(input<=1)
	{
		printf("Number of Students not specified. Using default (4) students.\n");
		cstudents = 4;
	}
	
	else
	{
		printf("Number of Students specified. Creating %d threads.\n",cstudents);
		cstudents = input;
	}
	
	Students = malloc(sizeof(cstudents));	
	
	pthread_create(&TA, NULL, TA_Activity, NULL);	
	
	for(i = 0; i <cstudents; i++)
	{
		pthread_create(&Students[i], NULL, Student_Activity,(void*) (long)i);
	}
	
	pthread_join(TA, NULL);
	
	for(i = 0; i <cstudents; i++)
	{
		pthread_join(Students[i], NULL);
	}
}
