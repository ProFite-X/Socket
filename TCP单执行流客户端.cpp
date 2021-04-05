#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet.in.h>
#include <arpa/inet.h>
#include<string.h>
#include<iostream>
int main()
{
	//1.创建套接字
	int listen_sockfd = sock(AF_INET, SOCK_STREAM, 0);
	if (listen_sockfd < 0)
	{
		perror("create listen_sockfd false!");
		return -1;
	}
	//2.connect连接 
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("172.16.99.129");
	addr.sin_port = htons(18888);
	int ret = connect(listen_sockfd, (struct sockaddr*)&addr, sizeof(addr));
	if(ret < 0)
	{
		perror("connect false!");
		return -1;
	}
	while(1)
	{
		char buf[1024]={0};
		printf("enter your mesage to server:/n");
		std::cin >> buf;
		ssize_t send_size = send(listen_sockfd, buf, strlen(buf),0);
		if(send_size < 0)
		{
			perror("send");
			continue;
		}
		
		memset(buf,'\0',sizeof(buf));
		ssize_t recv_size = recv(listen_sockfd, buf, sizeof(buf)-1, 0);
			if (recv_size < 0)
		{
			perror("recv");
			continue;
		}
		else if(recv_size == 0)
		{
			printf("peer shutdown!/n");
			close(listen_sock);
			return 0;
		}
		else
		{
			printf("client says: %s\n",buf);
		}
	}
	close(listen_sockfd);
	return 0;
}

