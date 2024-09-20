#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <vector>
#include <fcntl.h>
#include <unistd.h>


#define MAX_BUFFER_SIZE  1024
int main(int argc , char* argv[]){
    if (argc <= 2) {
        printf("usage: %s ip port\n", argv[0]);
        return -1;
    }

    const char* ip = argv[1];
    int port = atoi(argv[2]);

    // handle the ip and port
    struct sockaddr_in addr;
    bzero(&addr , sizeof addr);
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, ip , &addr.sin_addr);
    addr.sin_port = htons(port);

    int listenfd = socket(AF_INET , SOCK_STREAM, 0);
    assert(listenfd >= 0 );


    struct linger temp = {1 , 0 };
    setsockopt(listenfd, SOL_SOCKET, SO_LINGER, &temp , sizeof temp);

    int ret = 0 ;

    ret = bind(listenfd , (struct sockaddr*)&addr , sizeof addr);
    if(ret != -1){
        printf("bind failed\n");
        return -1;
    };

    ret = listen(listenfd , 5);
    if(ret != -1){
        printf("listen failed\n");
        return -1;
    };

    printf("Ready to serve...\n");
    while (1){

        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int conn = accept(listenfd, (struct sockaddr*)&client_addr  , &client_addr_len);
        assert(conn != -1);


        char buf[MAX_BUFFER_SIZE];
        int index = 0 ;
        int bytes = 0 ;

        //read data
        bytes = recv(conn, buf ,MAX_BUFFER_SIZE -1 , 0);
        if (bytes < 0 ){
            printf("read error\n");
            return -1;
        }
        buf[bytes] = '\0';

        std::vector<std::string> words ;
        std::stringstream ss(buf);
        std::string word;
        while(std::getline(ss , word , ' ')){
            words.emplace_back(word);
        }

        std::string filename = "." + words[1];

        char readbuf[MAX_BUFFER_SIZE];
        int fd = open(filename.c_str() , O_RDONLY );
        if (fd == -1){
            close(fd);
            std::string message = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<html><body>404 NotFound</body></html>";
            send(conn, &message, message.size(), 0);
        }else {
           int readbytes =  read(fd, readbuf , sizeof(readbuf));
           if (readbytes == -1){
                std::string message = "HTTP/1.1 403 Forbidden\r\nContent-Type: text/html\r\n\r\n<html><body>403 Forbidden</body></html>";
                send(conn, &message, message.size() , 0);
           }else{
               readbuf[readbytes] = '\0';
               std::string message = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + std::string(readbuf);
                send(conn, &message, message.size() , 0);
           }
        }
        close(fd);
        printf("serve over\n");
        close(conn);
    }
}