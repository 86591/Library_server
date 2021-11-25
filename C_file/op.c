#define _CRT_SECURE_NO_WARNINGS

#include <string.h>
#include <stdlib.h>
#include "info.h"

#pragma comment(lib, "ws2_32.lib")

int Graph[LIBRARYNUM][LIBRARYNUM] = {
	{ 0, 1000000000, 5, 3 },
	{ 1000000000, 0, 1, 4 },
	{ 5, 1, 0, 1 },
	{ 3, 4, 1, 0 }
};

int result[LIBRARYNUM][LIBRARYNUM] = { 0 };

FILE** retLibrary() {
	static FILE* ret[LIBRARYNUM];
	ret[0] = fopen("Library_1.txt", "r");
	ret[1] = fopen("Library_2.txt", "r");
	ret[2] = fopen("Library_3.txt", "r");
	ret[3] = fopen("Library_4.txt", "r");
	return ret;
}

Info* findByTitle(FILE** info, char* title, int location) {// 도서관 정보, title, 지점
	FILE* target = info[location];
	Info* ret = (Info*)malloc(sizeof(Info) * 10);
	int retNum = 0, isRet = 0;
	char buf[1000] = { 0 };

	Info temp;

	while (fgets(buf, 900, target) != NULL) {
		char* ptr = strtok(buf, " ");
		strcpy(temp.bookTitle, ptr);
		
		int check = 0;
		while (ptr == NULL) {
			ptr = strtok(NULL, " ");
			
			if (check == 0) strcpy(temp.bookAuthor, ptr);
			else if (check == 1) temp.bookID = strtol(ptr, NULL, 10);
			else temp.bookQuantity = strtol(ptr, NULL, 10);

			check++;
		}

		int searchLen = strlen(temp.bookTitle);
		if (strncmp(temp.bookTitle, title, searchLen) == 0 && temp.bookQuantity > 0) {
			ret[retNum++] = temp;
			isRet++;
		}
	}

	return isRet == 0 ? NULL : ret;
}

Info* findByAuthor(FILE** info, char* author, int location) {// 도서관 정보, author, 지점
	FILE* target = info[location];
	Info* ret = (Info*)malloc(sizeof(Info) * 10);
	int retNum = 0, isRet = 0;
	char buf[1000] = { 0 };

	Info temp;

	while (fgets(buf, 900, target) != NULL) {
		char* ptr = strtok(buf, " ");
		strcpy(temp.bookTitle, ptr);

		int check = 0;
		while (ptr == NULL) {
			ptr = strtok(NULL, " ");
			
			if (check == 0) strcpy(temp.bookAuthor, ptr);
			else if (check == 1) temp.bookID = strtol(ptr, NULL, 10);
			else temp.bookQuantity = strtol(ptr, NULL, 10);

			check++;
		}

		int searchLen = strlen(temp.bookAuthor);
		if (strncmp(temp.bookAuthor, author, searchLen) == 0 && temp.bookQuantity > 0) {
			ret[retNum++] = temp;
			isRet++;
		}
	}

	return isRet == 0 ? NULL : ret;
}

Info* findByID(FILE** info, int ID, int location) {// 도서관 정보, ID, 지점
	FILE* target = info[location];
	Info* ret = (Info*)malloc(sizeof(Info) * 10);
	int retNum = 0, isRet = 0;
	char buf[1000] = { 0 };

	Info temp;

	while (fgets(buf, 900, target) != NULL) {
		char* ptr = strtok(buf, " ");
		strcpy(temp.bookTitle, ptr);

		int check = 0;
		while (ptr == NULL) {
			ptr = strtok(NULL, " ");

			if (check == 0) strcpy(temp.bookAuthor, ptr);
			else if (check == 1) temp.bookID = strtol(ptr, NULL, 10);
			else temp.bookQuantity = strtol(ptr, NULL, 10);

			check++;
		}

		if (temp.bookID == ID && temp.bookQuantity > 0) {
			ret[retNum++] = temp;
			isRet++;
		}
	}

	return isRet == 0 ? NULL : ret;
}

int findBook(FILE** info, int flag, char* title_author, int ID, Info* retVal, int location) {
	for (int i = 0; i < LIBRARYNUM; i++)
		for (int j = 0; j < LIBRARYNUM; j++)
			result[i][j] = Graph[i][j];

	for (int k = 0; k < LIBRARYNUM; k++)
		for (int i = 0; i < LIBRARYNUM; i++)
			for (int j = 0; j < LIBRARYNUM; j++)
				if (result[i][j] > result[i][k] + result[k][j])
					result[i][j] = result[i][k] + result[k][j];

	typedef struct idx_distance {
		int idx;
		int distance;
	} idx_d;

	idx_d distanceOrder[LIBRARYNUM - 1] = {{ 0, 0 }};
	idx_d temp = { 0, 0 };

	for (int i = 0; i < LIBRARYNUM; i++)
		if (i != location) {
			distanceOrder[i].idx = i;
			distanceOrder[i].distance = result[location][i];
		}

	for (int i = 0; i < LIBRARYNUM - 1; i++)
		for (int k = 0; k < LIBRARYNUM - i - 2; k++)
			if (distanceOrder[k].distance > distanceOrder[k + 1].distance) {
				temp = distanceOrder[k];
				distanceOrder[k] = distanceOrder[k + 1];
				distanceOrder[k + 1] = temp;
			}

	Info* getRet = NULL;
	if (flag == 0) {
		for (int i = -1; i < LIBRARYNUM - 1; i++) {
			if (i == -1) {
				getRet = findByTitle(info, title_author, location);
				if (getRet != NULL) {
					retVal = getRet;
					return 11;
				}
			}
			else {
				getRet = findByTitle(info, title_author, distanceOrder[i].idx);
				if (getRet != NULL) {
					retVal = NULL;
					return distanceOrder[i].idx;
				}
			}
		}

		retVal = NULL;
		return 10;
	}
	else if (flag == 1) {
		for (int i = -1; i < LIBRARYNUM - 1; i++) {
			if (i == -1) {
				getRet = findByAuthor(info, title_author, location);
				if (getRet != NULL) {
					retVal = getRet;
					return 11;
				}
			}
			else {
				getRet = findByAuthor(info, title_author, distanceOrder[i].idx);
				if (getRet != NULL) {
					retVal = NULL;
					return distanceOrder[i].idx;
				}
			}
		}

		retVal = NULL;
		return 10;
	}
	else {
		for (int i = -1; i < LIBRARYNUM - 1; i++) {
			if (i == -1) {
				getRet = findByID(info, ID, location);
				if (getRet != NULL) {
					retVal = getRet;
					return 11;
				}
			}
			else {
				getRet = findByID(info, ID, distanceOrder[i].idx);
				if (getRet != NULL) {
					retVal = NULL;
					return distanceOrder[i].idx;
				}
			}
		}

		retVal = NULL;
		return 10;
	}
}
// flag, title/author, ID, 지점, 책 정보 반환용
// flag: 0 >> title 검색, flag: 1 >> author 검색, 2 >> ID 검색
// flag가 0, 1이면 3번째 파라미터를 0으로 설정한다.
// flag가 2면 2번째 파라미터를 null로 설정한다.
// 만약 책이 없으면, 10을 반환 && Info*는 null 설정
// 만약 현재 도서관에 책이 있으면, 11을 반환 && Info*에 들어간 인자에 정보 저장하기
// 만약 다른 도서관에 책이 있으면, 해당 도서관 지점 반환 && Info*에 null 설정
// + Floyd-Warshall 알고리즘 채용