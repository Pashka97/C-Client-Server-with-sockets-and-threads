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

using namespace std;

#define NUM_THREADS 5

int REPETITIONS = 0;

void * connection_thread(void*);

int main(int argc, char** argv){
	//pthread_t threads[NUM_THREADS];
	//cout << "Port value: " << connectionPort << endl;

	string temp = argv[1];
	int port = atoi(temp.c_str()); // last 4 digits of student id
	cout << "port " << argv[1] << endl;
	sockaddr_in acceptSockAddr;

	temp = argv[2];
	REPETITIONS = atoi(temp.c_str());

	bzero( (char*)&acceptSockAddr, sizeof( acceptSockAddr ) );
	acceptSockAddr.sin_family      = AF_INET; // Address Family Internet
	acceptSockAddr.sin_addr.s_addr = htonl( INADDR_ANY );
	acceptSockAddr.sin_port        = htons( port );


    int serverSd = socket( AF_INET, SOCK_STREAM, 0);


    if(serverSd < 0){
   		cout << "Error establishing connection." << endl;
   		exit(1);
   	}
   	cout << "Server Socket connection created..." << endl;

   	const int on = 1;
   	setsockopt( serverSd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof (int));


   	if((bind( serverSd, ( sockaddr* )&acceptSockAddr, sizeof( acceptSockAddr ) )) < 0){
   		cout << "Error binding connection, the socket has already been established..." << endl;
    	exit(1);
    }

    //int test = bind(serverSd, ( sockaddr* )&acceptSockAddr, sizeof( acceptSockAddr ) );
    //cout << "bind value: " << test << endl;

    cout << "Socket bind successful" << endl;

    listen(serverSd, 1);



	sockaddr_in newSockAddr;
	socklen_t newSockAddrSize = sizeof( newSockAddr );
	//newSd is the new socket for this connection with the client
	int newSd;
	//int i = 0;
	while( (newSd = accept( serverSd, ( sockaddr *)&newSockAddr, &newSockAddrSize ))){
		cout << "Connection successful" << endl;
		pthread_t thread_id;
		//should first param be &threads[i] ?
		pthread_create(&thread_id, NULL, connection_thread, (void*)&newSd);
	}

	close(serverSd);
}

void *connection_thread(void *socket_desc)
{

	int sock = *(int*)socket_desc;
	int BUFSIZE = 1500;
	int count = 0;

	struct timeval start, end;
	long mtime, seconds, useconds;

	char databuf[BUFSIZE];
	gettimeofday(&start, NULL);

	for(int i = 0; i < REPETITIONS; i++){
		for(int nRead = 0;
			(nRead += read(sock, databuf, BUFSIZE - nRead)) < BUFSIZE;
			++count);
	}

	gettimeofday(&end, NULL);

	seconds = end.tv_sec - start.tv_sec;
	useconds = end.tv_usec - start.tv_usec;
	//mtime is now equal to time elapsed
	mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;

	cout << "data-receiving time = " << mtime << " usec" << endl;

	/*
	string delay;
	stringstream mystream;
	mystream << count;
	delay = mystream.str();

	int msgSize = delay.size();
	char msg[msgSize];
	for(int i = 0; i < msgSize; i++){
		msg[i] = delay.at(i);
	}
	*/

	int countToSend = htonl(count);

	write(sock, (const char*) &countToSend, sizeof(countToSend));

	close(sock);
}
