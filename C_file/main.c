#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <process.h>
#include "info.h"

#define BUF_SIZE 1500
#define CLIENT_NUM 10

unsigned WINAPI HandleClient(void* arg);// ������ �Լ�
void SendMsg(SOCKET curSocket, char* msg, int len);// ���� >> Ŭ���̾�Ʈ ����
void ErrorHandling(char* msg);

int clientCount = 0;
SOCKET clientSocks[CLIENT_NUM];
HANDLE hMutex;// ���ؽ�

// WINDOW �ܼ� â���� Library_Server.exe (���� ��ȣ) �Է��ϸ� ����ȴ�.
// + �ش� ���� �ִ� ��ġ�� �̵��ؾ� �Ѵ�.
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

	if (WSAStartup(MAKEWORD(2, 2), &wasData) != 0)// ������ ���� ����� �ü���� �˸�
		ErrorHandling("WSAStartup() error!");

	hMutex = CreateMutex(NULL, FALSE, NULL);// ���ؽ� 1�� ����
	serverSock = socket(PF_INET, SOCK_STREAM, 0);// ���� 1�� ����

	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(atoi(argv[1]));

	if (bind(serverSock, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)// ������ ���� ��ġ
		ErrorHandling("bind() error");
	if (listen(serverSock, 5) == SOCKET_ERROR)// ������ �غ� ���·�!
		ErrorHandling("listen() error");

	while (1) {
		clientAddrSize = sizeof(clientAddr);
		clientSock = accept(serverSock, (SOCKADDR*)&clientAddr, &clientAddrSize);// ������ ���޵� Ŭ���̾�Ʈ ������ clinetSock�� ����
		WaitForSingleObject(hMutex, INFINITE);// ���ؽ� ����
		clientSocks[clientCount++] = clientSock;// Ŭ���̾�Ʈ ���� �迭�� ��� ���� ���� �ּҸ� ����
		ReleaseMutex(hMutex);// ���ؽ� ����
		hThread = (HANDLE)_beginthreadex(NULL, 0, HandleClient, (void*)&clientSock, 0, NULL);// HandleClient ������ ���� && clientSock�� �Ű������� ����
		printf("Connected Client IP: %s\n", inet_ntoa(clientAddr.sin_addr));
	}
	closesocket(serverSock);// ������ ���� ����
	WSACleanup();// ������ ������ ���Ḧ �ü������ �˸�

	return 0;
}

unsigned WINAPI HandleClient(void* arg) {
	SOCKET clientSock = *((SOCKET*)arg);// �Ű������� ���� Ŭ���̾�Ʈ ���� ����
	int strLen = 0, i;
	char msg[BUF_SIZE];

	while ((strLen = recv(clientSock, msg, sizeof(msg), 0)) != 0)// Ŭ���̾�Ʈ�κ��� �޽��� �ޱ�(�׶����� ���)
		SendMsg(clientSock, msg, strLen);// SendMsg�� ���� �޽��� ����

	// Ŭ���̾�Ʈ�� ���� ���
	WaitForSingleObject(hMutex, INFINITE);// ���ؽ� ����
	for (i = 0; i < clientCount; i++) {// �迭�� ��� �׸� ����
		if (clientSock == clientSocks[i]) {// clientSock�� �迭�� ���� �׸�� ���ٸ�
			while (i++ < clientCount - 1)// ���� Ŭ���̾�Ʈ���� ������ ����.
				clientSocks[i] = clientSocks[i + 1];
			break;
		}
	}
	clientCount--;// Ŭ���̾�Ʈ�� ������ ����
	ReleaseMutex(hMutex);// ���ؽ� ����
	closesocket(clientSock);// ���� ����
	return 0;
}

char* makeProtocol(char* msg) {
	FILE** info = retLibrary();// ������ ����
	Info finalResult[10];// ��� ���

	char* toClient = (char*)malloc(sizeof(char) * 5000);// Ŭ���̾�Ʈ���� ���޵Ǵ� �޽��� ����

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
	WaitForSingleObject(hMutex, INFINITE);// ���ؽ� ����
	char* toClient = makeProtocol(msg);
	send(curSocket, toClient, strlen(toClient), 0);// Ŭ���̾�Ʈ���� ����
	free(toClient);
	ReleaseMutex(hMutex);// ���ؽ� ����
}

void ErrorHandling(char* msg) {
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}