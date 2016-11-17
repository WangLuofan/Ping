//
//  main.cpp
//  Ping
//
//  Created by 王落凡 on 2016/11/10.
//  Copyright © 2016年 王落凡. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ping.hpp"


char host[BUFSIZ] = {0};
int ttl = 0, count = INFINITE, timeout = 1, packagesize = 56;

void help(void)
{
    printf("usage:./ping [-c count] [-m ttl] [-t timeout] [-s packagesize] host\n");
    return ;
}

bool parse(int argc, const char* argv[])
{
    if(argc <= 1)
    {
        help();
        return false;
    }
    else if(argc == 2)
    {
        strcpy(host, argv[1]);
        return true;
    }
    else
    {
        int index = 1;
        for(; index < argc - 1; index += 2)
        {
            if(argv[index][0] != '-')
            {
                help();
                return false;
            }
            
            int param = ::atoi(argv[index + 1]);
            switch (argv[index][1])
            {
                case 'c':
                {
                    if(param == 0)
                    {
                        printf("./ping: invalid count of packets to transmit: '%s'\n", argv[index + 1]);
                        return false;
                    }
                    
                    count = param;
                }
                    break;
                case 't':
                {
                    if(param == 0)
                    {
                        printf("./ping: invalid timeout: '%s'\n", argv[index + 1]);
                        return false;
                    }
                    
                    timeout = param;
                }
                    break;
                case 'm':
                {
                    if(param == 0)
                    {
                        printf("./ping: invalid TTL: '%s'\n", argv[index + 1]);
                        return false;
                    }
                    
                    ttl = param;
                }
                    break;
                case 's':
                {
                    if(param == 0)
                    {
                        printf("./ping: invalid Package Size:'%s'\n", argv[index + 1]);
                        return false;
                    }
                    
                    const int maxPackageSize = 65507;
                    packagesize = param;
                    if(packagesize > maxPackageSize)
                    {
                        printf("./ping: package size too large: %d > %d\n", param, maxPackageSize);
                        return false;
                    }
                    else if(packagesize <= 0)
                    {
                        printf("./ping: package size must greater or equal than 0: %d < 0\n", param);
                        return false;
                    }
                }
                    
                default:
                    break;
            }
        }
        
        if(index != argc - 1)
        {
            help();
            return false;
        }else
            strcpy(host, argv[argc - 1]);
    }
    
    return true;
}

int main(int argc, const char * argv[])
{
    if(parse(argc, argv))
        ping();
    return 0;
}
