#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdatomic.h>
#include <time.h>

#define BUFFER_SIZE 4
#define MAX_MESSAGES 20

int buffer[BUFFER_SIZE];
int in = 0;
int out = 0;
int next_value = 0;

typedef struct {
    _Atomic int value;
} Semaphore;

typedef struct {
    char message[50];
} Message;

Message messages[MAX_MESSAGES];
int message_index = 0;

pthread_mutex_t message_mutex;

void sem_init(Semaphore *sem, int value) {
    atomic_init(&sem->value, value);
}

void sem_wait(Semaphore *sem) {
    while (1) {
        int expected = atomic_load(&sem->value);
        while (expected <= 0) {
            expected = atomic_load(&sem->value);
        }
        if (atomic_compare_exchange_weak(&sem->value, &expected, expected - 1)) {
            break;
        }
    }
}

void sem_signal(Semaphore *sem) {
    atomic_fetch_add(&sem->value, 1);
}

Semaphore semWrite;
Semaphore semRead;

int flag[2] = {0, 0};
int turn;

#define barrier() asm volatile("mfence" ::: "memory")

void enter_critical_section(int self) {
    barrier();  // 메모리 배리어 추가
    flag[self] = 1;
    turn = 1 - self;
    while (flag[1 - self] == 1 && turn == 1 - self);
    barrier();  // 메모리 배리어 추가
}

void leave_critical_section(int self) {
    barrier();  // 메모리 배리어 추가
    flag[self] = 0;
    barrier();  // 메모리 배리어 추가
}

void *producer(void *param) {
    int self = 0;
    for (int i = 0; i < 10; i++) {
        sem_wait(&semWrite);
        enter_critical_section(self);

        buffer[in] = next_value;
        next_value = (next_value + 1) % 10;
        pthread_mutex_lock(&message_mutex);
        snprintf(messages[message_index++].message, 50, "producer : wrote %d", buffer[in]);
        pthread_mutex_unlock(&message_mutex);
        in = (in + 1) % BUFFER_SIZE;

        leave_critical_section(self);
        sem_signal(&semRead);
        sleep(rand() % 3);
    }
    pthread_exit(0);
}

void *consumer(void *param) {
    int self = 1;
    for (int i = 0; i < 10; i++) {
        sem_wait(&semRead);
        enter_critical_section(self);

        int item = buffer[out];
        pthread_mutex_lock(&message_mutex);
        snprintf(messages[message_index++].message, 50, "consumer : read %d", item);
        pthread_mutex_unlock(&message_mutex);
        out = (out + 1) % BUFFER_SIZE;

        leave_critical_section(self);
        sem_signal(&semWrite);
        sleep(rand() % 3);
    }
    pthread_exit(0);
}

int main() {
    pthread_t tid1, tid2;

    srand(time(NULL));  // 난수 생성기 초기화
    pthread_mutex_init(&message_mutex, NULL);

    sem_init(&semWrite, BUFFER_SIZE);
    sem_init(&semRead, 0);

    pthread_create(&tid1, NULL, producer, NULL);
    pthread_create(&tid2, NULL, consumer, NULL);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    for (int i = 0; i < message_index; i++) {
        printf("%s\n", messages[i].message);
        fflush(stdout);  // 출력 버퍼 강제 플러시
    }

    pthread_mutex_destroy(&message_mutex);
    return 0;
}

