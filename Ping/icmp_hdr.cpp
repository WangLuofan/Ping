//
//  icmp_hdr.cpp
//  Ping
//
//  Created by 王落凡 on 2016/11/10.
//  Copyright © 2016年 王落凡. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "icmp_hdr.hpp"

extern int packagesize;

icmp_hdr::icmp_hdr(const char* buf)
{
    this->cType = (unsigned char)buf[0];
    this->cCode = (unsigned char)buf[1];
    this->sCheckSum = (((unsigned short)buf[3]) << 8) | ((unsigned short)buf[2]);
    this->sFlag = (((unsigned short)buf[5]) << 8) | ((unsigned short)buf[4]);
    this->sSeqNum = (((unsigned short)buf[7]) << 8) | ((unsigned short)buf[6]);
}

void icmp_hdr::compute_checksum(const char* data)
{
    unsigned int checksum = 0;
    checksum += ((((unsigned short)this->cType) << 8) + this->cCode);
    checksum += ntohs(this->sFlag);
    checksum += ntohs(this->sSeqNum);
    
    int dataLen = MAKEEVEN(packagesize);
    for(int i = 0; i != dataLen; i = i + 2)
    {
        checksum += ((unsigned short)(*(data + i)) << 8);
        checksum += (unsigned short)(*(data + i + 1));
    }
    checksum = (checksum >> 16) + (checksum & 0xFFFF);
    checksum += (checksum >> 16);
    
    this->sCheckSum = htons(~((unsigned short)checksum));
    
    return ;
}
