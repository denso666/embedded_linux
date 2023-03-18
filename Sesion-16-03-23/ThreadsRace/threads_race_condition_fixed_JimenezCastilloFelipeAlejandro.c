#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

int x = 0;
pthread_mutex_t mutex_addition = PTHREAD_MUTEX_INITIALIZER;
sem_t semaphore_addition;

void* thread_func(void* args){

    int i;
    for ( i = 0; i < 10000000; i++ )
    {
        //critical region
        pthread_mutex_lock(&mutex_addition); // lock resource
        x++; // errata from the processes class: this is not atomic
        pthread_mutex_unlock(&mutex_addition); // unlock resource
        sem_post(&semaphore_addition); // post resource
    }
}
void* thread_func2(void* args) {
    int i = 0;

    for (int i = 0; i < 5000000; i++)
    {
        sem_wait(&semaphore_addition); // consume resource
        pthread_mutex_lock(&mutex_addition); // lock resource
        x--; // errata from the processes class: this is not atomic
        if (x < 0)
        {
            printf("stack underflow :(\n");
        }
        pthread_mutex_unlock(&mutex_addition); // unlock resource
    }
}

int main () {
    // semaphore initilize
    sem_init(&semaphore_addition, 0, 0);

    pthread_t thread_id[3];

    printf("I will create two threads\n");
    printf("Point 1 >> X is: %d\n", x);
    pthread_create(&thread_id[0],NULL,&thread_func,NULL);
    pthread_create(&thread_id[1],NULL,&thread_func,NULL);
    pthread_create(&thread_id[2],NULL,&thread_func2,NULL);
    printf("The threads has been created\n");
    pthread_join(thread_id[0],NULL);
    pthread_join(thread_id[1],NULL);
    pthread_join(thread_id[2],NULL);
    printf("The threads have exited\n");
    printf("Point 2 >> X is: %d\n", x);

    return 0;
}
