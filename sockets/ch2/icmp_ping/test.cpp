#include <cstdio>
#include <netinet/ip.h>
int main(){
    struct iphdr* hdr ;
    hdr->ihl  = 1;
    printf("it is %d\n" , hdr->ihl);
}
