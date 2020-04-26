#include "HttpServer.h"
#include <unistd.h>
#include <stdio.h>
#include <getopt.h>
#include <arpa/inet.h> // htonl, htons
#include <iostream>
#define DEBUG false
int main(int argc, char** argv)
{
	int c,numThread=4,num=0;
	std::string ip="127.0.0.1",port="6666",proxy;
    int digit_optind = 0;
    while (1) {
        int this_option_optind = optind ? optind : 1;
        int option_index = 0;
        static struct option long_options[] = {
            {"ip",    required_argument, 0,  1 },
            {"port",  required_argument, 0,  2 },
            {"proxy", required_argument, 0,  3 },
            {"number-thread", required_argument, 0,  4 },
            {0,       0,               0,  0 }
        };
       	c = getopt_long(argc, argv, "",
                 long_options, &option_index);
        if (c == -1)
            break;
 		num++;
       	switch (c) {
        	case 1:
        		ip=optarg;
        		break;
        	case 2:
        		port=optarg;
        		break;
        	case 3:
            	proxy=optarg;
            	break;
            case 4:
            	numThread=std::stoi(optarg);
       		case '?':
            	break;
 			
       		default:
            	if(DEBUG) printf("?? getopt returned character code 0%o ??\n", c);
       	}
    }
    if(DEBUG){
    	std::cout<<"ip:"<<ip<<' ';
    	std::cout<<"port:"<<port<<' ';
    	std::cout<<"proxy:"<<proxy<<' ';
    	std::cout<<"number-thread:"<<numThread<<std::endl;
    }
    const char* cs;
    cs=ip.c_str();
    in_addr ipAddr;
    ipAddr.s_addr=inet_addr(cs);
	swings::HttpServer server(ipAddr,std::stoi(port), numThread);
	server.run();

	return 0;
}
