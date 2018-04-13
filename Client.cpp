#include <sys/types.h> //socket,bind
#include <sys/socket.h> // socket, bind, listen, inet_ntoa
#include <netinet/in.h> // htonl, htons, inet_ntoa
#include <arpa/inet.h> // inet_ntoa
#include <netdb.h>	   // gethostbyname
#include <unistd.h>	   // read, write, close
#include <strings.h>   // bzero
#include <netinet/tcp.h>//SO_REUSEADDR
#include <sys/uio.h>	//writev
#include <thread>		//threads
#include <pthread.h>	//pthread
#include <iostream>		//input/output
#include <stdlib.h>		//exit
#include <sys/time.h>	//duration
#include <sstream>		//wow
#include <stdio.h>

using namespace std;

//Takes 6 arguments
/*
 * 1:port: server IP port
 * 2:repetition: the repetition of sending data buffers
 * 3:nbufs: the number of data buffers
 * 4:bufsize: the size of each buffer in (in bytes)
 * 5:serverIP: the server IP name
 * 6:the type of transfer scenario (1,2 or 3)
 * 		1:multiple writes: call write once for each data buffer
 * 		2:writev
 * 		3:single write
 */
int main(int argc, char** argv){



	string temp = argv[1];
	int port = atoi(temp.c_str());

	temp = argv[2];
	int repetitions = atoi(temp.c_str());

	temp = argv[3];
	int nbufs = atoi(temp.c_str());

	temp = argv[4];
	int bufsize = atoi(temp.c_str());

	char * ip =argv[5];

	temp = argv[6];
	int transferType = atoi(temp.c_str());

	char databuf[nbufs][bufsize];

	//IP port and name received from args
	struct hostent* host = gethostbyname( argv[1] );

	sockaddr_in sendSockAddr;
	bzero( (char*)&sendSockAddr, sizeof( sendSockAddr ) );
	sendSockAddr.sin_family      = AF_INET; // Address Family Internet
	sendSockAddr.sin_addr.s_addr =
    inet_addr( inet_ntoa( *(struct in_addr*)*host->h_addr_list ) );
	sendSockAddr.sin_port        = htons( port );

	inet_pton(AF_INET, ip, &sendSockAddr.sin_addr);

	int clientSd = socket( AF_INET, SOCK_STREAM, 0 );

	if(clientSd < 0){
		cout << "Error establishing socket..." << endl;
		exit(1);
	}
	cout << "Socket established successfully!" << endl;

	int con = connect( clientSd, ( sockaddr* )&sendSockAddr, sizeof( sendSockAddr ) );
	if(con < 0){
		cout << "Could not connect to server..." << endl;
		exit(1);
	}
	cout << "Connected to server: " << inet_ntoa(sendSockAddr.sin_addr) << " with port number: " << port << endl;

	cout << "This is the transferTYpe: " <<transferType << endl;
	char * buf = (char *)malloc(sizeof(char) * nbufs * bufsize);

	struct timeval start, end;
	long mtime1, mtime2, seconds, usec;
	gettimeofday(&start, NULL);

	for(int i = 0; i < repetitions; i++){
		//cout << i << endl;

		if(transferType == 1){
			for ( int j = 0; j < nbufs; j++ ){
					write( clientSd, databuf[j], bufsize );    // sd: socket descriptor
					//cout << "This should print 20000 times" << endl;
				}

			}
		else if(transferType == 2) {
				//cout << "We're in Bois + i:" << i << endl;
				struct iovec vector[nbufs];
				int j;
					for(  j = 0; j < nbufs; j++){
						vector[j].iov_base = buf + j * bufsize;
						vector[j].iov_len = bufsize;
					}
					if (writev(clientSd, vector, nbufs) < 0)
						perror("writev");
			}
		else if(transferType == 3){
					write(clientSd, databuf, nbufs * bufsize);
			}
		else{
					cout << "Bad transferType" << endl;
					break;
			}

	}

	gettimeofday(&end, NULL);
	seconds = end.tv_sec - start.tv_sec;
	usec = end.tv_usec - start.tv_usec;
	mtime1 = ((seconds) * 1000 + usec/1000.0) + 0.5;

	int BUFSIZE = 1500;
	char readbuf[BUFSIZE];
	int temp1;
	int temp2 =0;
	//cout << "This is before the read, Hi stephen!" << endl;
	int yes = read(clientSd, &temp1, sizeof(temp1) );
	if(yes < 0){
		cout << "RUt RO" << endl;
	}

	gettimeofday(&end, NULL);
	seconds = end.tv_sec - start.tv_sec;
	usec = end.tv_usec - start.tv_usec;
	mtime2 = ((seconds) * 1000 + usec/1000.0) + 0.5;
	//cout << "This survived the read call...or did we?" << endl;
	temp2 = ntohl(temp1);
	cout << "Type: " << transferType << " Time to send: " << mtime1 << " usec, round-trip time: " << mtime2 << " usec, #reads = " << temp2 << endl;

	close(clientSd);

}
