#ifndef TCPCLIENT_HPP
#define TCPCLIENT_HPP


// Server side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string>



#define MAXLINE 1024

class TcpClient{
public:
    static const int typeTemp = 0;
    static const int typeRH = 1;
    static const int typeCo2 = 2;
int type =0;
    static const int receivingPort = 6720;
    static const int sendingPort = 9889;
   const std::string ipAddress =  "127.0.0.1";
    void* receivingLoop(void* arg);
    void startReceivingThread();
    volatile   int sockfd, connfd;
    struct sockaddr_in servaddr, cliaddr;
    char buffer[MAXLINE];
    //static char *hello = "Hello from server(comms)";
    unsigned int lengthOfCliAdrr;
    bool isConnected = false;
    void sendString(std::string s);
    static const std::string TAG;

    void receive();

    void handlerSigpipe(int s);
    int startClientSocket(int sensorType);

    void sendBytes(unsigned char *data, int len);
    void startConnectingThread();
};

#endif // TCPCLIENT_HPP
