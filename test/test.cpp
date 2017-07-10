#include <iostream>
#include <stdlib.h>
#include <string>

#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>

using namespace std;

int main (int argc, char *argv[]) {
	int portno = atoi(argv[2]);
	cout << "port = " << portno << " hostname = " << argv[1] << endl;

	int sockfd, newsockfd; // for server socket
	char buffer[256]; // for reading and writing
	struct sockaddr_in serv_addr;
	struct hostent *server;
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("ERROR opening socket");
		exit(1);
	}
	server = gethostbyname(argv[1]);
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);
	/* Now connect to the server */
	if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
		perror("ERROR connecting");
		exit(1);
	}
	/* Now ask for a message from the user, this message
	* will be read by server
	*/
	int n;
	char buffer[256];
	while (1) {
		bzero(buffer,256);
		printf("Client message: Please enter command to be passed to server:");
		n = write(sockfd,buffer,strlen(buffer));
		if (n < 0) {
			perror("ERROR writing to socket");
			exit(1);
		}
	}
	close(sockfd);
	return 0;
}
