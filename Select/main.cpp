#include "select.hpp"
#include "tcp.hpp"
#define CHECK_RET(p) if(p < 0){return 0;}

int main(int argc, char* argv[])
{
  if(argc != 3)
  {
    printf("usage...\n");
    return 0;
  }
  TcpSvr ts;

  string ip = argv[1];
  uint16_t port = atoi(argv[2]);
  CHECK_RET(ts.CreateSocket());
  //创建套接字
  CHECK_RET(ts.Bind(ip, port));
  //绑定地址信息
  CHECK_RET(ts.Listen());
  //侦听套接字
  
  SelectSvr ss;
  ss.AddFd(ts.GetSockfd());
  //将侦听套接字加入到听时间集合当中去
  
  while(1)
  {
    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    std::vector<TcpSvr> vec;
    vec.clear();
    //开始侦听
    int ret = ss.SelectWait(&tv, &vec);
    cout << "vec.size(): " << vec.size() << endl;
    if(ret < 0)
    {
      perror("select");
      return -1;
    }
    else if(ret == 0)
    {
      continue;
    }
    //开始循环判断事件集合中的就绪描述符
    for(size_t i = 0; i < vec.size(); i++)
    {
      //如果是侦听套接字
      if(vec[i].GetSockfd() == ts.GetSockfd())
      {
        cout << "begin accept...\n"<< endl;
        //创建新的套接字
        TcpSvr new_ts;
        struct sockaddr_in peer_addr;
        socklen_t peer_addrlen = sizeof(peer_addr);
        ts.Accept(&new_ts, &peer_addr, &peer_addrlen);
        printf("new socket is %d, peer_ip is %s, peer_port is %d\n", new_ts.GetSockfd(), inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port));
        ss.AddFd(ts.GetSockfd());
      }
      else 
      {
        //开始沟通
        TcpSvr new_tcp_svr = vec[i];
        string data;
        data.clear();
        int ret = new_tcp_svr.Recv(&data);
        if(ret < 0)
        {
          perror("Recv");
          //对端关闭了，那么关闭套接字
          close(new_tcp_svr.GetSockfd());
          //将文件描述符从事件集合中移除出来
          ss.DeleteFd(new_tcp_svr.GetSockfd());
          continue;
        }
        else if(ret == 0)
        {
          close(new_tcp_svr.GetSockfd());
          ss.DeleteFd(new_tcp_svr.GetSockfd());
          continue;
        }
        printf("Recv data %s\n", data.c_str());
        string send_data = "i am server~~";
        new_tcp_svr.Send(send_data);
      }
    }
  }
  close (ts.GetSockfd());
  return 0;
}
