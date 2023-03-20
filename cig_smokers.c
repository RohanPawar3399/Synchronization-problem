#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include<stdbool.h>
#include<time.h>

typedef struct {
	int value;
	pthread_cond_t cond;
	pthread_mutex_t lock;	
}sem_t;
void sem_init(sem_t *s,int value)
{
	s->value=value;
	pthread_cond_init(&s->cond,NULL);
	pthread_mutex_init(&s->lock,NULL);
}

void sem_wait(sem_t *s)
{
	pthread_mutex_lock(&s->lock);
	while(s->value<=0)
		pthread_cond_wait(&s->cond,&s->lock);
	s->value--;
	pthread_mutex_unlock(&s->lock);
	return;
}

void sem_post(sem_t *s)
{
	pthread_mutex_lock(&s->lock);
	s->value++;
	pthread_cond_signal(&s->cond);
	pthread_mutex_unlock(&s->lock);
	return;
}
sem_t agent_sem;
sem_t tobacco;
sem_t paper;
sem_t match;

sem_t tobacco_sem; //for smoker with tobacco
sem_t paper_sem; //for smoker with paper
sem_t match_sem; //for smoker with match

pthread_mutex_t pusher_mutex; //only one pusher can go at a time

//used for pusher
bool is_tobacco = false;
bool is_paper = false;
bool is_match = false;

void* agnt(void*)
{
	while(1)
	{
		srand(time(0));
		int rnd;
		rnd=(rand()%3)+1;
		if(rnd==1)
		{
			sem_wait(&agent_sem);
			printf("agent: paper and tobacco is released.\n");
			fflush(stdout);
			sem_post(&tobacco);
			sem_post(&paper);
		}
		else if(rnd==2)
		{
			sem_wait(&agent_sem);
			printf("agent: paper and match is released.\n");
			fflush(stdout);
			sem_post(&paper);
			sem_post(&match);
		}
		else
		{
			sem_wait(&agent_sem);
			printf("agent: tobacco and match is released.\n");
			fflush(stdout);
			sem_post(&tobacco);
			sem_post(&match);
		}  		
	}
}


void *pusher_t() 
{
	while(1) {
		sem_wait(&tobacco);
		pthread_mutex_lock(&pusher_mutex);
		if(is_paper) {
			is_paper = false;
			sem_post(&match_sem);
		}
		else if(is_match) {
			is_match = false;
			sem_post(&paper_sem);
		}
		else {
			is_tobacco = true;
		}
		pthread_mutex_unlock(&pusher_mutex);
	}
}


void *pusher_p() 
{
	while(1) {
		sem_wait(&paper);
		pthread_mutex_lock(&pusher_mutex);
		if(is_match) {
			is_match = false;;
			sem_post(&tobacco_sem);
		}
		else if(is_tobacco) {
			is_tobacco = false;;
			sem_post(&match_sem);
		}
		else {
			is_paper =true;
		}
		pthread_mutex_unlock(&pusher_mutex);
	}
}

void *pusher_m() 
{
	while(1) {
		sem_wait(&match);
		pthread_mutex_lock(&pusher_mutex);
		if(is_paper) {
			is_paper = false;
			sem_post(&tobacco_sem);
		}
		else if(is_tobacco) {
			is_tobacco = false;
			sem_post(&paper_sem);
		}
		else {
			is_match =true;
		}
		pthread_mutex_unlock(&pusher_mutex);
	}
}


void *smoker_t() 
{	
	while(1) {
		sem_wait(&tobacco_sem);
		sem_post(&agent_sem);
		printf("Smoker with tobacco: smokes.\n");
		fflush(stdout);
		sleep(2);
	}
}

//smoker with paper
void *smoker_p() 
{
	while(1) {
		sem_wait(&paper_sem);
		sem_post(&agent_sem);
		printf("Smoker with paper: smokes.\n");
		fflush(stdout);
		sleep(2);
	}
}

//smoker with match
void *smoker_m() 
{
	while(1) {
		sem_wait(&match_sem);
		sem_post(&agent_sem);
		printf("Smoker with match: smokes.\n");
		fflush(stdout);
		sleep(2);
	}
}

int main()
{	
	sem_init(&agent_sem,  1);
	sem_init(&tobacco, 0);
	sem_init(&paper,  0);
	sem_init(&match,  0);
	sem_init(&tobacco_sem,  0);
	sem_init(&paper_sem,  0);
	sem_init(&match_sem,  0);
	pthread_mutex_init(&pusher_mutex, NULL);
	pthread_t smoker_with_match,smoker_with_tbc,smoker_with_paper;
	pthread_t tbc_pusher,paper_pusher,match_pusher,agent;
	pthread_create(&smoker_with_tbc, NULL, smoker_t, NULL);
	pthread_create(&smoker_with_paper, NULL, smoker_p, NULL);
	pthread_create(&smoker_with_match, NULL, smoker_m, NULL);
	pthread_create(&tbc_pusher, NULL, pusher_t, NULL);
	pthread_create(&paper_pusher, NULL, pusher_p, NULL);
	pthread_create(&match_pusher, NULL, pusher_m, NULL);
	pthread_create(&agent, NULL, agnt, NULL);
	
	pthread_join(agent,NULL);
	pthread_join(smoker_with_match,NULL);
	pthread_join(smoker_with_paper,NULL);
	pthread_join(smoker_with_tbc,NULL);
	pthread_join(tbc_pusher,NULL);
	pthread_join(paper_pusher,NULL);
	pthread_join(match_pusher,NULL);


	return 0;
}
