#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

// UNISEX BATHROOM PROBLEM WITH NO STARVATION //

   /* contributors :- 
 ARYAN BATRA 21114019
 MANOJ KUMAR SIAL 21114059  */
              

typedef struct {
	int value;
	pthread_cond_t cond;
	pthread_mutex_t lock;	
}sem_t;

void sem_init(sem_t *s,int value){
	s->value=value;
	pthread_cond_init(&s->cond,NULL);
	pthread_mutex_init(&s->lock,NULL);
}

void sem_wait(sem_t *s){
	pthread_mutex_lock(&s->lock);
	while(s->value<=0)
		pthread_cond_wait(&s->cond,&s->lock);
	s->value--;
	pthread_mutex_unlock(&s->lock);
	return;
}

void sem_post(sem_t *s){
	pthread_mutex_lock(&s->lock);
	s->value++;
	pthread_cond_signal(&s->cond);
	pthread_mutex_unlock(&s->lock);
	return;
}

#define N 15 // Number of people

sem_t bathroom, male_turn, female_turn, mutex;
int male_count = 0, female_count = 0, male_waiting = 0, num_female_waiting = 0;


void* male(void* arg){
    int id = *(int*)arg;

    // Wait to enter bathroom
    sem_wait(&mutex);
    if (num_female_waiting > 0 || female_count > 0 ) {
        male_waiting++;
        sem_post(&mutex);
        sem_wait(&male_turn);
        male_waiting--;
    }
    male_count++;
    sem_post(&mutex);

    // Enter bathroom
    printf("Male %d enters bathroom.\n", id);
    sem_wait(&bathroom);
    sleep(rand() % 5 + 1);
    sem_post(&bathroom);

    // Leave bathroom
    sem_wait(&mutex);
    male_count--;
    if (male_count == 0 && num_female_waiting > 0) {
        int n = num_female_waiting;
        num_female_waiting = 0;
        for (int i = 0; i < n; i++) {
            sem_post(&female_turn);
        }
    }
    sem_post(&mutex);

    pthread_exit(NULL);
}

void* female(void* arg)
{
    int id = *(int*)arg;

    // Wait to enter bathroom
    sem_wait(&mutex);
    if (male_count > 0 || male_waiting > 0) {
        num_female_waiting++;
        sem_post(&mutex);
        sem_wait(&female_turn);
        num_female_waiting--;
    }
    female_count++;
    sem_post(&mutex);

    // Enter bathroom
    printf("Female %d enters bathroom.\n", id);
    sem_wait(&bathroom);
    sleep(rand() % 5 + 1);
    sem_post(&bathroom);

    // Leave bathroom

    female_count--;
    if (female_count == 0 && male_waiting > 0) {
        int n = male_waiting;
        male_waiting = 0;
        for (int i = 0; i < n; i++) {
            sem_post(&male_turn);
        }
    }
    sem_post(&mutex);

    pthread_exit(NULL);
}

int main()
{
    srand(time(NULL));

    sem_init(&bathroom,1);
    sem_init(&male_turn,0);
    sem_init(&female_turn,0);
    sem_init(&mutex,1);

    pthread_t people[N];
    int ids[N];
    for (int i = 0; i < N; i++) {
        ids[i] = i;
        if (rand() % 2 == 0) {
            pthread_create(&people[i], NULL, male, &ids[i]);
        } else {
            pthread_create(&people[i], NULL, female, &ids[i]);
        }
    }

    for (int i = 0; i < N; i++) {
        pthread_join(people[i], NULL);
    }

    return 0;
}
