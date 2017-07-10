#ifndef   LOGGER_H
#define   LOGGER_H
#include <string>
#include <vector>
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
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
#define BUFFER_SIZE 2048
#define LISTEN_TIMEOUT 10

using namespace std;

class logger {
	int port;
	int rotation;
	int kill1, kill2, kill3, kill4;
	string tag;
	string filter;
	string nofilter;
	vector<int> outmode; // 0(file), 1(socket)
	vector<string> outnames; // hostnames and file names
	vector<int> outports;

	int listenhandle; // socket handle for listening
	vector<ofstream> writehandle;
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
		killall = 0;
		killsignal = 0;
		// create listen handle
		createsocketlistener();
		openwritehandles();
		listener = thread(listener_function, ref(*this));
		collector = thread(collector_function, ref(*this));
		writer = thread(writer_function, ref(*this));
		rotator = thread(rotator_function, ref(*this));
		killer = thread(killer_function, ref(*this));
		//create write handle , or call other functions.

	}
	// destructor
	~logger () {
		cout << "Called Destructor" << endl;
		kill1 = 1;
		listener.join();
		collector.join();
		writer.join();
		rotator.join();
		killer.join();
	}

	void createsocketlistener () {
		// listening for incoming messages
		struct sockaddr_in serv_addr;
		listenhandle = socket(AF_INET, SOCK_STREAM, 0);
		// creating a socket for sever
		if (listenhandle < 0) {
			cerr << "ERROR opening socket";
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
		struct timeval tv;
		tv.tv_sec = LISTEN_TIMEOUT;  // in seconds
		tv.tv_usec = 0;
		// setting the time out of recv call
		setsockopt(listenhandle, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv,sizeof(struct timeval));
		listen(listenhandle,MAX_LISTEN);
		return;
	}
	
	void openwritehandles () {
		int i = 0;
		for (auto it = outmode.begin(); it != outmode.end(); it++) {
			if (it == 0) {
				// file mode
				ofstream myfile;
				myfile.open (outnames[i]);
				writehandle.push_back(myfile);
			} else {
				// socket mode
				// :TODO
			}
			i++;
		}
	}

	static void listener_function (logger &log) {
		int n;
		char buffer[BUFFER_SIZE];
		while (1) {
			if (log.kill1 == 1) {
				cout << "killing listener thread" << endl;
				break;
			}
			bzero(buffer,BUFFER_SIZE);
			n = read(log.getPort(),buffer,BUFFER_SIZE);
			buffer[strlen(buffer) - 1] = '\0';
			if (n < 0) {
				cerr << "ERROR reading from socket" << endl;
				continue
			}
			// insert into message vector
			log.mu_for_message.lock();
			incoming_messages.push_back(string(buffer));
			log.mu_for_message.unlock();
		}
		close(log.getPort());
		kill2 =1;
	}
	
	static void collector_function (logger &log) {
		while (1) {
			if (log.kill2 == 1) {
				cout << "killing collector thread" << endl;
				break;
			}
			log.mu_for_message.lock();
			// move everything to writing_messages
			if (!incoming_messages.empty()) {
				writing_messages.insert(writing_messages.end(), incoming_messages.begin(), incoming_messages.end());
				incoming_messages.clear();
			}
			log.mu_for_message.unlock();
		}
		kill3 = 1;
	}
	
	static void rotator_function (logger &log) {
		while (1) {
			if (log.kill3 == 1) {
				cout << "killing rotator_function thread" << endl;
				break;
			}
			this_thread::sleep_for(std::chrono::seconds(2));
		}
		kill4 = 1;
	}
	
	static void killer_function (logger &log) {
		while (1) {
			if (log.kill4 == 1) {
				cout << "killing rotator_function thread" << endl;
				break;
			}
			this_thread::sleep_for(std::chrono::seconds(2));
		}
		for (auto it1 = log.writehandle.begin(); it1 != log.writehandle.end(); it1++) {
			it.close();
		}
		// close socket writing
		
		
	}
	
	
	
	static void writer_function (logger &log) {
		while (1) {
			if (log.killall == 1) {
				cout << "killing collector thread" << endl;
				break;
			}
			log.mu_for_message.lock();
			// write everything to output
			if (!log.writing_messages.empty()) {
				for (auto it = log.writing_messages.begin(); it != log.writing_messages.end(); it++) {
					for (auto it1 = log.writehandle.begin(); it1 != log.writehandle.end(); it1++) {
						it << it1 << endl;
					}
				}
				writing_messages.clear();
			}
			log.mu_for_message.unlock();
		}
	}
	
	static void collector_function (logger &log) {
		while (1) {
			if (log.killall == 1) {
				cout << "killing collector thread" << endl;
				break;
			}
			log.mu_for_message.lock();
			// move everything to writing_messages
			if (!incoming_messages.empty()) {
				writing_messages.insert(writing_messages.end(), incoming_messages.begin(), incoming_messages.end());
				incoming_messages.clear();
			}
			log.mu_for_message.unlock();
		}
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
