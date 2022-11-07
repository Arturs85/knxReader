#include "tcpclient.hpp"
#include <signal.h>

#include <iostream>
#include <thread>
#include "uicom.hpp"
#define SA struct sockaddr
const std::string TcpClient::TAG = "[tcpClient] ";


void TcpClient::startReceivingThread(){


    std::thread t1(&TcpClient::receive,this);
    t1.detach();
}

int TcpClient::startClientSocket(int sensorType)
{
    type = sensorType;
    int sock = 0, valread, client_fd;
    char subCo2[] = {0x00,0x05,0x00,0x22,0x00,0x03,0x00};
    char subTem[] = {0x00,0x05,0x00,0x22,0x00,0x01,0x00};
    char subRh[] = {0x00,0x05,0x00,0x22,0x00,0x02,0x00};

    char* queryMsg;
    if(sensorType == typeTemp) queryMsg = subTem;
    else if(sensorType == typeRH) queryMsg = subRh;
    else if(sensorType == typeCo2) queryMsg = subCo2;
    char buffer[1024] = { 0 };
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(receivingPort);

    // Convert IPv4 and IPv6 addresses from text to binary
    // form
    if (inet_pton(AF_INET, ipAddress.data(), &servaddr.sin_addr)
            <= 0) {
        printf(
                    "\nInvalid address/ Address not supported \n");
        return -1;
    }

    if ((client_fd
         = connect(sockfd, (struct sockaddr*)&servaddr,
                   sizeof(servaddr)))
            < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    isConnected = true;
    send(sockfd, queryMsg, 7, 0);
    printf("Hello message sent\n");
    startReceivingThread();
    return 0;

}

void TcpClient::receive(){

    std::cout<<TAG<<" receiving loop started "<<std::endl;
    char buffer[MAXLINE] = {0};
    std::string name ="temp";
    if(type == typeRH) name = "rh";
    else if(type == typeCo2) name = "co2";
    while(true){
        int  n = read( sockfd , buffer, 1024);

        // int   n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, ( struct sockaddr *) &servaddr, &lengthOfCliAdrr);

        if(n>=1){
            std::cout<<TAG<<" received from "<<servaddr.sin_addr.s_addr<<" no of bytes: "<<n<<std::endl;
            buffer[n] = '\0';
            printf("%s\n",buffer );
            int offset =0;
            if((n==8 && buffer[3] == 0x02)||(n==10 && buffer[5] == 0x02)){//address 2?
                if(n == 10) offset =2;
                std::cout<<"data: "<<(int)buffer[6]<<" "<<(int)buffer[7]<<std::endl;

                int16_t temp;


                memcpy(&temp , buffer+5+offset, 2);
                memcpy((&temp), buffer+7+offset, 1);

                int16_t mantisaMask = 0b1000011111111111;
                int16_t powMask = 0b0111100000000000;
                int16_t pow = (temp & powMask)>>11;
                int16_t mant = temp & mantisaMask;
                float val = 0.01*mant*(1<<(pow));

                UiCom::sendAir(std::to_string(val),name);
                std::cout<<"temp "<<temp<<" pow "<<pow<<" mant "<<mant<<" data float: "<<val<<std::endl;
            }

        }else{
            // canSend=false;
            std::cout<<"receiving error "<<n<<std::endl;
            break;
        }
    }
}

void TcpClient::sendBytes(unsigned char* data, int len)
{
    if(isConnected){
        int sent =  send (sockfd, data, len,MSG_DONTWAIT|MSG_NOSIGNAL);

        if(sent<0){
            std::cout<<TAG<<"connfd: "<<sockfd<<" , could not send  "<<len <<std::endl;
            // startWaitForClientThread();
        }else if (len == 4) {
            std::cout<<TAG<<"sent no of bytes: "<<sent<<" , int value:  "<<*((int*)data) <<std::endl;

        }
    }
}

void TcpClient::sendString(std::string s)
{
    if(isConnected){
        int sent =  send (sockfd, s.data(), s.size(),MSG_DONTWAIT|MSG_NOSIGNAL);

        if(sent<0){
            std::cout<<TAG<<"connfd: "<<sockfd<<" , could not send: "<<s<<std::endl;
            // startWaitForClientThread();
        }
    }
}

void TcpClient::handlerSigpipe(int s){
    //  Control::pathExecutor.te.motorControl->rc->shutDown();
    //usleep(1000);

    //Control::pathExecutor.te.motorControl->rc->shutDown();
    std::cout<<"Caught signal"<<  s<<std::endl;;

    exit(0);

}

