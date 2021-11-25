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

FILE** retLibrary();// 도서관들의 정보를 가진 파일 포인터를 반환한다.

Info* findByTitle(FILE**, char*, int);// 파라미터: 도서관 정보, title, 지점
Info* findByAuthor(FILE**, char*, int);// 파라미터: 도서관 정보, author, 지점
Info* findByID(FILE**, int, int);// 파라미터: 도서관 정보, ID, 지점

int findBook(FILE**, int, char*, int, Info*, int);
// flag, title/author, ID, 지점, 책 정보 반환용
// flag: 0 >> title 검색, flag: 1 >> author 검색, 2 >> ID 검색
// flag가 0, 1이면 3번째 파라미터를 0으로 설정한다.
// flag가 2면 2번째 파라미터를 null로 설정한다.
// 만약 책이 없으면, 10을 반환 && Info*는 null 설정
// 만약 현재 도서관에 책이 있으면, 11을 반환 && Info*에 들어간 인자에 정보 저장하기
// 만약 다른 도서관에 책이 있으면, 해당 도서관 지점 반환 && Info*에 null 설정
// + Floyd-Warshall 알고리즘 채용

#endif