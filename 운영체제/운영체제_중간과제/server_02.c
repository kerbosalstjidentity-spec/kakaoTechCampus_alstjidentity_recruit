#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>

// in your browser type: http://localhost:8090
// IF error: address in use then change the PORT number
#define PORT 8090 // 포트 번호 정의

/**
 * @brief 새로운 연결을 처리하는 함수의 선언
 */
void *handle_connection(void *socket_desc);

int main(int argc, char const *argv[])
{
    int server_fd, new_socket; // 서버 소켓과 새 클라이언트 소켓을 위한 파일 디스크립터
    struct sockaddr_in address; // 소켓 주소 구조체
    int addrlen = sizeof(address); // 주소 길이

    // 소켓 생성
    /**
     * @brief 소켓 생성
     */
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }

    // 주소 구조체 설정
    /**
     * @brief 주소 구조체 설정
     */
    address.sin_family = AF_INET; // 주소 체계 설정 (IPv4)
    address.sin_addr.s_addr = INADDR_ANY; // 모든 IP 주소에서의 연결 허용
    address.sin_port = htons(PORT); // 포트 번호 설정

    // 주소 구조체의 나머지 부분 초기화
    /**
     * @brief 주소 구조체의 나머지 부분 초기화
     */
    memset(address.sin_zero, '\0', sizeof address.sin_zero); // 구조체의 나머지 부분을 0으로 초기화

    // 소켓에 주소 바인딩
    /**
     * @brief 소켓에 주소 바인딩
     */
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }

    // 연결 대기열 설정
    /**
     * @brief 연결 대기열 설정
     */
    if (listen(server_fd, 10) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }

    // 무한 루프를 돌면서 클라이언트 연결을 기다림
    /**
     * @brief 무한 루프를 돌면서 클라이언트 연결을 기다림
     */
    while (1)
    {
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");

        // 클라이언트 연결 수락
        /**
         * @brief 클라이언트 연결 수락
         */
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }

        // 새로운 스레드 생성
        /**
         * @brief 새로운 스레드 생성
         */
        pthread_t thread_id; // 스레드 ID
        int *new_sock = malloc(sizeof(int)); // 클라이언트 소켓을 위한 메모리 할당
        *new_sock = new_socket; // 클라이언트 소켓 디스크립터 저장

        // 클라이언트 연결 처리 스레드 생성
        if (pthread_create(&thread_id, NULL, handle_connection, (void *)new_sock) < 0)
        {
            perror("could not create thread");
            exit(EXIT_FAILURE);
        }

        printf("Handler assigned\n");
    }
    return 0;
}

/**
 * @brief 클라이언트 연결을 처리하는 함수
 * 
 * @param socket_desc 소켓 디스크립터 포인터
 * @return void* 
 */
void *handle_connection(void *socket_desc)
{
    int sock = *(int *)socket_desc; // 클라이언트 소켓 디스크립터
    long valread; // 읽은 바이트 수
    char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain"
                  "Content-Length: 20\n\nMy first web server!"; // 응답 메시지
    char buffer[30000] = {0}; // 클라이언트 요청을 저장할 버퍼

    // 클라이언트로부터 데이터 읽기
    /**
     * @brief 클라이언트로부터 데이터 읽기
     */
    valread = read(sock, buffer, 30000); // 클라이언트 요청 읽기
    printf("%s\n", buffer); // 요청 출력

    // 클라이언트에 응답 전송
    /**
     * @brief 클라이언트에 응답 전송
     */
    write(sock, hello, strlen(hello)); // 응답 메시지 전송
    printf("-------------Hello message sent---------------\n");

    // 소켓 닫기 및 메모리 해제
    /**
     * @brief 소켓 닫기 및 메모리 해제
     */
    close(sock); // 클라이언트 소켓 닫기
    free(socket_desc); // 할당된 메모리 해제

    return NULL;
}