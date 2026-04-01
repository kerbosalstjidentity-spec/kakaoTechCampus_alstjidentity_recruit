#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>
#include <errno.h>    // errno를 위해 이 헤더 포함
#include <sys/wait.h> // waitpid를 위해 이 헤더 포함

#define PORT 8090 // 포트 번호 정의

/**
 * 클라이언트 요청을 처리하는 함수
 * @param new_socket 클라이언트와의 통신을 위한 소켓
 */
void handle_client(int new_socket) {
    long valread;
    char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain"
                  "Content-Length: 20\n\nMy first web server!";
    char buffer[30000] = {0};

    // 클라이언트로부터 데이터 읽기
    valread = read(new_socket, buffer, 30000);
    printf("%s\n", buffer);

    // 클라이언트에게 응답 보내기
    write(new_socket, hello, strlen(hello));
    printf("-------------Hello message sent---------------\n");

    // 소켓 닫기
    close(new_socket);
}

/**
 * 자식 프로세스 종료 시그널을 처리하는 함수
 * @param s 시그널 번호
 */
void sigchld_handler(int s) {
    // waitpid()가 errno를 덮어쓸 수 있으므로 이를 저장하고 복원함
    int saved_errno = errno;
    while(waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
}

int main(int argc, char const *argv[]) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    struct sigaction sa;

    // 좀비 프로세스를 방지하기 위해 모든 종료된 프로세스를 수거함
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    // 소켓 생성
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("In socket");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET; // IPv4 사용
    address.sin_addr.s_addr = INADDR_ANY; // 모든 인터페이스에서 수신
    address.sin_port = htons(PORT); // 포트 설정

    memset(address.sin_zero, '\0', sizeof address.sin_zero);

    // 소켓에 주소 바인딩
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("In bind");
        exit(EXIT_FAILURE);
    }

    // 연결 대기
    if (listen(server_fd, 10) < 0) {
        perror("In listen");
        exit(EXIT_FAILURE);
    }

    while (1) {
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");

        // 새 연결 수락
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("In accept");
            exit(EXIT_FAILURE);
        }

        if (!fork()) { // 자식 프로세스 생성
            close(server_fd); // 자식 프로세스는 리스너가 필요 없음
            handle_client(new_socket); // 클라이언트 요청 처리
            exit(0); // 자식 프로세스 종료
        }

        close(new_socket); // 부모 프로세스는 이 소켓이 필요 없음
    }

    return 0;
}
