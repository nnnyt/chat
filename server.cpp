#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERVER_PORT 5208 //侦听端口
#define BUF_SIZE 1024
#define MAX_CLNT 256    // 最大连接数

void handle_clnt(int clnt_sock);
void send_msg(const std::string &msg);
int output(const char *arg,...);
int error_output(const char *arg,...);
void error_handling(const std::string &message);

int clnt_cnt=0;
int clnt_socks[MAX_CLNT];
std::mutex mtx;

int main(int argc,const char **argv,const char **envp){
    int serv_sock,clnt_sock;
    // sockaddr_in serv_addr{},clnt_addr{};
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_size;

    // if (argc!=2){
    //     error_output("Usage : %s <port> \n",argv[0]);
    //     exit(1);
    // }

    // 创建套接字，参数说明：
    //   AF_INET: 使用 IPv4
    //   SOCK_STREAM: 面向连接的数据传输方式
    //   IPPROTO_TCP: 使用 TCP 协议
    serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serv_sock == -1){
        error_handling("socket() failed!");
    }
    // 将套接字和指定的 IP、端口绑定
    //   用 0 填充 serv_addr （它是一个 sockaddr_in 结构体）
    memset(&serv_addr,0, sizeof(serv_addr));
    //   设置 IPv4
    //   设置 IP 地址
    //   设置端口
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    // serv_addr.sin_port=htons(atoi(argv[1]));
    serv_addr.sin_port = htons(SERVER_PORT);

    //   绑定
    if (bind(serv_sock, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1){
        error_handling("bind() failed!");
    }

    // 使得 serv_sock 套接字进入监听状态，开始等待客户端发起请求
    if (listen(serv_sock, MAX_CLNT)==-1){
        error_handling("listen() error!");
    }

    while(1){   // 循环监听客户端，永远不停止
        clnt_addr_size = sizeof(clnt_addr);
        // 当没有客户端连接时， accept() 会阻塞程序执行，直到有客户端连接进来
        clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
        if (clnt_sock == -1){
            error_handling("accept() failed!");
        }

        // 增加客户端数量
        mtx.lock();
        clnt_socks[clnt_cnt++] = clnt_sock;
        mtx.unlock();

        // 生成线程
        std::thread th(handle_clnt, clnt_sock);
        th.detach();

        output("Connected client IP: %s \n", inet_ntoa(clnt_addr.sin_addr));

    }
    close(serv_sock);
    return 0;
}

void handle_clnt(int clnt_sock){
    char msg[BUF_SIZE];

    while(recv(clnt_sock, msg, sizeof(msg),0) != 0){
        send_msg(std::string(msg));
    }
    // 客户端关闭连接
    mtx.lock();
    for (int i = 0; i < clnt_cnt ; i++) {
        if (clnt_sock == clnt_socks[i]){
            while (i++ < clnt_cnt-1){
                clnt_socks[i] = clnt_socks[i+1];
            }
            break;
        }
    }
    clnt_cnt--;
    mtx.unlock();
    close(clnt_sock);
}

void send_msg(const std::string &msg){
    // 广播
    mtx.lock();
    for (int i = 0; i < clnt_cnt ; i++) {
        send(clnt_socks[i], msg.c_str(), msg.length()+1, 0);
    }
    mtx.unlock();
}

int output(const char *arg, ...){
    int res;
    va_list ap;
    va_start(ap, arg);
    res = vfprintf(stdout, arg, ap);
    va_end(ap);
    return res;
}

int error_output(const char *arg, ...){
    int res;
    va_list ap;
    va_start(ap, arg);
    res = vfprintf(stderr, arg, ap);
    va_end(ap);
    return res;
}

void error_handling(const std::string &message){
    std::cerr<<message<<std::endl;
    exit(1);
}