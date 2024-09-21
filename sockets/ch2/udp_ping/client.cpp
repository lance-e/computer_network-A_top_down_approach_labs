#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>
int main(int argc , char* argv[]){
    if (argc <= 2){
        printf("usage: %s ip port", argv[0]);
        return -1;
    }

    const char* ip= argv[1];
    int port = atoi(argv[2]);

    //client address
    struct sockaddr_in  server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET,ip , &server_addr.sin_addr);
    server_addr.sin_port = htons(port);
    socklen_t  len = sizeof(server_addr);

    int socketfd = socket(AF_INET, SOCK_DGRAM, 0);

    //set timeout 
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec =  0 ;
    setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO,(const char* )&tv ,sizeof(tv));

    printf("begin\n");
    for(int i =1 ; i <= 10 ; i++){
        char message[1024];
        snprintf(message, sizeof(message) ,"this is %d time message\n",  i); 

        auto time1 = std::chrono::high_resolution_clock::now();
        sendto(socketfd, message, strlen(message), 0 , (struct sockaddr*)&server_addr, len);
        
        char buf[1024];
        struct sockaddr_in client_addr;
        socklen_t client_addr_len =  sizeof(client_addr);

        int bytes = 0 ; 
        bytes =  recvfrom(socketfd , buf, sizeof(buf) , 0 , (struct sockaddr*)&client_addr ,&client_addr_len);
        auto time2 = std::chrono::high_resolution_clock::now();
        if (bytes <=  0){
            printf("Ping %d timeout\n", i);
            continue;
        }else {
            auto duration =  std::chrono::duration_cast<std::chrono::microseconds>(time2 - time1);
            std::cout<<"Ping " << i << " "  << duration.count() << "us" <<std::endl;
        }
    }

    printf("over\n");
    close(socketfd);
}
