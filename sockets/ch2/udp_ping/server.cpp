#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <netinet/in.h>
#include <random>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 1024
int main(int argc , char* argv[]){
    if (argc <= 2){
        printf("usage: %s ip port", argv[0]);
        return -1;
    }

    const char* ip = argv[1];
    int port = atoi(argv[2]);

    struct sockaddr_in addr;
    bzero(&addr, sizeof addr);
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, ip , &addr.sin_addr );
    addr.sin_port = htons(port);


    int socketfd = socket(AF_INET, SOCK_DGRAM , 0);
    assert(socketfd != -1);

    int ret = 0;
    ret = bind(socketfd, (struct sockaddr*)&addr, sizeof(addr));
    if (ret == -1){
        printf("bind failed\n");
        close(socketfd);
        return -1;
    }

    std::random_device rd;
    std::mt19937  gen(rd());
    std::uniform_int_distribution<> distrib(0, 10);


    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    printf("server begin\n");
    while(1){
        char message[MAX_BUFFER_SIZE];

        int bytes = 0 ;
        bytes = recvfrom(socketfd, message, sizeof(message), 0, (struct sockaddr*)&client_addr, &client_addr_len);
        if (bytes < 0 ) {
            printf("recvfrom failed\n");
            continue;
        }
        message[bytes]  = '\0';
        
        std::string temp = std::string(message);
        std::transform(temp.begin() , temp.end() ,temp.begin() , ::toupper );

        if (distrib(gen) < 4){
            continue;
        }else{
            sendto(socketfd, message, sizeof(message),0 ,(struct sockaddr*)&client_addr, client_addr_len);
        }
    }
    close(socketfd);
}
