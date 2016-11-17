//
//  icmp_hdr.hpp
//  Ping
//
//  Created by 王落凡 on 2016/11/10.
//  Copyright © 2016年 王落凡. All rights reserved.
//

#ifndef icmp_hdr_hpp
#define icmp_hdr_hpp

#define MAKEEVEN(odd) (odd + (odd % 2))

typedef struct icmp_hdr
{
    unsigned char cType;                //8位类型
    unsigned char cCode;                //8位代码
    unsigned short sCheckSum;           //16位校验和
    unsigned short sFlag;               //16位标志
    unsigned short sSeqNum;             //16位序列号
    
    void compute_checksum(const char*);
    icmp_hdr(const char*);
}icmp_hdr;

#endif /* icmp_hdr_hpp */
