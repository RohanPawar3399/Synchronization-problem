#include <iostream>
#include <stdlib.h>
#include <pthread.h> 
using namespace std;
#define n 10
//Here Each of the fork is taken as a binary semaphore
int fork[n];
//here 1 indicates that fork is available, 0 indicates that fork is taken;
int phil[n] ;

void sleep(double d){
  while(d--);
}
void wait(int* fork1, int* fork2){
	
 //Wait till both the forks are not taken
  while(*fork1 == 0 || *fork2 == 0); 
 
  *fork1 -= 1;
  *fork2 -= 1;
}

void signal(int* fork1, int* fork2){
  *fork1 += 1;
  *fork2 += 1;
}
void* philospher(void* num) 
{
  int *ID = (int*)num;
  int i = *ID;
  //here i is Philosopher ID
    while (1) {
        cout<<"Philosopher "<<i<<" is Hungry"<<endl;
        wait(fork+i, fork+((i-1)%n));
        cout<<"Philosopher "<<i<<" is Eating"<<endl;
        //its the critical section
        sleep(200000000);        

        signal(fork + i,fork+((i-1)%n));
        cout<<"Philosopher "<<i<<" has finished Eating"<<endl;
        cout<<"Forks "<<i<<" & "<<(i-1)%n<<" are free"<<endl;
        //eating is done, wait for random time before hungry again;
        cout<<"Philosopher "<<i<<" is Thinking"<<endl;
        double waiter = rand() % 200000000 + 200000000;
        sleep(waiter); 
        
    } 
} 

int main() {
    pthread_t thread_id[n];
     for (int i = 0; i < n; i++){ 
        // create the philosopher processes 
        fork[i]=1;
        phil[i]=i;
        pthread_create(&thread_id[i], NULL, philospher, &phil[i]); 
     }
        // starting theprocesses 
        for (int i = 0; i < n; i++)   
        pthread_join(thread_id[i], NULL);
}
