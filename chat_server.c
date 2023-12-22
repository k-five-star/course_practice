#include <stdio.h> // 표준 입출력 라이브러리
#include <stdlib.h> // 표준 라이브러리
#include <winsock2.h> // 소켓 라이브러리
#include <string.h> // 문자열 라이브러리
#include <pthread.h> // 스레드 라이브러리

#define MAX_CLIENTS 4 // 한 번에 접속할 수 있는 최대 클라이언트 수
#define BUFFER_SIZE 1024 // 버퍼 사이즈는 최대 1024로
#define NICKNAME_LEN 32 // 닉네임의 최대 길이는 32로

int clientSockets[MAX_CLIENTS]; // 클라이언트 소켓을 저장할 배열
int nClients = 0; // 현재 접속한 클라이언트 수
pthread_mutex_t clientsMutex = PTHREAD_MUTEX_INITIALIZER; // 클라이언트 배열을 위한 뮤텍스

void BroadcastMessage(char *message, int sender) { // 모든 클라이언트에게 메시지를 보내는 함수
    pthread_mutex_lock(&clientsMutex); // 뮤텍스를 잠금(세마포와 비슷한 기능으로써, 해당 뮤텍스를 잠그고 있는 스레드가 있으면 다른 스레드는 해당 뮤텍스를 사용할 수 없음)
    int i; 
    for (i = 0; i < nClients; i++) { // 메시지를 보낸 클라이언트를 제외하고 모든 클라이언트에게 메시지를 보냄
        if (clientSockets[i] != sender) { 
            send(clientSockets[i], message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&clientsMutex); // 뮤텍스를 잠금 해제
}

void *ClientHandler(void *socketDesc) { // 클라이언트를 처리하는 함수
    int sock = *(int*)socketDesc; // 소켓을 받아옴
    char nickname[NICKNAME_LEN]; // 닉네임을 저장할 변수
    char buffer[BUFFER_SIZE]; // 버퍼 생성
    int readSize; // 읽은 사이즈를 저장할 변수
    int nicknameSet = 0; // 닉네임이 설정되었는지 확인하는 변수
    
    char initialMsg[] = "First, You have to set your nickname : ";
    send(sock, initialMsg, strlen(initialMsg), 0);


    while ((readSize = recv(sock, buffer, BUFFER_SIZE - 1, 0)) > 0) { //무한루프로 클라이언트로부터 메시지를 받음
        buffer[readSize] = '\0'; // 버퍼의 끝을 널로 설정
        
        if (!nicknameSet) { // 닉네임이 설정되지 않았을 경우
            strncpy(nickname, buffer, NICKNAME_LEN); // 닉네임을 설정
            nickname[NICKNAME_LEN - 1] = '\0'; // 닉네임의 끝을 널로 설정
            nicknameSet = 1; // 닉네임이 설정되었다고 표시
            
            char confirmMsg[] = "Accepted. Start conversation!\n"; // 닉네임 설정이 완료되었다고 클라이언트에게 알림
            send(sock, confirmMsg, strlen(confirmMsg), 0); 

            char welcomeMsg[BUFFER_SIZE]; // 클라이언트가 접속했다고 다른 클라이언트에게 알림
            sprintf(welcomeMsg, "%s joined the chat.\n", nickname); //문자열 생성
            printf("%s", welcomeMsg); // 문자열 출력
            BroadcastMessage(welcomeMsg, sock); // 모든 클라이언트에게 해당 환영 메시지를 보냄
        } 
		else {
            // Broadcast message
            char msgToSend[BUFFER_SIZE + NICKNAME_LEN]; // 메시지를 보낼 변수
            sprintf(msgToSend, "%s: %s\n", nickname, buffer); // 메시지를 생성
            printf("%s", msgToSend); // 메시지를 출력
            BroadcastMessage(msgToSend, sock); // 모든 클라이언트에게 해당 메시지를 보냄
        }
    }

    if (readSize == 0) { // 클라이언트가 종료되었을 경우
        printf("%s disconnected.\n", nickname); // 클라이언트가 종료되었다고 출력

    } 
    else if (readSize == -1) { // 클라이언트와의 통신에 에러가 발생했을 경우
        perror("recv failed"); // 에러 메시지 출력
    }

    closesocket(sock); // 소켓을 닫음
    return NULL; 
}


int main() {
    WSADATA wsaData; // 윈도우 소켓 데이터
    SOCKET serverSocket, newSocket; // 서버 소켓과 클라이언트 소켓
    struct sockaddr_in server, client; // 서버와 클라이언트 주소 구조체
    int c, *newSock; // 클라이언트 소켓을 저장할 변수
    pthread_t threadId; // 클라이언트를 처리할 스레드

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) < 0) { // 윈도우 소켓을 초기화 
        printf("Error initializing Winsock: %d\n", WSAGetLastError()); // 초기화에 실패할 경우, 에러 처리
        return 1;
    }

    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) { // 소켓 생성
        printf("Could not create socket: %d\n", WSAGetLastError()); // 소켓 생성에 실패할 경우, 에러 처리
        return 1;
    }

    server.sin_family = AF_INET; // IPv4 주소 체계 사용 
    server.sin_addr.s_addr = INADDR_ANY; // 모든 IP 주소로부터의 접속을 허용
    server.sin_port = htons(5000); // 포트 번호 5000을 사용

    if (bind(serverSocket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) { // 서버 소켓에 주소를 할당
        printf("Bind failed: %d\n", WSAGetLastError()); // 주소 할당에 실패할 경우, 에러 처리
        return 1;
    }

    listen(serverSocket, 3); // 클라이언트의 접속을 기다림

    printf("Server started. Waiting for client connections...\n");
    c = sizeof(struct sockaddr_in); // 클라이언트 주소 구조체의 크기를 저장할 변수

    while ((newSocket = accept(serverSocket, (struct sockaddr *)&client, &c)) != INVALID_SOCKET) { // 클라이언트의 접속을 기다림
        printf("Connection accepted.\n"); // 클라이언트가 접속되었다고 출력

        pthread_mutex_lock(&clientsMutex); // 뮤텍스를 잠금. 세마포와 비슷한 기능으로, 해당 뮤텍스를 잠그고 있는 스레드가 있으면 다른 스레드는 해당 뮤텍스를 사용할 수 없음
        if (nClients < MAX_CLIENTS) { // 최대 접속 가능한 클라이언트 수보다 현재 접속한 클라이언트 수가 작을 경우
            clientSockets[nClients++] = newSocket; // 클라이언트 소켓을 배열에 저장
            newSock = malloc(sizeof(int)); // 클라이언트 소켓을 저장할 변수를 동적 할당
            *newSock = newSocket; // 클라이언트 소켓을 저장
            if (pthread_create(&threadId, NULL, ClientHandler, (void*)newSock) < 0) { // 클라이언트를 처리할 스레드 생성
                perror("Could not create thread"); // 스레드 생성에 실패할 경우, 에러 처리
                return 1;
            }
        } 
        else { // 최대 접속 가능한 클라이언트 수보다 현재 접속한 클라이언트 수가 클 경우
            printf("Max clients reached. Connection rejected.\n"); // 클라이언트가 접속되지 않았다고 출력
            send(newSocket, "Server full.\n", 13, 0); // 클라이언트에게 서버가 가득 찼다고 알림
            closesocket(newSocket); // 클라이언트 소켓을 닫음
        }
        pthread_mutex_unlock(&clientsMutex); // 뮤텍스를 잠금 해제
    }

    if (newSocket == INVALID_SOCKET) { // 클라이언트와의 접속에 에러가 발생했을 경우
        printf("Accept failed: %d\n", WSAGetLastError()); // 에러 메시지 출력
        return 1;
    }

    closesocket(serverSocket); // 서버 소켓을 닫음
    WSACleanup(); // 윈도우 소켓을 종료
    return 0;
}

