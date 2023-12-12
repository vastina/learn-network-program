#define VASTINA_CPP
#include "../include/tools.h"


int main(){
    int clientsock = CreateClientSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    char buffer[BUFSIZ];

    while(true)
    {
        printf("send message or quit(enter quit or q to quit) : ");
		scanf("%s", buffer);
		if (-1 == write(clientsock, buffer, strlen(buffer)))
		{
            errorhandling("fail to send with code: %d\n", errno);
		}
        if(quitjudge(buffer)){
            //shutdown(clientsock, SHUT_WR);
            break;
        } 
        else if(sendfile(buffer)){
            FILE* fp;
            fp = fopen("b.txt", "wb");
            if(fp == NULL) printf("no such a file\n");
            int read_count = 0;
            if((read_count = read(clientsock, buffer, BUFSIZ) )> 0)
                fwrite((void*)buffer, sizeof(char), read_count, fp);
            fclose(fp);
            printf("file received\n");
            fclose(fp);
        }
        memset(buffer, 0, sizeof(buffer));
        
		if (read(clientsock, buffer, sizeof(char)*BUFSIZ) > 0)
		{
            if(quitjudge(buffer)){
                printf("serve disconnect\n");
                break;
            }
            printf("recive message:%s \n", buffer);
			memset(buffer, 0, sizeof(buffer));
		}	
    }
    printf("will be closed in one minute.......\n");
    int t = clock();
    while(clock()-t < 60) if(read(clientsock, buffer, BUFSIZ) ) break;
    printf("last message from serve: %s \n", buffer);

    close(clientsock);
    return 0;
}