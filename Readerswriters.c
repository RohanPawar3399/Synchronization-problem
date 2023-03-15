#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#define NTHREADS 5
#define MAX_LIMIT 256

int readCount =0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t roomEmpty = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t turnstile = PTHREAD_MUTEX_INITIALIZER;

void *reader(void *);
void *writer(void *);

void *doReading(void *);
void *doWriting(void *);

char *message; 

int main()
{
    message = (char *)malloc(MAX_LIMIT * sizeof(char));
    *message = "Hi";

    pthread_t thread_id[NTHREADS];
    for(int i=0;i<NTHREADS;i++)
    {
        void *func = (i%2)?reader:writer;
        int retc = pthread_create(&thread_id[i],NULL,func,NULL);
    }

    for(int i=0;i<NTHREADS;i++)
    {
        pthread_join(thread_id[i],NULL);
    }
    free(message);

    exit(0);
}

void *reader(void *dummyPtr)
{
    pthread_mutex_lock(&turnstile);
    pthread_mutex_unlock(&turnstile);

    pthread_mutex_lock(&mutex);
        readCount++;
        if(readCount==1)
            pthread_mutex_lock(&roomEmpty);
    pthread_mutex_unlock(&mutex);

    doReading(NULL);

    pthread_mutex_lock(&mutex);
        readCount--;
        if(readCount==0)
        pthread_mutex_unlock(&roomEmpty);
    pthread_mutex_unlock(&mutex);
}

void *writer(void *dummyPtr)
{
    pthread_mutex_lock(&turnstile);
    pthread_mutex_lock(&roomEmpty);
        doWriting(NULL);
    pthread_mutex_unlock(&roomEmpty);
    pthread_mutex_unlock(&turnstile);
}
 
void *doReading(void *x)
{
    printf("\nThread %ld is doing reading ",pthread_self());
    printf("\n%s\n",message);
}

void *doWriting(void *x)
{
    printf("\nGive data to be written for writer %ld ",pthread_self());
    // scanf("%s", message);
    fgets(message, MAX_LIMIT, stdin);
}