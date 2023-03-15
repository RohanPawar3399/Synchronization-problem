#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define BUFFER_SIZE 11

int buffer[BUFFER_SIZE]; // shared buffer
int count = 0; // count of items in buffer
int in = 0; // index to add item to buffer
int out = 0; // index to remove item from buffer

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // mutex to protect buffer access
pthread_cond_t full = PTHREAD_COND_INITIALIZER; // condition variable for buffer full
pthread_cond_t empty = PTHREAD_COND_INITIALIZER; // condition variable for buffer empty

void *producer(void *arg) {
    int i;
    int item;
    for (i = 0; i < 5; i++) { // produce 5 items
        item = rand(); // generate a random number as item
        pthread_mutex_lock(&mutex); // acquire the mutex
        while (count == BUFFER_SIZE) { // if buffer is full, wait for it to become empty
            pthread_cond_wait(&empty, &mutex);
        }
        buffer[in] = item; // add the item to buffer
        in = (in + 1) % BUFFER_SIZE; // increment the index to add item
        count++; // increment the count of items in buffer
        printf("Produced item %d\n", item);
        pthread_cond_signal(&full); // signal that buffer is not empty anymore
        pthread_mutex_unlock(&mutex); // release the mutex
    }
    pthread_exit(NULL);
}

void *consumer(void *arg) {
    int i;
    int item;
    for (i = 0; i < 5; i++) { // consume 5 items
        pthread_mutex_lock(&mutex); // acquire the mutex
        while (count == 0) { // if buffer is empty, wait for it to become full
            pthread_cond_wait(&full, &mutex);
        }
        item = buffer[out]; // remove the item from buffer
        out = (out + 1) % BUFFER_SIZE; // increment the index to remove item
        count--; // decrement the count of items in buffer
        printf("Consumed item %d\n", item);
        pthread_cond_signal(&empty); // signal that buffer is not full anymore
        pthread_mutex_unlock(&mutex); // release the mutex
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t prod_tid, cons_tid;
    srand(time(NULL)); // seed the random number generator
    pthread_create(&prod_tid, NULL, producer, NULL); // create producer thread
    pthread_create(&cons_tid, NULL, consumer, NULL); // create consumer thread
    pthread_join(prod_tid, NULL); // wait for producer thread to finish
    pthread_join(cons_tid, NULL); // wait for consumer thread to finish
    return 0;
}
