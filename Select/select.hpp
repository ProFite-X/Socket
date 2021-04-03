#pragma once 
#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <vector>
#include "tcp.hpp"
using namespace std;
class SelectSvr
{
  public:
    //构造函数
    SelectSvr()
      :nfds_(-1)
    {
      FD_ZERO(&readfds_);
    }
    //析构函数
    ~SelectSvr(){}

    //添加事件集合
    void AddFd(int fd)
    {
      FD_SET(fd, &readfds_);
      //printf("添加fd: %d成功！\n",fd);
      //判断最大文件描述符是否改变
      if(fd > nfds_)
      {
        nfds_ = fd;
      }
      // printf("添加fd后，nfds 为 : %d\n",nfds_);
    }

    //删除事件集合
    void DeleteFd(int fd)
    {
      //删除文件描述符事件
      FD_CLR(fd, &readfds_);
      //判断最大的文件描述符是否改变
      for(int i = nfds_; i >= 0; i--)
      {
        if(FD_ISSET(i, &readfds_) == 1)
        {
          nfds_ = i;
          break;
        }
      }
    }

    //select监听事件
    int SelectWait(struct timeval* tv, std::vector<TcpSvr>* vec)
    {
      fd_set tmp = readfds_;
      int ret = select(nfds_ + 1, &tmp, NULL, NULL, tv);
      // printf("SelectWait ：nfds 的值为 ：%d\n", nfds_);
      if(ret < 0)
      {
        perror("select");
        return -1;
      }
      else if(ret == 0)
      {
        //说明阻塞超时了
        printf("time out...\n");
        return 0;
      }
      //正常监听返回结果
      for(int i = 0; i <= nfds_; i++)
      {
        if(FD_ISSET(i, &tmp) == 1)
        {
          TcpSvr ts;
          ts.SetSocketFd(i);
          vec->push_back(ts);
        }
      }
      return ret;
    }
    
  private:
    //听事件集合
    fd_set readfds_;
    //最大描述符
    int nfds_;
};
