#include <stdio.h> // 표준 입출력 라이브러리
#include <stdlib.h> // 표준 라이브러리
#include <winsock2.h> // 소켓 라이브러리
#include <string.h> // 문자열 라이브러리
#include <pthread.h> // 스레드 라이브러리
 
#define BUFFER_SIZE 1024 // 버퍼 사이즈는 최대 1024로

void *ReceiveMessages(void *socketDesc) { // 서버로부터 메시지를 받는 함수
    int sock = *(int*)socketDesc; // 소켓을 받아옴
    char buffer[BUFFER_SIZE]; // 버퍼 생성
    int readSize; // 읽은 사이즈를 저장할 변수

    while ((readSize = recv(sock, buffer, BUFFER_SIZE - 1, 0)) > 0) { // 서버로부터 메시지를 받음
        buffer[readSize] = '\0'; // 버퍼의 끝을 널로 설정
        printf("%s", buffer); // 버퍼를 출력
    }

    if (readSize == 0) { // 서버가 종료되었을 경우
        printf("Server disconnected.\n"); // 서버가 종료되었다고 출력
    } else if (readSize == -1) { // 서버와의 통신에 에러가 발생했을 경우
        perror("recv failed"); // 에러 메시지 출력
    }

    return NULL; // NULL 반환
}

int main(int argc, char *argv[]) {
    WSADATA wsaData; // 윈도우 소켓 데이터
    SOCKET sock; // 소켓 변수
    struct sockaddr_in server; // 서버 주소 구조체
    char message[BUFFER_SIZE], serverReply[BUFFER_SIZE]; // 메시지를 저장할 변수
    char addr[20]; // 서버 주소를 저장할 변수
    int port_number; // 서버 포트 번호를 저장할 변수
	int readSize; // 읽은 사이즈를 저장할 변수
    pthread_t receiveThread; // 서버로부터 메시지를 받을 스레드

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) < 0) { //윈도우 소켓을 초기화
        printf("Failed to initialize Winsock. Error Code : %d", WSAGetLastError()); // 초기화에 실패할 경우, 에러 처리
        return 1;
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) { // 소켓 생성
        printf("Could not create socket : %d", WSAGetLastError()); // 소켓 생성에 실패할 경우, 에러 처리
        return 1; 
    }
	
	printf("Enter server addres and port number : "); // 서버 주소와 포트 번호를 입력받음
	scanf("%s %d", addr, &port_number); 
	
    server.sin_addr.s_addr = inet_addr(addr); // 서버 주소 설정
    server.sin_family = AF_INET; // IPv4 주소 체계 사용
    server.sin_port = htons(port_number); // 포트 번호 설정

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) { // IP주소와 포트 번호로 서버에 연결
        perror("connect failed. Error"); // 연결에 실패할 경우, 에러 처리
        return 1;
    }

    printf("Connected to server.\n"); // 서버에 연결되었다고 출력

    if (pthread_create(&receiveThread, NULL, ReceiveMessages, (void*)&sock) < 0) { // 서버로부터 메시지를 받을 스레드 생성
        perror("Could not create thread"); // 스레드 생성에 실패할 경우, 에러 처리
        return 1;
    }


	while (1) { //메시지를 입려받아 서버로 전송. 무한루프로 실행시킨다.
    	fgets(message, BUFFER_SIZE, stdin); // 표준 입력으로부터 메시지를 입력받음
    

    	size_t len = strlen(message); // 메시지의 길이를 저장
    	if (len > 0 && message[len - 1] == '\n') { // 메시지의 끝이 개행 문자일 경우
        	message[len - 1] = '\0'; // 개행 문자를 널로 변경 => 메시지의 끝을 설정
    	}

    	if (send(sock, message, strlen(message), 0) < 0) { // 서버로 메시지를 전송
        	puts("Send failed"); // 전송에 실패할 경우, 에러 처리
        	return 1;
    	}
	}

    closesocket(sock); // 소켓을 닫음
    WSACleanup(); // 윈도우 소켓을 종료
    return 0;
}

