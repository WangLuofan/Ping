//
//  ping.cpp
//  Ping
//
//  Created by 王落凡 on 2016/11/10.
//  Copyright © 2016年 王落凡. All rights reserved.
//

#include "ping.hpp"
#include "ip_hdr.hpp"
#include "icmp_hdr.hpp"
#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <sys/select.h>
#include <signal.h>

extern char host[BUFSIZ];
extern int ttl, count, timeout, packagesize;
static int seqNum = 0, received = 0;
static float min = UINT_MAX, avg = 0, max = 0;
static struct timeval btv{0, 0}, etv{0, 0};

#define IP_ADDR_LEN 20
#define SAFEFREE(pointer) do {  \
        if(pointer != NULL) { free(pointer); pointer = NULL;} \
    }while(0)

char* icmp_construct(void)
{
    char* icmp_data = (char*)malloc(sizeof(char) * MAKEEVEN(packagesize)) + sizeof(struct icmp_hdr);
    memset(icmp_data, 0, sizeof(char) * MAKEEVEN(packagesize) + sizeof(struct icmp_hdr));
    memset(icmp_data + sizeof(icmp_hdr), 1, sizeof(char) * MAKEEVEN(packagesize));
    
    ((struct icmp_hdr*)icmp_data)->cType = (char)0x08;
    ((struct icmp_hdr*)icmp_data)->sFlag = htons((unsigned short)getpid());
    ((struct icmp_hdr*)icmp_data)->sSeqNum = htons((unsigned short)++seqNum);
    
    ((struct icmp_hdr*)icmp_data)->compute_checksum(icmp_data + sizeof(struct icmp_hdr));
    
    return icmp_data;
}

void statistics(void)
{
    printf("\n--- %s ping statistics ---\n", host);
    printf("%d packets transmitted, %d packets received, %.1lf%% packet loss\n",
           seqNum, received, ((double)(seqNum - received) / seqNum) * 100);
    printf("round-trip min/avg/max = %.3f/%.3f/%f\n", min, avg / seqNum, max);
    return ;
}

void sig_int(int sig)
{
    statistics();
    _exit(-1);
    return ;
}

int SOCKET_RAW(void)
{
    int fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if(fd == -1)
    {
        perror("./ping: socket error");
        exit(-1);
    }
    
    //设置生存时间
    if(ttl != 0)
    {
        if(setsockopt(fd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0)
        {
            perror("./ping: setsockopt error");
            exit(-1);
        }
    }
    
    return fd;
}

void parse(const char* recvBuf, size_t bufLen, const char* ipAddr)
{
    struct ip_hdr* hdr_ip = new struct ip_hdr(recvBuf);
    struct icmp_hdr* hdr_icmp = new struct icmp_hdr(recvBuf + sizeof(struct ip_hdr));
    
    float timespec = (float)(etv.tv_sec - btv.tv_sec) * 1000 + ((float)(etv.tv_usec - btv.tv_usec)) / 1000;
    
    if(hdr_icmp->cType == 0 && hdr_icmp->cCode == 0)
        printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n", hdr_ip->sTotalLen, ipAddr, seqNum - 1,
               hdr_ip->cTTL, timespec);
    else if(hdr_icmp->cType == 3)
        printf("unreachable destination.\n");
    else if(hdr_icmp->cType == 11)
        printf("Request timeout for icmp_seq %d,  Time to live exceeded.\n", seqNum - 1);
    else
        printf("./ping: Unknown error occured.\n");
    
    fflush(stdout);
    ++received;
    
    if(timespec < min)
        min = timespec;
    if(timespec > max)
        max = timespec;
    avg += timespec;
    
    return ;
}

void ping(void)
{
    //解析主机地址
    struct hostent* _hstent = gethostbyname(host);
    if(_hstent == NULL)
    {
        printf("./ping: cannot resolve %s: Unknown host\n", host);
        return ;
    }
    
    strcpy(host, _hstent->h_name);
    
    char ipAddr[IP_ADDR_LEN] = {0};
    if(inet_ntop(AF_INET, (struct in_addr*)_hstent->h_addr, ipAddr, sizeof(char) * IP_ADDR_LEN) == NULL)
    {
        printf("./ping: cannot transfer address\n");
        return ;
    }
    
    //建立socket
    int fd = SOCKET_RAW();
    
    //填写IP地址信息
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_len = sizeof(struct sockaddr_in);
    addr.sin_port = htons(0);
    addr.sin_addr = *(struct in_addr*)_hstent->h_addr;
    
    //设置统计函数
    atexit(statistics);
    
    //捕获中断信号
    if(signal(SIGINT, sig_int) == SIG_ERR)
    {
        perror("signal error");
        return ;
    }
    
    //开始ping
    printf("PING %s(%s): %d data bytes\n", host, ipAddr, packagesize);
    for(int i = 0; i != count; ++i)
    {
        //防止溢出
        if(i == INT_MAX - 1)
            i = 0;
        
        //构建icmp回显请求报文
        char* icmp_data = icmp_construct();
        size_t dataLen = sizeof(struct icmp_hdr) + sizeof(char) * MAKEEVEN(packagesize);
        
        char recvBuf[BUFSIZ] = {0};
        //设置超时
        struct timeval _tout {timeout, 0};
        
        ssize_t bufLen = sendto(fd, icmp_data, dataLen, 0, (struct sockaddr*)&addr, (socklen_t)sizeof(struct sockaddr));
        //记录数据报发出时间
        gettimeofday(&btv, NULL);
        if(bufLen == 0)
        {
            perror("./ping: recvfrom error");
            return ;
        }
        
        fd_set readSet;
        while (true) {
            FD_ZERO(&readSet);
            FD_SET(fd, &readSet);
            int ret = select(fd + 1, &readSet, NULL, NULL, &_tout);
            
            if(ret == -1)
            {
                perror("./ping: select error");
                return ;
            }
            else if(ret == 0)
            {
                printf("Request timeout for icmp_seq %d\n", seqNum - 1);
                break;
            }
            else
            {
                if(FD_ISSET(fd, &readSet))
                {
                    bufLen = recvfrom(fd, recvBuf, BUFSIZ * sizeof(char), 0, NULL, NULL);
                    if(bufLen > 0)
                    {
                        gettimeofday(&etv, NULL);
                        parse(recvBuf, bufLen, ipAddr);
                        sleep(1);
                        break;
                    }
                    else
                    {
                        perror("./ping: recvfrom error");
                        return ;
                    }
                }
            }
        }
    }
    
    return ;
}
