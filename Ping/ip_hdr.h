//
//  ip_hdr.hpp
//  Ping
//
//  Created by 王落凡 on 2016/11/10.
//  Copyright © 2016年 王落凡. All rights reserved.
//

#ifndef ip_hdr_hpp
#define ip_hdr_hpp

typedef struct ip_hdr
{
    unsigned char cVerLen;                  //4位版本，4位首部长度
    unsigned char cTos;                     //8位服务类型
    unsigned short sTotalLen;               //16位总长度
    unsigned short sIdentifier;             //16位标识
    unsigned short sFlagsOffset;            //3位标志和13位片偏移
    unsigned char cTTL;                     //8位TTL
    unsigned char cProtocol;                //8位协议
    unsigned short sCheckSum;               //16位首部校验和
    unsigned char cSrcAddr[4];              //32位源地址
    unsigned char cDstAddr[4];              //32位目的地址
    
    ip_hdr(const char*);
    
}ip_hdr;

#endif /* ip_hdr_hpp */
