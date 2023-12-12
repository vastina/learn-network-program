#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

int main(){
    struct hostent *host;
    host = gethostbyname("www.github.com");

    printf("%s \n",host->h_name);
    for(int i=0; host->h_aliases[i]; ++i)
        printf("%s\n", host->h_aliases[i]);
    printf("%d\n", host->h_addrtype);
    printf("%d\n", host->h_length);
    for(int i=0; host->h_addr_list[i]; ++i)
        printf("%s\n", host->h_addr_list[i]);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    int sndbufsz, recvbufsz;
    int sb1 = sizeof(sndbufsz);
    int sb2 = sizeof(recvbufsz);
    int state = getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void*)&sndbufsz, &sb1);
    if(state) printf("sb\n");
    printf("%d\n", sndbufsz);

    state = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void*)&recvbufsz, &sb2);
    if(state) printf("sb\n");
    printf("%d\n", recvbufsz);

    sndbufsz = BUFSIZ;  recvbufsz = BUFSIZ;

    state = setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void*)&sndbufsz, sb1 );
    state = getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void*)&sndbufsz, &sb1);
    if(state) printf("sb\n");
    printf("%d\n", sndbufsz);

    state = setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void*)&recvbufsz, sb2 );
    state = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void*)&recvbufsz, &sb2);
    if(state) printf("sb\n");
    printf("%d\n", recvbufsz);



    // // struct sockaddr_in serveaddr;
    // // memset(&serveaddr, 0, sizeof(serveaddr));
    // // serveaddr.sin_addr.s_addr= inet_addr("39.156.66.10");
    // const char* sb = "39.156.66.10";
    // struct hostent *host2 = gethostbyaddr((void*)sb, 4, AF_INET);
    

    // printf("%s \n",host2->h_name);
    // for(int i=0; host2->h_aliases[i]; ++i)
    //     printf("%s\n", host2->h_aliases[i]);
    // printf("%d\n", host2->h_addrtype);
    // printf("%d\n", host2->h_length);
    // for(int i=0; host2->h_addr_list[i]; ++i)
    //     printf("%s\n", host2->h_addr_list[i]);
}