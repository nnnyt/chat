#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
 #include <stdarg.h>

#define BUF_SIZE 1024
#define SERVER_PORT 5208 //侦听端口
#define IP "127.0.0.1"

void send_msg(int sock);
void recv_msg(int sock);
int output(const char *arg,...);
int error_output(const char *arg,...);
void error_handling(const std::string &message);

std::string name = "DEFAULT";
std::string msg;

int main(int argc,const char **argv,const char **envp){
    int sock;
    // sockaddr_in serv_addr{};
    struct sockaddr_in serv_addr;

    if (argc != 2){
        error_output("Usage : %s <Name> \n",argv[0]);
        exit(1);
    }

    // 客户端名称
    name = "["+std::string(argv[1])+"]";

    // sock=socket(PF_INET, SOCK_STREAM, 0);
    // 创建Socket,使用TCP协议
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == -1){
        error_handling("socket() failed!");
    }

    // 将套接字和指定的 IP、端口绑定
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(IP);
    serv_addr.sin_port = htons(SERVER_PORT);
    
    //连接服务器
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1){
        error_handling("connect() failed!");
    }
    // 向服务器发送自己的名字
    std::string my_name = "#new client:" + std::string(argv[1]);
    send(sock, my_name.c_str(), my_name.length() + 1, 0);
    
    // 生成发送、接受消息的线程
    std::thread snd(send_msg, sock);
    std::thread rcv(recv_msg, sock);
    
    snd.join();
    rcv.join();
    
    close(sock);

    return 0;
}

void send_msg(int sock){
    while(1){
        getline(std::cin, msg);
        if (msg == "Quit"|| msg == "quit"){
            close(sock);
            exit(0);
        }
        // 生成消息格式（[name] massage）
        std::string name_msg = name + " " + msg;
        send(sock, name_msg.c_str(), name_msg.length() + 1, 0);
    }
}

void recv_msg(int sock){
    char name_msg[BUF_SIZE + name.length() + 1];
    while (1){
        int str_len = recv(sock, name_msg, BUF_SIZE+name.length() + 1, 0);
        if (str_len == -1){
            exit(-1);
        }
        std::cout<<std::string(name_msg)<<std::endl;
    }
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