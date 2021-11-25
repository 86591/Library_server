# Library_server

기본적인 정보

도서관은 총 4개이다. Library_1 ~ 4

프로그램에서 한 번에 접속할 수 있는 클라이언트의 수는 10이다.

각 도서관 사이의 cost는 그림으로 제공한다. (카카오톡으로 보낼 예정)

cost는 일단 양수로 지정한다. (음수인 것도 좀 이상..)

클라이언트 >> 서버 전송 정보는 최대 1000byte 이다.

서버 >> 클라이언트 전송 정보는 최대 5000byte 이다. (이보다 크면 좋겠지만, 그렇게 책이 많지는 않을 것이니...)

ID는 int 타입이다.

도서관 지점도 int 타입이다.

===================================================
Protocol

서버 >> 클라이언트
도서관 지점: 0, 1, 2, 3
(Library_1: 0, Library_2: 1, Library_3: 2, LIbrary_4: 3)

모든 도서관을 통틀어서 책이 없는 경우: 100

이 도서관에 있는 경우: 101//(책 1의 이름)//(책 1의 저자 이름)//(책 1의 ID)//(책 1의 수량)//(책 2의 이름) ...
+ 책이 여러개면 위의 순서로 계속 나간다.
ex) 101//book1//author1//1//4//book2//author2//2//45

다른 도서관에 있는 경우: 102//(책이 있는 도서관 지점)
ex) 102//1

===================================================
클라이언트 >> 서버
도서관 지점: 0, 1, 2, 3
(Library_1: 0, Library_2: 1, Library_3: 2, LIbrary_4: 3)

(유일한 프로토콜..)
책에 대한 정보 요청(title 사용): 100//(도서관 지점)//(책 이름 >> 부분 이름이어도 된다.)
ex) 100//0//Book1

책에 대한 정보 요청(저자 사용): 101//(도서관 지점)//(저자 이름 >> 부분 이름이어도 된다.)
ex) 101//3//Book2

책에 대한 정보 요청(ID 사용): 102//(도서관 지점)//(ID >> 완전히 일치해야만 한다.)
ex) 102//2//Book69

+ 종료는 따로 프로토콜 전송할 필요 없다. 그냥 클라이언트가 TCP연결 끊으면 된다.
