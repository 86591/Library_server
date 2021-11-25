#ifndef __INFO_H__
#define __INFO_H__
#define _CRT_SECURE_NO_WARNINGS
#define LIBRARYNUM 4

#include <stdio.h>

typedef struct Information {
	char bookTitle[100];
	char bookAuthor[100];
	int bookID;
	int bookQuantity;
} Info;

FILE** retLibrary();// ���������� ������ ���� ���� �����͸� ��ȯ�Ѵ�.

Info* findByTitle(FILE**, char*, int);// �Ķ����: ������ ����, title, ����
Info* findByAuthor(FILE**, char*, int);// �Ķ����: ������ ����, author, ����
Info* findByID(FILE**, int, int);// �Ķ����: ������ ����, ID, ����

int findBook(FILE**, int, char*, int, Info*, int);
// flag, title/author, ID, ����, å ���� ��ȯ��
// flag: 0 >> title �˻�, flag: 1 >> author �˻�, 2 >> ID �˻�
// flag�� 0, 1�̸� 3��° �Ķ���͸� 0���� �����Ѵ�.
// flag�� 2�� 2��° �Ķ���͸� null�� �����Ѵ�.
// ���� å�� ������, 10�� ��ȯ && Info*�� null ����
// ���� ���� �������� å�� ������, 11�� ��ȯ && Info*�� �� ���ڿ� ���� �����ϱ�
// ���� �ٸ� �������� å�� ������, �ش� ������ ���� ��ȯ && Info*�� null ����
// + Floyd-Warshall �˰��� ä��

#endif