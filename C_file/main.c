#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <process.h>
#include "info.h"

#define BUF_SIZE 1500
#define CLIENT_NUM 10

unsigned WINAPI HandleClient(void* arg);// 스레드 함수
void SendMsg(SOCKET curSocket, char* msg, int len);// 서버 >> 클라이언트 전송
void ErrorHandling(char* msg);

int clientCount = 0;
SOCKET clientSocks[CLIENT_NUM];
HANDLE hMutex;// 뮤텍스

// WINDOW 콘솔 창에서 Library_Server.exe (소켓 번호) 입력하면 실행된다.
// + 해당 파일 있는 위치로 이동해야 한다.
int main(int argc, char* argv[]) {
	WSADATA wasData;
	
	SOCKET serverSock, clientSock;
	SOCKADDR_IN serverAddr, clientAddr;
	int clientAddrSize;
	HANDLE hThread;
	
	if (argc != 2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	if (WSAStartup(MAKEWORD(2, 2), &wasData) != 0)// 윈도우 소켓 사용을 운영체제에 알림
		ErrorHandling("WSAStartup() error!");

	hMutex = CreateMutex(NULL, FALSE, NULL);// 뮤텍스 1개 생성
	serverSock = socket(PF_INET, SOCK_STREAM, 0);// 소켓 1개 생성

	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(atoi(argv[1]));

	if (bind(serverSock, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)// 생성한 소켓 배치
		ErrorHandling("bind() error");
	if (listen(serverSock, 5) == SOCKET_ERROR)// 소켓을 준비 상태로!
		ErrorHandling("listen() error");

	while (1) {
		clientAddrSize = sizeof(clientAddr);
		clientSock = accept(serverSock, (SOCKADDR*)&clientAddr, &clientAddrSize);// 서버에 전달된 클라이언트 소켓을 clinetSock에 전달
		WaitForSingleObject(hMutex, INFINITE);// 뮤텍스 실행
		clientSocks[clientCount++] = clientSock;// 클라이언트 소켓 배열에 방금 들어온 소켓 주소를 전달
		ReleaseMutex(hMutex);// 뮤텍스 중지
		hThread = (HANDLE)_beginthreadex(NULL, 0, HandleClient, (void*)&clientSock, 0, NULL);// HandleClient 스레드 실행 && clientSock을 매개변수로 제공
		printf("Connected Client IP: %s\n", inet_ntoa(clientAddr.sin_addr));
	}
	closesocket(serverSock);// 생성한 소켓 삭제
	WSACleanup();// 윈도우 소켓의 종료를 운영체제에게 알림

	return 0;
}

unsigned WINAPI HandleClient(void* arg) {
	SOCKET clientSock = *((SOCKET*)arg);// 매개변수로 받은 클라이언트 소켓 전달
	int strLen = 0, i;
	char msg[BUF_SIZE];

	while ((strLen = recv(clientSock, msg, sizeof(msg), 0)) != 0)// 클라이언트로부터 메시지 받기(그때까지 대기)
		SendMsg(clientSock, msg, strLen);// SendMsg에 받은 메시지 전달

	// 클라이언트가 나간 경우
	WaitForSingleObject(hMutex, INFINITE);// 뮤텍스 실행
	for (i = 0; i < clientCount; i++) {// 배열의 모든 항목에 대해
		if (clientSock == clientSocks[i]) {// clientSock이 배열의 현재 항목과 같다면
			while (i++ < clientCount - 1)// 뒤의 클라이언트들을 앞으로 당긴다.
				clientSocks[i] = clientSocks[i + 1];
			break;
		}
	}
	clientCount--;// 클라이언트의 개수를 감소
	ReleaseMutex(hMutex);// 뮤텍스 중지
	closesocket(clientSock);// 소켓 종료
	return 0;
}

char* makeProtocol(char* msg) {
	FILE** info = retLibrary();// 도서관 파일
	Info finalResult[10];// 출력 대상

	char* toClient = (char*)malloc(sizeof(char) * 5000);// 클라이언트에게 전달되는 메시지 버퍼

	char* protocol = strtok(msg, "//");
	char* location = strtok(NULL, "//");
	char* information = strtok(NULL, "//");

	int check = 0;

	if (strtol(protocol, NULL, 10) == 100) check = findBook(info, 0, information, 0, finalResult, strtol(location, NULL, 10));
	else if (strtol(location, NULL, 10) == 101) check = findBook(info, 1, information, 0, finalResult, strtol(location, NULL, 10));
	else check = findBook(info, 2, NULL, strtol(information, NULL, 10), finalResult, strtol(location, NULL, 10));

	char int2str[500] = { 0 };

	if (check == 10) strcpy(toClient, "100");
	else if (check == 11) {
		int printNum = sizeof(finalResult) / sizeof(Info);
		strcpy(toClient, "101");
		for (int i = 0; i < printNum; i++) {
			strcat(toClient, "//");
			strcat(toClient, finalResult[i].bookTitle);
			strcat(toClient, "//");
			strcat(toClient, finalResult[i].bookAuthor);
			strcat(toClient, "//");
			sprintf(int2str, "%d", finalResult[i].bookID);
			strcat(toClient, int2str);
			strcat(toClient, "//");
			sprintf(int2str, "%d", finalResult[i].bookQuantity);
			strcat(toClient, int2str);
		}
	}
	else {
		strcpy(toClient, "102");
		sprintf(int2str, "%d", check);
		strcat(toClient, "//");
		strcat(toClient, int2str);
	}

	return toClient;
}

void SendMsg(SOCKET curSocket, char* msg, int len) {
	WaitForSingleObject(hMutex, INFINITE);// 뮤텍스 실행
	char* toClient = makeProtocol(msg);
	send(curSocket, toClient, strlen(toClient), 0);// 클라이언트에게 전달
	free(toClient);
	ReleaseMutex(hMutex);// 뮤텍스 중지
}

void ErrorHandling(char* msg) {
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}