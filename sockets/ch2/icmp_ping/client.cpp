#include <iostream>
#include <cstddef>
#include <cstdio>
#include <ctime>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <strings.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <sys/select.h>
#include <unistd.h>

using namespace std;

#define MAX_BUFFER_SIZE 1024
char buf[MAX_BUFFER_SIZE];

unsigned short check_sum(void * b, int len){
    unsigned short *buf = (unsigned short *)b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2) {
        sum += *buf++;
    }
    if (len == 1) {
        sum += *(unsigned char *)buf;
    }
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}


double receiveOnePing(int mySocket, int ID, double timeout, struct sockaddr_in &destAddr) {
    double timeLeft = timeout;

    while (true) {
        struct timeval tv;
        tv.tv_sec = static_cast<long>(timeLeft);
        tv.tv_usec = static_cast<long>((timeLeft - static_cast<long>(timeLeft)) * 1000000);
        
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(mySocket, &readfds);

        int selectResult = select(mySocket + 1, &readfds, nullptr, nullptr, &tv);
        double howLongInSelect = (tv.tv_sec + tv.tv_usec / 1000000.0);

        if (selectResult == 0) { // Timeout
            printf("Request timed out.");
            return -1;
        }

        char recPacket[1024];
        socklen_t addrLen = sizeof(destAddr);
        ssize_t bytesReceived = recvfrom(mySocket, recPacket, sizeof(recPacket), 0, (struct sockaddr*)&destAddr, &addrLen);
        recPacket[bytesReceived] = '\0';

        double now = time(nullptr);

        // Fetch the ICMP header from the IP packet
        struct icmp* icmp = (struct icmp*)(recPacket + 20 );
        char data = icmp->icmp_dun.id_data[0];


        return now - double(data);
    }
}

void sendOnePing(int mySocket, const string &destAddr, int ID) {
    uint16_t myChecksum = 0;
    struct icmp header;
    memset(&header, 0, sizeof(header));

    header.icmp_type = ICMP_ECHO;
    header.icmp_id = htons(ID);
    header.icmp_seq = 1;
    header.icmp_dun.id_data[0] = time(nullptr);


    // Calculate checksum
    myChecksum = check_sum(&header, sizeof(header));
    header.icmp_cksum = myChecksum;

    // Send packet
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, destAddr.c_str(), &addr.sin_addr);
    sendto(mySocket, &header, sizeof(header), 0, (struct sockaddr *)&addr, sizeof(addr));
}



double doOnePing(const string& destAddr, double timeout) {
    int mySocket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (mySocket < 0) {
        perror("Socket creation failed");
        return -1;
    }

    int myID = getpid() & 0xFFFF;

    sendOnePing(mySocket, destAddr, myID);
    
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    inet_pton(AF_INET,destAddr.c_str() , &addr.sin_addr);


    double delay = receiveOnePing(mySocket, myID, timeout, addr);
    close(mySocket);
    return delay;
}

void ping(const string &host, double timeout = 1) {
    struct addrinfo info , *res;
    bzero(&info, sizeof(info));
    info.ai_family = AF_INET;
    info.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(host.c_str(), nullptr, &info , &res) !=  0){
        printf("get ip from host failed\n");
        return;
    }

    string dest = res->ai_addr->sa_data;
    cout << "Pinging " << dest << " using C++:" << endl;

    while (true) {
        double delay = doOnePing(dest, timeout);
        cout << delay << " ms" << endl;
        sleep(1); // one second
    }
}

int main(){
    ping("www.baidu.com");
}
