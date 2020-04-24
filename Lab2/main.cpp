#include "HttpServer.h"
#include <unistd.h>
#include <stdio.h>
#include <getopt.h>

int main(int argc, char** argv)
{
	int port = 6666;
	if(argc >= 2) {
		port = atoi(argv[1]);
	}
	int numThread = 4;
	if(argc >= 3) {
		numThread = atoi(argv[2]);
	}

	swings::HttpServer server(port, numThread);
	server.run();

	return 0;
}
