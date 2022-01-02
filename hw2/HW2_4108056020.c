#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#include<time.h>
int big_buffer[1024];
int max_buffer[4], min_buffer[4];
int maximum = 0, minimum = 2147483647;
int max_count = 0, min_count = 0;
sem_t max_buffer_full, min_buffer_full;
sem_t max_buffer_mutex, min_buffer_mutex;
pthread_t producers[4], consumers[2];
void *produce(void* p){
    // range of that 256 numbers in big buffer
    int t = *(int*)p * 256, end = t + 256;
    // find temporary max/min
    int temp_max = maximum, temp_min = minimum;
    for(int i = t; i < end; i++){
        t = big_buffer[i];
        temp_max = temp_max > t ? temp_max : t;
        temp_min = temp_min < t ? temp_min : t;
    }
    printf("Producer: Temporary max = %d min = %d\n", temp_max, temp_min);
    // put temp_max in max bufer
    sem_wait(&max_buffer_mutex);
    max_buffer[max_count] = temp_max;
    printf("Producer: Put %d into max-buffer at %d\n", temp_max, max_count++);
    sem_post(&max_buffer_mutex);
    sem_post(&max_buffer_full);
    // put temp_min in min bufer
    sem_wait(&min_buffer_mutex);
    min_buffer[min_count] = temp_min;
    printf("Producer: Put %d into min-buffer at %d\n", temp_min, min_count++);
    sem_post(&min_buffer_mutex);
    sem_post(&min_buffer_full);
    pthread_exit(NULL);
}
void *consume(void* c){
    if(*(int*)c) for(int i = -1; ++i < 4;){
        sem_wait(&max_buffer_full);
        sem_wait(&max_buffer_mutex);
        maximum = max_buffer[i] > maximum ? max_buffer[i] : maximum;
        printf("Updated! maximum = %d\n", maximum);
        sem_post(&max_buffer_mutex);
    }else for(int i = -1; ++i < 4;){
        sem_wait(&min_buffer_full);
        sem_wait(&min_buffer_mutex);
        minimum = min_buffer[i] < minimum ? min_buffer[i] : minimum;
        printf("Updated! minimum = %d\n", minimum);
        sem_post(&min_buffer_mutex);
    }
    pthread_exit(NULL);
}
int main(){
    srand(time(NULL));
    sem_init(&max_buffer_full, 0, 0);
    sem_init(&min_buffer_full, 0, 0);
    sem_init(&max_buffer_mutex, 0, 1);
    sem_init(&min_buffer_mutex, 0, 1);
    int p[4] = {0, 1, 2, 3};
    int c[2] = {0, 1};
    for(int i = -1; ++i < 1024; big_buffer[i] = rand());
    for(int i = -1; ++i < 4; pthread_create(&(producers[i]), NULL, produce, &p[i]));
    for(int i = -1; ++i < 2; pthread_create(&(consumers[i]), NULL, consume, &c[i]));
    for(int i = -1; ++i < 2; pthread_join(consumers[i], NULL));
    printf("Success! maximum = %d and minimum = %d\n", maximum, minimum);
    sem_destroy(&max_buffer_full);
    sem_destroy(&min_buffer_full);
    sem_destroy(&max_buffer_mutex);
    sem_destroy(&min_buffer_mutex);
    return EXIT_SUCCESS;
}