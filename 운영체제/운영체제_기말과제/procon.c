#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define BUFFER_SIZE 4

typedef struct {
    int buffer[BUFFER_SIZE];
    int in;
    int out;
} CircularQueue;

CircularQueue queue;
sem_t semWrite;
sem_t semRead;
pthread_mutex_t mutex;

void enqueue(int item) {
    queue.buffer[queue.in] = item;
    queue.in = (queue.in + 1) % BUFFER_SIZE;
}

int dequeue() {
    int item = queue.buffer[queue.out];
    queue.out = (queue.out + 1) % BUFFER_SIZE;
    return item;
}

void *producer(void *param) {
    for (int i = 0; i < 10; i++) {
        sleep(rand() % 3);  // Random wait time between 0 and 2 seconds

        int item = i;
        sem_wait(&semWrite);
        pthread_mutex_lock(&mutex);

        enqueue(item);
        printf("producer : wrote %d\n", item);

        pthread_mutex_unlock(&mutex);
        sem_post(&semRead);
    }
    pthread_exit(0);
}

void *consumer(void *param) {
    for (int i = 0; i < 10; i++) {
        sleep(rand() % 3);  // Random wait time between 0 and 2 seconds

        sem_wait(&semRead);
        pthread_mutex_lock(&mutex);

        int item = dequeue();
        printf("consumer : read %d\n", item);

        pthread_mutex_unlock(&mutex);
        sem_post(&semWrite);
    }
    pthread_exit(0);
}

int main() {
    pthread_t producerThread, consumerThread;

    srand(time(NULL));  // Seed the random number generator

    queue.in = 0;
    queue.out = 0;

    sem_init(&semWrite, 0, BUFFER_SIZE);
    sem_init(&semRead, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    pthread_create(&producerThread, NULL, producer, NULL);
    pthread_create(&consumerThread, NULL, consumer, NULL);

    pthread_join(producerThread, NULL);
    pthread_join(consumerThread, NULL);

    sem_destroy(&semWrite);
    sem_destroy(&semRead);
    pthread_mutex_destroy(&mutex);

    return 0;
}
