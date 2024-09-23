#include <algorithm>
#include <cassert>
#include <csetjmp>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netdb.h>
#include <sstream>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <strings.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <vector>
#include <fcntl.h>
#include <unistd.h>


#define MAX_BUFFER_SIZE  10240 + 4096
int main(int argc , char* argv[]){
    if (argc <= 1) {
        printf("usage: %s ip \n", argv[0]);
        return -1;
    }

    const char* ip = argv[1];

    // handle the ip and port
    struct sockaddr_in addr;
    bzero(&addr , sizeof addr);
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, ip , &addr.sin_addr);
    addr.sin_port = htons(6789);

    int listenfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(listenfd >= 0 );
    

    int ret = 0 ;

    ret = bind(listenfd , (struct sockaddr*)&addr , sizeof addr);
    if(ret == -1){
        printf("bind failed\n");
        return -1;
    };

    ret = listen(listenfd , 5);
    if(ret == -1){
        printf("listen failed\n");
        return -1;
    };
    int bytes = 0 ;

    while (1){
        printf("Ready to serve...\n");

        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int connfd = accept(listenfd, (struct sockaddr*)&client_addr  , &client_addr_len);
        assert(connfd != -1);

        printf("new connection from %d\n" , client_addr.sin_addr.s_addr);

        char buf[MAX_BUFFER_SIZE];
        bytes = recv(connfd, buf , sizeof(buf) - 1 , 0);
        if (bytes <= 0 ){
            close(connfd);
            continue;
        }
        
        buf[bytes] = '\0';


        std::vector<std::string> words ;
        std::stringstream ss(buf);
        std::string word;
        while(std::getline(ss , word , ' ')){
            words.emplace_back(word);
        }

        std::string filename = "./resource" + words[1];
        printf("filename : %s\n", filename.c_str());

        int fd = open(filename.c_str() , O_RDONLY ,0777);
        if (fd == -1 ){
            if (errno == ENOENT){
                printf("no such file! request for destination server!\n");
                
                std::string hostname = words[1].substr(1);
                size_t pos = hostname.find('/');

                struct addrinfo info , *res;
                bzero(&info, sizeof(info));
                info.ai_family = AF_INET;
                info.ai_socktype = SOCK_STREAM;

                if (getaddrinfo(hostname.substr(0 , pos).c_str(), "80", &info , &res) !=  0){
                    printf("get ip from host failed\n");
                    close(fd);
                    close(connfd);
                    continue;
                }

                //create socket
                int clientfd = socket(AF_INET, SOCK_STREAM, 0);
                if (clientfd == -1){
                    printf("create new socket failed\n");
                    close(fd);
                    close(connfd);
                    continue;
                }

                //TCP connection
                if (connect(clientfd, res->ai_addr , res->ai_addrlen) < 0 ){
                    printf("create tcp connection failed\n");
                    close(fd);
                    close(connfd);
                    close(clientfd);
                    continue;
                }
                printf("create tcp connection done\n");

                //send request
                std::string target;
                if(pos == std::string::npos){
                    target = "/";
                }else{
                    target = hostname.substr(pos + 1);
                }
                std::string request = "GET " + target+ " HTTP/1.1\r\n\r\n";
                if ( send(clientfd, request.c_str(), request.size(), 0) < 0) {
                    printf("send request failed\n");
                    close(fd);
                    close(connfd);
                    close(clientfd);
                    continue;
                }
                printf("send request done\n");
                
                //recv response
                bzero(buf, sizeof(buf));
                int i = 0 ;
                while (1) {
                    bytes = recv(clientfd, buf + i, sizeof(buf)- i -1 , 0);
                    if (bytes < 0 ){
                        printf("receive data from target server failed\n");
                        break;
                    }else if (bytes ==0){
                        break;
                    }
                    i += bytes;
                    printf("recv %d bytes\n", bytes);
                }
                buf[i] = '\0';
                printf("receive response done\n");

                //create file
                int createfd = open(filename.c_str() , O_CREAT | O_RDWR | O_TRUNC , 0777);
                if (createfd == -1) {
                    printf("create failed");
                    close(fd);
                    close(clientfd);
                    close(connfd);
                    close(createfd);
                    continue;
                } 
                printf("create file done\n");

                //write file
                if (write(createfd, buf , strlen(buf)) == -1){
                    printf("write file failed\n");
                    close(fd);
                    close(clientfd);
                    close(connfd);
                    close(createfd);
                    continue;
                }
                printf("write file done\n");
                close(createfd);

                //send response
                if (send(connfd,buf , sizeof(buf), 0) == -1){
                    printf("send response failed\n");
                    close(fd);
                    close(connfd);
                    close(clientfd);
                    continue;
                }
                printf("send response done\n");
            }else {
                printf("can't open file \n");
            }
            close(fd);
        }else{
            bzero(buf, sizeof(buf));
            bytes = read(fd, buf, sizeof(buf));
            if (bytes < 0 ){
                printf("read file failed\n");
                close(fd);
                close(connfd);
                continue;
            }
            buf[bytes] = '\0';
            printf("read file done\n");

            //send response
            if (send(connfd , buf , sizeof(buf), 0) == -1){
                printf("send response failed\n");
                close(fd);
                close(connfd);
                continue;
            }
            printf("send response done\n");
        }
        printf("serve over\n");
        close(connfd);
        close(fd);
    }
}

