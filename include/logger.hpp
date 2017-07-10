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
		cout << "Called Constructor for tag=" << gtag << " with port=" << gport << endl;
		killall = 0;
		killsignal = 0;
		// create listen handle
		cout << "opening reading socket..." << endl;
		createsocketlistener();
		cout << "opening writing handles..." << endl;
		openwritehandles();
		cout << "creating listener_function_parent thread..." << endl;
		listener = thread(listener_function_parent, ref(*this));
		cout << "creating collector_function thread..." << endl;
		collector = thread(collector_function, ref(*this));
		cout << "creating writer_function thread..." << endl;
		writer = thread(writer_function, ref(*this));
		cout << "creating rotator_function thread..." << endl;
		rotator = thread(rotator_function, ref(*this));
		cout << "creating killer_function thread..." << endl;
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
		cout << "socket listener created properly" << endl;
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
		cout << "write handles are opened properly" << endl;
	}

	int accept_client(int s, int timeout) {
	   int iResult;
	   struct timeval tv;
	   fd_set rfds;
	   FD_ZERO(&rfds);
	   FD_SET(s, &rfds);

	   tv.tv_sec = (long)timeout;
	   tv.tv_usec = 0;

	   iResult = select(s + 1, &rfds, (fd_set *) 0, (fd_set *) 0, &tv);
	   if(iResult > 0)
	   {
		  return accept(s, NULL, NULL);
	   }
	   else
	   {
		 //always here, even if i connect from another application
	   }
	   return 0;
	}
	
	static void listener_function_parent (logger &log) {
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
		cout << "socket listener created properly" << endl;
		vector <thread *> allth;
		while (1) {
			printf ("listening for new connection client...\n");
			// newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &addrlen);
			newsockfd = accept_client(sockfd, 30);
			printf("Recieved someone...\n");
			if (newsockfd <= 0) {
				perror("ERROR on accept");
				continue;
			}
			if (log.kill1 == 1) {
				cout << "killing listener parent thread" << endl;
				break;
			}
			/* Create child thread */
			thread t1 = thread(listener_function, ref(log), newsockfd);
			allth.push_back(&t1);
		}
		for (auto it = allth.begin(); it != allth.end(); it++) {
			it->join();
		}
	}
	
	static void listener_function (logger &log, int sock) {
		struct timeval tv;
		tv.tv_sec = LISTEN_TIMEOUT;  // in seconds
		tv.tv_usec = 0;
		// setting the time out of recv call
		setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv,sizeof(struct timeval));
		int n;
		char buffer[BUFFER_SIZE];
		while (1) {
			if (log.kill1 == 1) {
				cout << "killing listener child thread" << endl;
				break;
			}
			cout << "listening in listening thread.." << endl;
			bzero(buffer,BUFFER_SIZE);
			n = read(sock,buffer,BUFFER_SIZE);
			buffer[strlen(buffer) - 1] = '\0';
			cout << "got the message =" << buffer << endl;
			if (n <= 0) {
				cerr << "ERROR reading from socket" << endl;
				continue
			}
			// insert into message vector
			log.mu_for_message.lock();
			cout << "pushing the message in incoming_messages" << endl;
			incoming_messages.push_back(string(buffer));
			log.mu_for_message.unlock();
		}
		close(sock);
		kill2 =1;
		cout << "ending the listener_function thread" << endl;
	}
	
	static void collector_function (logger &log) {
		while (1) {
			if (log.kill2 == 1) {
				cout << "killing collector thread" << endl;
				break;
			}
			cout << "collector_function thread running" << endl;
			this_thread::sleep_for(std::chrono::seconds(1));
			log.mu_for_message.lock();
			// move everything to writing_messages
			if (!incoming_messages.empty()) {
				cout << "inserting the messages inside writing_messages from incoming_messages" << endl;
				writing_messages.insert(writing_messages.end(), incoming_messages.begin(), incoming_messages.end());
				incoming_messages.clear();
			}
			log.mu_for_message.unlock();
		}
		kill3 = 1;
		cout << "ending the collector_function thread" << endl;
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
	
	static void rotator_function (logger &log) {
		while (1) {
			if (log.kill3 == 1) {
				cout << "killing rotator_function thread" << endl;
				break;
			}
			cout << "rotator_function thread running.." << endl;
			this_thread::sleep_for(std::chrono::seconds(2));
		}
		kill4 = 1;
		cout << "ending the rotator_function thread" << endl;
	}
	
	static void killer_function (logger &log) {
		while (1) {
			if (log.kill4 == 1) {
				cout << "killing rotator_function thread" << endl;
				break;
			}
			cout << "killer_function thread running.." << endl;
			this_thread::sleep_for(std::chrono::seconds(2));
		}
		for (auto it1 = log.writehandle.begin(); it1 != log.writehandle.end(); it1++) {
			it.close();
		}
		// close socket writing
		cout << "ending the killer_function thread" << endl;
		
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
