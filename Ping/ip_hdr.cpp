//
//  ip_hdr.cpp
//  Ping
//
//  Created by 王落凡 on 2016/11/10.
//  Copyright © 2016年 王落凡. All rights reserved.
//

#include "ip_hdr.hpp"

ip_hdr::ip_hdr(const char* buf)
{
    this->cVerLen = (unsigned char)buf[0];
    this->cTos = (unsigned char)buf[1];
    this->sTotalLen = ((unsigned short)buf[3] << 8) | ((unsigned short)buf[2]);
    this->sIdentifier = ((unsigned short)buf[5] << 8) | ((unsigned short)buf[4]);
    this->sFlagsOffset = ((unsigned short)buf[7] << 8) | ((unsigned short)buf[6]);
    this->cTTL = (unsigned char)buf[8];
    this->cProtocol = (unsigned char)buf[9];
    this->sCheckSum = ((unsigned short)buf[11] << 8) | ((unsigned short)buf[10]);
    this->cSrcAddr[0] = (unsigned char)buf[12];
    this->cSrcAddr[1] = (unsigned char)buf[13];
    this->cSrcAddr[2] = (unsigned char)buf[14];
    this->cSrcAddr[3] = (unsigned char)buf[15];
    this->cDstAddr[0] = (unsigned char)buf[16];
    this->cDstAddr[1] = (unsigned char)buf[17];
    this->cDstAddr[2] = (unsigned char)buf[18];
    this->cDstAddr[3] = (unsigned char)buf[19];
}
