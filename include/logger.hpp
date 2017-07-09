#ifndef   LOGGER_H
#define   LOGGER_H
#include <string>
#include <vector>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "logger.hpp"

#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <cstring>

#define MAX_LISTEN 5

using namespace std;

class logger {
	int port;
	int rotation;
	string tag;
	string filter;
	string nofilter;
	vector<int> outmode; // 0(file), 1(socket)
	vector<string> outnames; // hostnames and file names
	vector<int> outports;

	int listenhandle; // socket handle for listening
	vector<int> writehandle;
	thread listener, collector, writer, rotator, killer;
	mutex mu_for_message, mu_for_write;
	condition_variable cv_for_collect, cv_for_write;
	vector<string> incoming_messages, writing_messages;

public:
	// constructor
	logger () {}
	logger (const logger &ob) {

	}
	logger (string gtag, int gport, int grotation, string
			gfilter, string gnofilter, vector<int>goutmode,
			vector<string>goutnames,vector<int>goutports
			):tag(gtag),port(gport),rotation(grotation),
			filter(gfilter),nofilter(gnofilter),
			outmode(goutmode), outnames(goutnames), outports(goutports)  {
		cout << "Called Constructor" << endl;
		// create listen handle
		createsocketid();
		//listener = thread(listener_function, ref(*this));
//		collector = thread(collector_function, ref(*this));
//		writer = thread(writer_function, ref(*this));
//		rotator = thread(rotator_function, ref(*this));
//		killer = thread(killer_function, ref(*this));
		//create write handle , or call other functions.

	}
	// destructor
	~logger () {
		cout << "Called Destructor" << endl;
//		listener.join();
//		collector.join();
//		writer.join();
//		rotator.join();
//		killer.join();
	}

	void createsocketid () {
		char buffer[256]; // for reading and writing
		int pid;
		struct sockaddr_in serv_addr, cli_addr;
		struct hostent *server;
		int addrlen = sizeof(cli_addr);
		// server call
		listenhandle = socket(AF_INET, SOCK_STREAM, 0);
		// creating a socket for sever
		if (listenhandle < 0) {
			perror("ERROR opening socket");
			exit(1);
		}
		bzero((char *) &serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = INADDR_ANY;
		serv_addr.sin_port = htons(port);
		/* Now bind the host address using bind() call.*/
		if (bind(listenhandle, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
			perror("ERROR on binding");
			exit(1);
		}
		listen(listenhandle,MAX_LISTEN);
		return;
	}

	static void listener_function (logger &log) {
		int n;
		char buffer[256];
		while (1) {
			bzero(buffer,256);
			n = read(log.getPort(),buffer,255);
			buffer[strlen(buffer) - 1] = '\0';
			if (n < 0) {
				perror("ERROR reading from socket");
				exit(1);
			}
			printf("Server message: executing \"%s\"\n",buffer);
			if (strcmp(buffer,"exit") == 0) {
				n = write(log.getPort(),"exit",16);
				printf("Exitting the current server..\n");
				break;
			}
			n = write(log.getPort(),"command_complete",16);
			if (n < 0) {
				perror("ERROR writing to socket");
				exit(1);
			}
		}
		close(log.getPort());
	}

	int getPort () {
		return port;
	}
	string getTag () {
		return tag;
	}
	string getFilter () {
		return filter;
	}
	string getNoFilter () {
		return nofilter;
	}
	vector<int> getOutmode () {
		return outmode;
	}
	vector<string> getOutnames () {
		return outnames;
	}
	vector<int> getOutports () {
		return outports;
	}
	void addOutput (int gmode, string gname, int gport) {
		outmode.push_back(gmode);
		outnames.push_back(gname);
		outports.push_back(gport);
	}
private:
	// disabling the copy constructor, because of handle copy
	// disabling the assignment operator
	// disabling move constructor
};


#endif
