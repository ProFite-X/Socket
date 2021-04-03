#pragma once
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <string.h>
using namespace std;

//创建TCP类，提供监听listen,accept,recv,send等功能
class TcpSvr{
  public:
    //构造函数
    TcpSvr()
      :sock_fd_(-1)
    {

    }
    ~TcpSvr()
    {
    }
    //创建套接字
    int CreateSocket()
    {
      sock_fd_ = socket(AF_INET, SOCK_STREAM, 0);
      if(sock_fd_ < 0)
      {
        //创建套接字失败
        perror("socket\n");
        return -1;
      }
      printf("套接字创建成功！\n");
      return 0;
    }
    //绑定地址信息
    int Bind(const string& IP, const uint16_t port)
    {
      sockaddr_in addr;
      //协议
      addr.sin_family = AF_INET;
      //IP地址
      addr.sin_addr.s_addr = inet_addr("0.0.0.0");
      //端口
      addr.sin_port = htons(19999);
      int ret = bind(sock_fd_, (struct sockaddr*)&addr, sizeof(addr));
      if(ret < 0)
      {
        perror("bind");
        return -1;
      }
      printf("地址绑定成功！\n");
      return 0;
    }
    //listen侦听套接字
    int Listen(int backlog = 5)
    {
      int ret = listen(sock_fd_, backlog);
      if(ret < 0)
      {
        perror("listen");
        return -1;
      }
      printf("监听成功！\n");
      return 0;
    }
    //获取新连接
    int Accept(TcpSvr* ts, struct sockaddr_in* peer_addr, socklen_t* peer_addrlen)
    {
      int new_sock = accept(sock_fd_, (struct sockaddr*)peer_addr, peer_addrlen);
      if(new_sock < 0)
      {
        perror("accept");
        return -1;
      }
      else 
      {
        printf("获取新连接成功！\n");
        ts->SetSocketFd(new_sock);
        return 0;
      }
    }

    void SetSocketFd(int fd)
    {
      sock_fd_ = fd;
      printf("SetSockfd成功！");
    }
    //发送信息
    int Send(const string& data)
    {
      ssize_t send_size = send(sock_fd_, data.c_str(), data.size(), 0);
      if(send_size < 0)
      {
        perror("send");
        return -1;
      }
      return send_size;
    }
    //接收信息
    int Recv(string* data)
    {
      char buf[1024] = {0};
      ssize_t recv_size = recv(sock_fd_, buf, sizeof(buf) - 1, 0);
      if(recv_size < 0)
      {
        perror("recv");
        return -1;
      }
      else if(recv_size == 0)
      {
        printf("peer shutdown...\n");
        return 0;
      }
      data->assign(buf, strlen(buf));
      return recv_size;
    }
    int GetSockfd()
    {
      return sock_fd_;
    }
    private:
    //套接字
    int sock_fd_;
};

