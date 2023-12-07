#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>

void errorhandling(const char* message, int error ){
	printf("%s : %d \n", message, error);
};

bool quitjudge(const char* s){
    if(s[0]=='q' && strlen(s)==1) return true;
    if(s[0]=='q' && s[1]=='u' && s[2]=='i' && s[3]=='t' && strlen(s)==4) return true;
    return false;
}; 


int CreateSeverSocket(int af,int type,int protocol)
{
	//创建空SOCKET
	//第一个参数 地址协议ipv4 ipv6 
	//第二个参数 传输协议类型 流式套接字 数据包 
	//第三个参数 使用具体的某个传输协议
	//传输协议为 TCP
	int so = socket(af, type, protocol);
	//TCP连接失败
	if (so == -1)
	{
        errorhandling("invalid socket: ", errno );
		return -1;
	}
	//给socket绑定ip地址和端口号
	struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;//和创建socket时必须一样
	short PORT = 8888;
	addr.sin_port = htons(PORT);//define htons 本地字节转网络字节
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");//网卡的任意ip//inet_addr("127.0.0.1");//转为二进制
	if (bind(so, (struct sockaddr*)&addr, sizeof(addr)) == -1)
	{
        errorhandling("connect fail with code: ", errno );
		return false;
	}
	//监听
	listen(so, 100);
	return so;
}

int CreateClientSocket(int af,int type,int protocol){
    int so = socket(af, type, protocol);
    if (so == -1)
	{
        errorhandling("invalid socket: ", errno);
		return -1;
	}
    struct sockaddr_in clntaddr;
    memset(&clntaddr, 0, sizeof(clntaddr));
    clntaddr.sin_family = AF_INET;
    clntaddr.sin_addr.s_addr= inet_addr("127.0.0.1");
	short PORT = 8888; printf("input port:");
	scanf("%hd", &PORT);
    clntaddr.sin_port = htons(PORT);
    if (connect(so, (struct sockaddr*)&clntaddr, sizeof(clntaddr)) == -1)
        errorhandling("connect fail: ", errno);
    
    return so;
}