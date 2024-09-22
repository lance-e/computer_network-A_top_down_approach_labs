#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>

std::string code_base64 = "\r\n";       //fix
std::string account_base64 = "\r\n";    //fix

int main(){

    int socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    assert(socketfd != -1);

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, "103.129.252.45", &server_addr.sin_addr);
    server_addr.sin_port = htons(25);

    socklen_t server_addr_len = sizeof(server_addr);

    int ret = 0 ;
    ret = connect(socketfd, (struct sockaddr*)&server_addr,server_addr_len);
    if (ret == -1){
        printf("connect failed\n");
        return -1;
    }
    
    char buf[1024];
    int bytes = recv(socketfd,buf , sizeof(buf)- 1 , 0);
    if (bytes < 0 ){
        printf("recv data failed\n");
        close(socketfd);
        return -1;
    }
    buf[bytes] = '\0';

    if (strncmp("220", buf, 3) != 0 ){
        printf("not 220 \n");
        close(socketfd);
        return -1;
    }

    //1 HELO
    const char* hello = "HELO smtp.163.com\r\n";
    bytes = send(socketfd, hello, strlen(hello) , 0);
    if (bytes < 0){
        printf("send HELO failed\n");
        close(socketfd);
        return -1;
    }

    bzero(buf, sizeof(buf));
    bytes = recv(socketfd,buf , sizeof(buf)- 1 , 0);
        printf("%s",buf);
    if (bytes < 0 ){
        printf("recv data failed\n");
        close(socketfd);
        return -1;
    }
    buf[bytes] = '\0';

    if (strncmp("250", buf, 3) != 0 ){
        printf("not 250 \n");
        close(socketfd);
        return -1;
    }

    //2 AUTH LOGIN
    const char* auth = "AUTH LOGIN\r\n";
    bytes = send(socketfd,auth , strlen(auth) , 0);
    if (bytes < 0){
        printf("send AUTH LOGIN failed\n");
        close(socketfd);
        return -1;
    }

    bzero(buf, sizeof(buf));
    bytes = recv(socketfd,buf , sizeof(buf)- 1 , 0);
        printf("%s",buf);
    if (bytes < 0 ){
        printf("recv data failed\n");
        close(socketfd);
        return -1;
    }

    buf[bytes] = '\0';
    if (strncmp("334", buf, 3) != 0 ){
        printf("not first 334\n");
        close(socketfd);
        return -1;
    }

        //account
    bytes = send(socketfd, account_base64.c_str(), account_base64.size() , 0);
    if (bytes < 0){
        printf("send account failed\n");
        close(socketfd);
        return -1;
    }

    bzero(buf, sizeof(buf));
    bytes = recv(socketfd,buf , sizeof(buf)- 1 , 0);
        printf("%s",buf);
    if (bytes < 0 ){
        printf("recv data failed\n");
        close(socketfd);
        return -1;
    }
    buf[bytes] = '\0';

    if (strncmp("334", buf, 3) != 0 ){
        printf("not second 334\n");
        close(socketfd);
        return -1;
    }

        //password
    bytes = send(socketfd, code_base64.c_str(), code_base64.size(), 0);
    if (bytes < 0){
        printf("send password failed\n");
        close(socketfd);
        return -1;
    }

    bzero(buf, sizeof(buf));
    bytes = recv(socketfd,buf , sizeof(buf)- 1 , 0);
        printf("%s",buf);
    if (bytes < 0 ){
        printf("recv data failed\n");
        close(socketfd);
        return -1;
    }
    buf[bytes] = '\0';

    if (strncmp("235", buf, 3) != 0 ){
        printf("not 235\n");
        close(socketfd);
        return -1;
    }
    //3 MAIL FROM 
    const char* from  = "MAIL FROM: <>\r\n";        //fix
    bytes = send(socketfd, from , strlen(from) , 0);
    if (bytes < 0){
        printf("send mail from failed\n");
        close(socketfd);
        return -1;
    }

    bzero(buf, sizeof(buf));
    bytes = recv(socketfd,buf , sizeof(buf)- 1 , 0);
        printf("%s",buf);
    if (bytes < 0 ){
        printf("recv data failed\n");
        close(socketfd);
        return -1;
    }
    buf[bytes] = '\0';

    if (strncmp("250", buf, 3) != 0 ){
        printf("not FROM 250\n");
        close(socketfd);
        return -1;
    }
    
    //4 RCPT TO
    const char* to = "RCPT TO: <>\r\n";             //fix
    bytes = send(socketfd, to , strlen(to) , 0);
    if (bytes < 0){
        printf("send rcpt to failed\n");
        close(socketfd);
        return -1;
    }

    bzero(buf, sizeof(buf));
    bytes = recv(socketfd,buf , sizeof(buf)- 1 , 0);
        printf("%s",buf);
    if (bytes < 0 ){
        printf("recv data failed\n");
        close(socketfd);
        return -1;
    }
    buf[bytes] = '\0';

    if (strncmp("250", buf, 3) != 0 ){
        printf("not TO 250\n");
        close(socketfd);
        return -1;
    }

    // DATA
    const char* data  = "DATA\r\n" ;
    bytes = send(socketfd, data , strlen(data) , 0);
    if (bytes < 0){
        printf("send data failed\n");
        close(socketfd);
        return -1;
    }

    bzero(buf, sizeof(buf));
    bytes = recv(socketfd,buf , sizeof(buf)- 1 , 0);
        printf("%s",buf);
    if (bytes < 0 ){
        printf("recv data failed\n");
        close(socketfd);
        return -1;
    }
    buf[bytes] = '\0';

    if (strncmp("354", buf, 3) != 0 ){
        printf("not 354\n");
        close(socketfd);
        return -1;
    }

    //content body
    bzero(buf, sizeof(buf));
    // fix
    const char* body = "From: \r\nTo: \r\nSubject: smtp lab\r\n\r\nhello?\nI'm lance , doing the smtp lab\n\r\n.\r\n" ;
    bytes = send(socketfd, body,strlen(body) , 0);
    if (bytes < 0){
        printf("send body failed\n");
        close(socketfd);
        return -1;
    }

    bzero(buf, sizeof(buf));
    bytes = recv(socketfd,buf , sizeof(buf)- 1 , 0);
        printf("%s",buf);
    if (bytes < 0 ){
        printf("recv data failed\n");
        close(socketfd);
        return -1;
    }
    buf[bytes] = '\0';

    if (strncmp("250", buf, 3) != 0 ){
        printf("not 250\n");
        close(socketfd);
        return -1;
    }

    //QUIT
    bzero(buf, sizeof(buf));
    const char* quit = "QUIT\r\n" ;
    bytes = send(socketfd, quit , strlen(quit) , 0);
    if (bytes < 0){
        printf("send QUIT failed\n");
        close(socketfd);
        return -1;
    }

    bzero(buf, sizeof(buf));
    bytes = recv(socketfd,buf , sizeof(buf)- 1 , 0);
        printf("%s",buf);
    if (bytes < 0 ){
        printf("recv data failed\n");
        close(socketfd);
        return -1;
    }
    buf[bytes] = '\0';

    if (strncmp("221", buf, 3) != 0 ){
        printf("not 221\n");
        close(socketfd);
        return -1;
    }

   
    printf("send all message OK\n");
    printf("client close...\n");
    close(socketfd);
}
