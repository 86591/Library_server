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

Info* findByTitle(FILE** info, char* title, int location) {// ������ ����, title, ����
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

Info* findByAuthor(FILE** info, char* author, int location) {// ������ ����, author, ����
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

Info* findByID(FILE** info, int ID, int location) {// ������ ����, ID, ����
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
// flag, title/author, ID, ����, å ���� ��ȯ��
// flag: 0 >> title �˻�, flag: 1 >> author �˻�, 2 >> ID �˻�
// flag�� 0, 1�̸� 3��° �Ķ���͸� 0���� �����Ѵ�.
// flag�� 2�� 2��° �Ķ���͸� null�� �����Ѵ�.
// ���� å�� ������, 10�� ��ȯ && Info*�� null ����
// ���� ���� �������� å�� ������, 11�� ��ȯ && Info*�� �� ���ڿ� ���� �����ϱ�
// ���� �ٸ� �������� å�� ������, �ش� ������ ���� ��ȯ && Info*�� null ����
// + Floyd-Warshall �˰��� ä��