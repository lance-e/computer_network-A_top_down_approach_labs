#include <cassert>
#include <cstdio>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>

std::string code = "";

int main(){
    
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(socketfd != -1);

    struct sockaddr_in server_addr;
    socklen_t server_addr_len = sizeof(server_addr);

    int ret = 0 ;
    ret = connect(socketfd, (struct sockaddr*)&server_addr,server_addr_len);
    if (ret == -1){
        printf("connect failed\n");
        return -1;
    }
    

}
