#include <pthread.h>
#include <iostream>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
int num_cust = 7;
using namespace std;

sem_t barber_ready; // Binary Semaphore for barber's readiness
sem_t customer_arr; // Binary Semaphore responsible for customer arrival
sem_t mutex; // Binary Semaphore required for access to chairs
int seats = 4; // Number of free seats in the waiting room

void *barber(void *val) {
    while (1) {
        sem_wait(&customer_arr); // Wait for customer to arrive
        sem_wait(&mutex); // Secure chair access
        seats++; // A seat has become free as one waiting cutomer is being served
        sem_post(&barber_ready); // Signal that barber is ready to cut hair
        sem_post(&mutex); // Unlock access to chairs
        cout <<"Barber is cutting hair."<<endl;
        fflush(stdout);
        sleep(2); // Cutting hair
    }
    return NULL;
}

void *customer(void *val) {
    int cust_id = *(int*)val;
    bool found_seat = false;
    while(found_seat!=true){
    	sem_wait(&mutex); // Secure chair access, no. of customers in the waiting room is protected by mutex
	    if (seats > 0) {
	        seats--; // Occupy a seat in the waiting room
	        found_seat = true;
	        sem_post(&customer_arr); // Signal the barber that customer has arrived
	        sem_post(&mutex); // Unlock access to chairs
	        sem_wait(&barber_ready); // Wait for the barber's readiness
	        cout<<cust_id<<" is getting a haircut."<<endl;
	        fflush(stdout);
	        sleep(1); // Getting haircut
	    } else {
	        sem_post(&mutex); // Unlock access to chairs
	        cout<<cust_id<<" left the shop because waiting room is full."<<endl;
	        fflush(stdout);
	        sleep(rand()%5+1); // Random delay of 1 to 5s before returning
	    }
	}
    
    return NULL;
}

int main() {
	srand(time(NULL)); // Seed the random number generator
    pthread_t barber_thread;
    pthread_t customer_threads[num_cust];
    sem_init(&customer_arr, 0, 0);
    sem_init(&barber_ready, 0, 0);
    sem_init(&mutex, 0, 1);
    pthread_create(&barber_thread, NULL, barber, NULL);
    int ids[num_cust];
    for (int i = 0; i < num_cust; i++) {
        ids[i] = i+1;
        pthread_create(&customer_threads[i], NULL, customer, (void *) &ids[i]);
        //sleep(1); // New customer arrives every 1 second
    }
    for (int i = 0; i < num_cust; i++) {
        pthread_join(customer_threads[i], NULL);
    }
    return 0;
}
