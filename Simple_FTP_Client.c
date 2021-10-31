#include < stdio.h > #include < sys / socket.h > #include < arpa / inet.h > #include < netdb.h > #include < string.h > #include < stdlib.h > #include < strings.h > #include < unistd.h > #define MAXBUF 1024 // 버퍼 최대 사이즈
#define PORT 21 // 포트 번호 21로 define
#define IP "223.194.7.95" // ssl.kw.ac.kr 웹사이트 IP주소
int main(int argc, char * argv[]) {
    int returnStatus = 0;
    int Socket1 = 0;
    struct sockaddr_in xferClient;
    char user[20] = "USER elec\n"; // user ID
    char pass[20] = "PASS comm\n"; // user password
    char retr[20] = "RETR welcome.txt\n"; // 파일 다운로드 하기 위한 명령어(RETR 파일명)
    char buf[MAXBUF] = {
        0
    };
    char buffer[MAXBUF] = {
        0
    };
    char welcome[MAXBUF] = {
        0
    };
    FILE * fp;
    // 파일 다운로드를 위한 파일포인터 선언
    /*소켓 생성(명령어 전달용)*/
    Socket1 = socket(AF_INET, SOCK_STREAM, 0);
    if (Socket1 == 1) {
        fprintf(stderr, "Could not create a socket!\n");
        exit(1);
    } else {
        fprintf(stderr, "Socket created!\n");
    }
    bzero(& xferClient, sizeof(xferClient)); // bzero를 통해 xferClinet 0으로 초기화
    xferClient.sin_family = AF_INET; // IPv4인터넷 프로토콜
    xferClient.sin_addr.s_addr = inet_addr(IP); // define 해둔 IP주소 형식을 빅엔디안 32비트 값
    으로 변환
    xferClient.sin_port = htons(21); // Host byte => Network byte
    returnStatus = connect(Socket1, (struct sockaddr *) & xferClient, sizeof(xferClient)); //
    if (returnStatus == 0) {
        fprintf(stderr, "Connect successful!\n");
    } else {
        fprintf(stderr, "Could not connect to address!\n");
        close(Socket1);
        exit(1); // 연결 실패시 종료
    }
    recv(Socket1, buf, MAXBUF, 0); // 소켓으로부터 데이터 받음
    printf("%s", buf);
    // 수신받은 자료 출력
    /*USER 명령어 전달*/
    memset(buf, 0, MAXBUF); // 버퍼 초기화
    returnStatus = send(Socket1, user, strlen(user), 0); // 소켓으로 USER ID 명령어 전송
    if (returnStatus > 0) {
        printf("%s", user);
    } else {
        fprintf(stderr, "%d\n", returnStatus);
    }
    recv(Socket1, buf, MAXBUF, 0); // 소켓으로부터 데이터 받음
    printf("%s", buf);
    // 수신받은 자료 출력
    /*PASS 명령어 전달
 위의 USER부분과 구조 동일*/
    memset(buf, 0, MAXBUF);
    returnStatus = send(Socket1, pass, strlen(pass), 0);
    if (returnStatus > 0) {
        printf("%s", pass);
    } else {
        fprintf(stderr, "%d\n", returnStatus);
    }
    recv(Socket1, buf, MAXBUF, 0);
    printf("%s", buf);
    /*PASV 명령어 전달*/
    memset(buf, 0, MAXBUF);
    returnStatus = send(Socket1, "PASV\n", strlen("PASV\n"), 0);
    if (returnStatus > 0) {
        printf("PASV\n");
    } else {
        fprintf(stderr, "%d \n", returnStatus);
    }
    recv(Socket1, buf, MAXBUF, 0);
    printf("%s", buf);
    /*string token 이용해 passive listen port 계산을 위한 정보만 추출*/
    int i = 0;
    char * port_number[7];
    char * temp = strtok(buf, "(,)");
    while (temp != NULL) {
        port_number[i] = temp;
        temp = strtok(NULL, "(,)");
        i += 1;
    }
    int listen_port_num = atoi(port_number[5]) * 256 + atoi(port_number[6]); // 포트 번호 계
    산
    int Socket2 = 0;
    /*파일 전달받기 위한 소켓 생성*/
    Socket2 = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in subClient;
    bzero(& subClient, sizeof(subClient));
    subClient.sin_family = AF_INET;
    subClient.sin_addr.s_addr = inet_addr(IP);
    subClient.sin_port = htons(listen_port_num);
    returnStatus = connect(Socket2, (struct sockaddr *) & subClient, sizeof(subClient));
    /*RETR 명령어 전달*/
    memset(buf, 0, MAXBUF);
    returnStatus = send(Socket1, retr, strlen(retr), 0); // RETR welcome.txt 전달
    if (returnStatus > 0) {
        printf("%s", retr);
    } else {
        fprintf(stderr, "%d\n", returnStatus);
    }
    recv(Socket1, buf, MAXBUF, 0);
    printf("%s", buf);
    /*소켓으로부터 welcome.txt 데이터 수신*/
    recv(Socket2, welcome, MAXBUF, 0);
    memset(buf, 0, MAXBUF);
    recv(Socket1, buf, MAXBUF, 0);
    printf("%s", buf);
    fp = fopen("welcome.txt", "w+"); // 읽기/쓰기 모드로 파일 열기(파일 없으면 생성)
    fwrite(welcome, sizeof(welcome), strlen(welcome), fp); // 열어놓은 파일에 데이터 write
    rewind(fp);
    // 파일 포인터 초기화
    /*저장된 파일 읽어와서 출력*/
    fread(buffer, 1, MAXBUF, fp);
    printf("%s", buffer);
    /*소켓 종료 및 파일 닫기*/
    close(Socket1);
    close(Socket2);
    fclose(fp);
    return 0;
}