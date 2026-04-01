#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>

#define PORT 8090                      // 서버가 사용할 포트 번호
#define MAX_BUFFER_SIZE 2000           // 요청 버퍼의 최대 크기
#define INITIAL_THREAD_POOL_SIZE 10    // 초기 스레드 풀의 크기
#define NUMBER_OF_POOLS 1              // 초기 설정을 위한 스레드 풀의 수
#define MONITOR_INTERVAL 5             // 모니터링 간격(초 단위)

typedef struct {
    int client_socket;                // 클라이언트 소켓 파일 디스크립터
} Request;

typedef struct {
    Request buffer[MAX_BUFFER_SIZE];  // 요청을 저장할 버퍼
    int head;                         // 버퍼의 헤드 인덱스
    int tail;                         // 버퍼의 테일 인덱스
    int size;                         // 버퍼의 현재 크기
    pthread_mutex_t lock;             // 버퍼 접근을 위한 뮤텍스
    pthread_cond_t not_empty;         // 버퍼가 비어 있지 않음을 알리는 조건 변수
} RequestBuffer;

typedef struct {
    pthread_t *threads;               // 스레드 배열
    int size;                         // 스레드 풀의 크기
    int active;                       // 스레드 풀 활성 상태
    RequestBuffer request_buffer;     // 요청 버퍼
} ThreadPool;

typedef struct {
    ThreadPool *thread_pools;         // 스레드 풀 배열
    int num_pools;                    // 스레드 풀의 수
    int next_pool;                    // 다음에 사용할 스레드 풀의 인덱스
    pthread_mutex_t lock;             // 스레드 풀 관리자 접근을 위한 뮤텍스
} ThreadPoolManager;

ThreadPoolManager thread_pool_manager; // 전역 스레드 풀 관리자
pthread_mutex_t log_mutex;             // 로그 출력을 위한 뮤텍스

/*** @par init_request_buffer
 * 요청 버퍼 초기화
 * @param buffer 초기화할 요청 버퍼 포인터
 ***/
void init_request_buffer(RequestBuffer *buffer) {
    buffer->head = 0;                          // 헤드 인덱스 초기화
    buffer->tail = 0;                          // 테일 인덱스 초기화
    buffer->size = 0;                          // 버퍼 크기 초기화
    pthread_mutex_init(&buffer->lock, NULL);   // 뮤텍스 초기화
    pthread_cond_init(&buffer->not_empty, NULL); // 조건 변수 초기화
}

/*** @par is_buffer_full
 * 요청 버퍼가 가득 찼는지 확인
 * @param buffer 확인할 요청 버퍼 포인터
 * @return 버퍼가 가득 찼으면 1, 그렇지 않으면 0
 ***/
int is_buffer_full(RequestBuffer *buffer) {
    return buffer->size == MAX_BUFFER_SIZE;    // 버퍼 크기가 최대 크기인지 확인
}

/*** @par is_buffer_empty
 * 요청 버퍼가 비었는지 확인
 * @param buffer 확인할 요청 버퍼 포인터
 * @return 버퍼가 비었으면 1, 그렇지 않으면 0
 ***/
int is_buffer_empty(RequestBuffer *buffer) {
    return buffer->size == 0;                  // 버퍼 크기가 0인지 확인
}

/*** @par enqueue_request
 * 요청을 버퍼에 추가
 * @param buffer 추가할 요청 버퍼 포인터
 * @param client_socket 추가할 클라이언트 소켓
 * @param pool_index 요청이 추가되는 풀의 인덱스
 ***/
void enqueue_request(RequestBuffer *buffer, int client_socket, int pool_index) {
    pthread_mutex_lock(&buffer->lock);          // 뮤텍스 잠금
    while (is_buffer_full(buffer)) {            // 버퍼가 가득 차면 대기
        pthread_cond_wait(&buffer->not_empty, &buffer->lock);
    }
    buffer->buffer[buffer->tail].client_socket = client_socket; // 요청을 버퍼에 추가
    buffer->tail = (buffer->tail + 1) % MAX_BUFFER_SIZE;        // 테일 인덱스 업데이트
    buffer->size++;                                             // 버퍼 크기 증가
    pthread_cond_signal(&buffer->not_empty);    // 조건 변수 신호
    pthread_mutex_unlock(&buffer->lock);        // 뮤텍스 잠금 해제
    
    // 요청이 큐에 추가되었음을 로그로 남김
    if (client_socket != -1) {  // 종료 신호에 대해서는 로그를 남기지 않음
        pthread_mutex_lock(&log_mutex);
        printf("Request enqueued in buffer of pool %d. Current size: %d\n", pool_index, buffer->size);
        pthread_mutex_unlock(&log_mutex);
    }
}

/*** @par dequeue_request
 * 버퍼에서 요청을 제거
 * @param buffer 제거할 요청 버퍼 포인터
 * @return 제거된 클라이언트 소켓
 ***/
int dequeue_request(RequestBuffer *buffer) {
    pthread_mutex_lock(&buffer->lock);          // 뮤텍스 잠금
    while (is_buffer_empty(buffer)) {           // 버퍼가 비어 있으면 대기
        pthread_cond_wait(&buffer->not_empty, &buffer->lock);
    }
    int client_socket = buffer->buffer[buffer->head].client_socket; // 버퍼에서 요청 제거
    buffer->head = (buffer->head + 1) % MAX_BUFFER_SIZE;            // 헤드 인덱스 업데이트
    buffer->size--;                                                 // 버퍼 크기 감소
    pthread_cond_signal(&buffer->not_empty);    // 조건 변수 신호
    pthread_mutex_unlock(&buffer->lock);        // 뮤텍스 잠금 해제
    return client_socket;
}

/*** @par init_thread_pool
 * 스레드 풀 초기화
 * @param pool 초기화할 스레드 풀 포인터
 * @param size 스레드 풀의 크기
 ***/
void init_thread_pool(ThreadPool *pool, int size) {
    pool->size = size;                          // 스레드 풀 크기 설정
    pool->active = 1;                           // 스레드 풀 활성화 상태 설정
    pool->threads = (pthread_t *)malloc(size * sizeof(pthread_t)); // 스레드 배열 할당
    if (pool->threads == NULL) {
        perror("Failed to allocate memory for threads");
        exit(EXIT_FAILURE);
    }
    init_request_buffer(&pool->request_buffer); // 요청 버퍼 초기화
}

/*** @par thread_function
 * 스레드가 실행할 함수
 * @param arg 요청 버퍼 포인터
 * @return NULL
 ***/
void *thread_function(void *arg) {
    RequestBuffer *buffer = (RequestBuffer *)arg; // 요청 버퍼 포인터
    while (1) {
        int client_socket = dequeue_request(buffer); // 요청 제거
        if (client_socket == -1) {
            break; // 종료 신호를 받으면 루프 종료
        }
        
        struct timespec start_time, end_time;
        clock_gettime(CLOCK_MONOTONIC, &start_time); // 시작 시간 기록
        
        // 처리 시간을 시뮬레이션
        sleep(1.99); // 1.99초 동안 대기 (실험을 위해 조정)
        
        // 요청 처리
        char buffer[30000] = {0};
        read(client_socket, buffer, 30000); // 클라이언트로부터 데이터 읽기
        pthread_mutex_lock(&log_mutex);
        printf("%s\n", buffer); // 읽은 데이터 출력
        pthread_mutex_unlock(&log_mutex);
        char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain"
                      "Content-Length: 20\n\nMy first web server!";
        write(client_socket, hello, strlen(hello)); // 클라이언트에 응답 쓰기
        close(client_socket); // 클라이언트 소켓 닫기
        
        clock_gettime(CLOCK_MONOTONIC, &end_time); // 종료 시간 기록
        
        // 작업 시간 계산
        double task_time = (end_time.tv_sec - start_time.tv_sec) + 
                           (end_time.tv_nsec - start_time.tv_nsec) / 1e9;
        
        // 작업 시간 로그 기록
        pthread_mutex_lock(&log_mutex);
        printf("Task time: %.6f seconds\n", task_time);
        pthread_mutex_unlock(&log_mutex);
    }
    return NULL;
}

/*** @par start_thread_pool
 * 스레드 풀 시작
 * @param pool 시작할 스레드 풀 포인터
 ***/
void start_thread_pool(ThreadPool *pool) {
    for (int i = 0; i < pool->size; i++) {
        pthread_create(&pool->threads[i], NULL, thread_function, (void *)&pool->request_buffer); // 스레드 생성 및 시작
    }
}

/*** @par gracefully_shutdown_thread_pool
 * 스레드 풀 정상 종료
 * @param pool 종료할 스레드 풀 포인터
 ***/
void gracefully_shutdown_thread_pool(ThreadPool *pool) {
    pool->active = 0; // 새 요청이 큐에 추가되지 않도록 풀을 비활성화

    // 모든 스레드가 현재 작업을 완료한 후 종료되도록 신호를 보냄
    for (int i = 0; i < pool->size; i++) {
        enqueue_request(&pool->request_buffer, -1, -1); // -1은 종료 신호
    }

    // 모든 스레드가 종료될 때까지 대기
    for (int i = 0; i < pool->size; i++) {
        pthread_join(pool->threads[i], NULL);
    }

    // 풀 정리
    free(pool->threads);
    pthread_mutex_destroy(&pool->request_buffer.lock);
    pthread_cond_destroy(&pool->request_buffer.not_empty);
}

/*** @par stop_thread_pool
 * 스레드 풀 종료
 * @param pool 종료할 스레드 풀 포인터
 ***/
void stop_thread_pool(ThreadPool *pool) {
    gracefully_shutdown_thread_pool(pool);
}

/*** @par init_thread_pool_manager
 * 스레드 풀 관리자 초기화
 * @param manager 초기화할 스레드 풀 관리자 포인터
 * @param num_pools 생성할 스레드 풀의 수
 * @param pool_size 각 스레드 풀의 크기
 ***/
void init_thread_pool_manager(ThreadPoolManager *manager, int num_pools, int pool_size) {
    pthread_mutex_init(&manager->lock, NULL); // 뮤텍스 초기화
    pthread_mutex_lock(&manager->lock);       // 뮤텍스 잠금
    manager->num_pools = num_pools;           // 스레드 풀 수 설정
    manager->next_pool = 0;                   // 다음에 사용할 스레드 풀 인덱스 초기화
    manager->thread_pools = (ThreadPool *)malloc(num_pools * sizeof(ThreadPool)); // 스레드 풀 배열 할당
    if (manager->thread_pools == NULL) {
        perror("Failed to allocate memory for thread pools");
        pthread_mutex_unlock(&manager->lock);
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < num_pools; i++) {
        init_thread_pool(&manager->thread_pools[i], pool_size); // 각 스레드 풀 초기화
        start_thread_pool(&manager->thread_pools[i]); // 스레드 풀 시작
    }
    pthread_mutex_unlock(&manager->lock); // 뮤텍스 잠금 해제
}

/*** @par add_thread_pool
 * 새로운 스레드 풀 추가
 * @param manager 스레드 풀 관리자 포인터
 * @param pool_size 새 스레드 풀의 크기
 ***/
void add_thread_pool(ThreadPoolManager *manager, int pool_size) {
    pthread_mutex_lock(&manager->lock); // 뮤텍스 잠금
    ThreadPool* temp = realloc(manager->thread_pools, (manager->num_pools + 1) * sizeof(ThreadPool)); // 스레드 풀 배열 크기 증가
    if (temp == NULL) {
        pthread_mutex_unlock(&manager->lock); // 뮤텍스 잠금 해제
        pthread_mutex_lock(&log_mutex);
        printf("Failed to add new ThreadPool. Memory allocation failed.\n"); // 메모리 할당 실패 로그
        pthread_mutex_unlock(&log_mutex);
        return;
    }
    manager->thread_pools = temp;
    init_thread_pool(&manager->thread_pools[manager->num_pools], pool_size); // 새로운 스레드 풀 초기화
    start_thread_pool(&manager->thread_pools[manager->num_pools]); // 새로운 스레드 풀 시작
    manager->num_pools++; // 스레드 풀 수 증가
    pthread_mutex_unlock(&manager->lock); // 뮤텍스 잠금 해제

    // 새로운 스레드 풀 추가 로그 기록
    pthread_mutex_lock(&log_mutex);
    printf("Added new ThreadPool. Total pools: %d\n", manager->num_pools);
    pthread_mutex_unlock(&log_mutex);
}

/*** @par remove_thread_pool
 * 스레드 풀 제거
 * @param manager 스레드 풀 관리자 포인터
 * @param pool_index 제거할 스레드 풀의 인덱스
 ***/
void remove_thread_pool(ThreadPoolManager *manager, int pool_index) {
    pthread_mutex_lock(&manager->lock); // 뮤텍스 잠금
    if (pool_index >= 0 && pool_index < manager->num_pools) {
        gracefully_shutdown_thread_pool(&manager->thread_pools[pool_index]); // 스레드 풀 정상 종료

        // 배열의 나머지 풀을 이동하여 빈 공간을 채움
        for (int i = pool_index; i < manager->num_pools - 1; i++) {
            manager->thread_pools[i] = manager->thread_pools[i + 1];
        }

        // 풀 수 업데이트
        manager->num_pools--;
        ThreadPool* temp = realloc(manager->thread_pools, manager->num_pools * sizeof(ThreadPool)); // 스레드 풀 배열 크기 감소
        if (temp == NULL && manager->num_pools > 0) {
            perror("Failed to shrink the memory allocation for thread pools");
            pthread_mutex_unlock(&manager->lock);
            exit(EXIT_FAILURE);
        }
        manager->thread_pools = temp;
    }
    pthread_mutex_unlock(&manager->lock); // 뮤텍스 잠금 해제

    // 스레드 풀 제거 로그 기록
    pthread_mutex_lock(&log_mutex);
    printf("Removed a ThreadPool. Total pools: %d\n", manager->num_pools);
    pthread_mutex_unlock(&log_mutex);
}

/*** @par enqueue_request_round_robin
 * 라운드 로빈 방식으로 요청을 스레드 풀에 추가
 * @param manager 스레드 풀 관리자 포인터
 * @param client_socket 추가할 클라이언트 소켓
 ***/
void enqueue_request_round_robin(ThreadPoolManager *manager, int client_socket) {
    pthread_mutex_lock(&manager->lock); // 뮤텍스 잠금
    int pool_index = manager->next_pool; // 다음에 사용할 스레드 풀 인덱스
    ThreadPool *pool = &manager->thread_pools[pool_index]; // 스레드 풀 포인터
    printf("Request being assigned to ThreadPool %d\n", pool_index); // 요청이 할당된 스레드 풀 로그 기록
    manager->next_pool = (manager->next_pool + 1) % manager->num_pools; // 다음 스레드 풀 인덱스 업데이트
    pthread_mutex_unlock(&manager->lock); // 뮤텍스 잠금 해제
    enqueue_request(&pool->request_buffer, client_socket, pool_index); // 요청 버퍼에 추가
}

/*** @par cleanup_thread_pool_manager
 * 스레드 풀 관리자 정리
 * @param manager 스레드 풀 관리자 포인터
 ***/
void cleanup_thread_pool_manager(ThreadPoolManager *manager) {
    pthread_mutex_lock(&manager->lock); // 뮤텍스 잠금
    for (int i = 0; i < manager->num_pools; i++) {
        stop_thread_pool(&manager->thread_pools[i]); // 각 스레드 풀 종료
    }
    free(manager->thread_pools); // 스레드 풀 배열 메모리 해제
    pthread_mutex_unlock(&manager->lock); // 뮤텍스 잠금 해제
}

/*** @par monitor_and_adjust_pools
 * 스레드 풀의 수를 모니터링하고 조정
 * @param arg 스레드 풀 관리자 포인터
 * @return NULL
 ***/
void *monitor_and_adjust_pools(void *arg) {
    ThreadPoolManager *manager = (ThreadPoolManager *)arg; // 스레드 풀 관리자 포인터
    while (1) {
        int total_pending_requests = 0;
        pthread_mutex_lock(&manager->lock); // 뮤텍스 잠금
        for (int i = 0; i < manager->num_pools; i++) {
            pthread_mutex_lock(&manager->thread_pools[i].request_buffer.lock); // 각 스레드 풀의 요청 버퍼 뮤텍스 잠금
            total_pending_requests += manager->thread_pools[i].request_buffer.size; // 총 대기 중인 요청 수 계산
            pthread_mutex_unlock(&manager->thread_pools[i].request_buffer.lock); // 각 스레드 풀의 요청 버퍼 뮤텍스 잠금 해제
        }
        pthread_mutex_unlock(&manager->lock); // 뮤텍스 잠금 해제

        // 총 대기 중인 요청 수 로그 기록
        pthread_mutex_lock(&log_mutex);
        printf("Total pending requests: %d\n", total_pending_requests);
        pthread_mutex_unlock(&log_mutex);

        int required_pools = (total_pending_requests + 7) / 8;  // 총 대기 중인 요청 수 / 8의 올림값

        // 필요한 풀 수 로그 기록
        pthread_mutex_lock(&log_mutex);
        printf("Required pools: %d, Current pools: %d\n", required_pools, manager->num_pools);
        pthread_mutex_unlock(&log_mutex);

        // 필요한 경우 스레드 풀 추가
        while (required_pools > manager->num_pools) {
            add_thread_pool(manager, INITIAL_THREAD_POOL_SIZE);
        }

        // 필요한 경우 스레드 풀 제거
        while (required_pools < manager->num_pools && manager->num_pools > 1) {
            remove_thread_pool(manager, manager->num_pools - 1); // 마지막 풀 제거
        }

        // 일정 시간 대기 후 다시 확인
        sleep(MONITOR_INTERVAL);
    }
}

/*** @par main
 * 메인 함수
 * @param argc 인수의 수
 * @param argv 인수 배열
 * @return 0
 ***/
int main(int argc, char const *argv[]) {
    int server_fd, new_socket;
    struct sockaddr_in address; // 소켓 주소 구조체
    int addrlen = sizeof(address); // 주소 길이

    // 여러 개의 풀로 스레드 풀 관리자 초기화
    init_thread_pool_manager(&thread_pool_manager, NUMBER_OF_POOLS, INITIAL_THREAD_POOL_SIZE);

    // 로그 뮤텍스 초기화
    pthread_mutex_init(&log_mutex, NULL);

    // 모니터링 스레드 시작
    pthread_t monitor_thread;
    pthread_create(&monitor_thread, NULL, monitor_and_adjust_pools, (void *)&thread_pool_manager);

    // 서버 소켓 생성
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("In socket");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET; // IPv4
    address.sin_addr.s_addr = INADDR_ANY; // 모든 인터페이스에서 수신
    address.sin_port = htons(PORT); // 포트 설정

    memset(address.sin_zero, '\0', sizeof address.sin_zero); // 주소 구조체 초기화

    // 소켓 바인딩
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    // 소켓 리슨
    if (listen(server_fd, 10) < 0) {
        perror("In listen");
        exit(EXIT_FAILURE);
    }

    while (1) {
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        // 새로운 클라이언트 연결 수락
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("In accept");
            exit(EXIT_FAILURE);
        }
        enqueue_request_round_robin(&thread_pool_manager, new_socket); // 라운드 로빈 방식으로 요청을 스레드 풀에 추가
    }

    // 정리
    cleanup_thread_pool_manager(&thread_pool_manager); // 스레드 풀 관리자 정리
    pthread_mutex_destroy(&log_mutex); // 로그 뮤텍스 파괴

    return 0;
}
