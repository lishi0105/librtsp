#if defined(__WIN32__) || defined(_WIN32)
#include <WinSock2.h>
#include <Windows.h>
#include <ws2tcpip.h>
#include <stdint.h>
#pragma comment(lib,"ws2_32.lib")
#define closeSocket closesocket
typedef unsigned in_addr_t;
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/time.h>
#include <sys/select.h>
#define closeSocket close
#endif


in_addr_t ourIPAddress();
int setupDatagramSocket(uint16_t);
uint8_t socketJoinGroup(int socket, uint32_t groupAddress);
uint8_t writeSocket(int socket, struct in_addr address, uint32_t port,
					uint8_t ttlArg, unsigned char *buffer, unsigned bufferSize);
int readSocket(int socket, unsigned char *buffer, unsigned bufferSize,
struct sockaddr_in *fromAddress, struct timeval *timeout);
int blockUntilReadable(int socket, struct timeval* timeout);
uint8_t badAddress(uint32_t addr);
uint8_t socketLeaveGroup(int socket, uint32_t groupAddress);
uint8_t IsMulticastAddress(uint32_t address);

#define SET_SOCKADDR_SIN_LEN(var)
#define MAKE_SOCKADDR_IN(var,adr,prt) /*adr,prt must be in network order*/\
struct sockaddr_in var;\
	var.sin_family = AF_INET;\
	var.sin_addr.s_addr = (adr);\
	var.sin_port = (prt);\
	SET_SOCKADDR_SIN_LEN(var);

int getOurIpAddr(char *ipaddr) {
	struct sockaddr_in ourAddress;
	if(ipaddr==NULL)return -1;
	ourAddress.sin_addr.s_addr = ourIPAddress();

	if (ourAddress.sin_addr.s_addr == 0) {
		printf("get local ip error!\n");
		getchar();
		return -1;
	}

	strcpy(ipaddr,inet_ntoa(ourAddress.sin_addr));
	return 0;
}

in_addr_t ourIPAddress() {
	uint32_t ourAddress = 0;
	int sock = -1;
	struct in_addr testAddr;
	struct sockaddr_in fromAddr;
	fromAddr.sin_addr.s_addr = 0;

	// Get our address by sending a (0-TTL) multicast packet,
	// receiving it, and looking at the source address used.
	// (This is kinda bogus, but it provides the best guarantee
	// that other nodes will think our address is the same as we do.)
	testAddr.s_addr = inet_addr("228.67.43.92");
	uint16_t testPort = 15948;
	sock = setupDatagramSocket(testPort);
	if (sock < 0) {
		return 0;
	}

	if (!socketJoinGroup(sock, testAddr.s_addr)) {
		return 0;
	}

	unsigned char testString[] = "hostIdTest";
	unsigned testStrLength = sizeof(testString);
	if (!writeSocket(sock, testAddr, testPort, 0, testString, testStrLength)) {
		return 0;
	}

	unsigned char readBuffer[20];
	struct timeval timeout;
	timeout.tv_sec = 5;
	timeout.tv_usec = 0;

	int bytesRead = readSocket(sock, readBuffer, sizeof(readBuffer), &fromAddr,
		&timeout);
	if (bytesRead == 0 // timeout occurred
		|| bytesRead != (int) testStrLength || strncmp(
		(char *) readBuffer, (char *) testString, testStrLength) != 0) {
			printf("read socket error\n");
			return 0;
	}

	uint32_t from = fromAddr.sin_addr.s_addr;
	if (badAddress(from)) {
		printf("This computer has an invalid IP address: 0x%x\n",
			(uint32_t) (ntohl(from)));
		from = 0;
	}

	ourAddress = from;

	if (sock >= 0) {
		socketLeaveGroup(sock, testAddr.s_addr);
		closeSocket(sock);
	}

	return ourAddress;
}

int setupDatagramSocket(uint16_t port) {
	int reuseFlag = 1;
	int newSocket = socket(AF_INET, SOCK_DGRAM, 0);
	const uint8_t loop = 1;

	if (newSocket < 0) {
		printf("unable to create datagram socket\n");
		return newSocket;
	}

	if (setsockopt(newSocket, SOL_SOCKET, SO_REUSEADDR,
		(const char *) &reuseFlag, sizeof(reuseFlag)) < 0) {
			printf("setsockopt(SO_REUSEADDR) error!\n");
			closeSocket(newSocket);
			return -1;
	}

	if (setsockopt(newSocket, IPPROTO_IP, IP_MULTICAST_LOOP,
		(const char*) &loop, sizeof(loop)) < 0) {
			printf("setsockopt(IP_MULTICAST_LOOP) error!\n");
			closeSocket(newSocket);
			return -1;
	}

	uint32_t addr = INADDR_ANY;
	MAKE_SOCKADDR_IN(name, addr, port);

	if (bind(newSocket, (struct sockaddr *) &name, sizeof(name)) != 0) {
		printf("bind() error (port number: %d)!\n", ntohs(port));
		closeSocket(newSocket);
		return -1;
	}

	return newSocket;
}

uint8_t socketJoinGroup(int socket, uint32_t groupAddress) {
	if (!IsMulticastAddress(groupAddress)) {
		return 1; // ignore this case
	}

	struct ip_mreq imr;
	imr.imr_multiaddr.s_addr = groupAddress;
	imr.imr_interface.s_addr = INADDR_ANY;
	if (setsockopt(socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char *) &imr,
		sizeof(struct ip_mreq)) < 0) {
			printf("setsockopt(IP_ADD_MEMBERSHIP) error!\n");
			perror("setsockopt");
			return 0;
	}

	return 1;
}


uint8_t writeSocket(int socket, struct in_addr address, uint32_t port,
					uint8_t ttlArg, unsigned char *buffer, unsigned bufferSize) {
	if (ttlArg != 0) {
		// Before sending, set the socket's TTL:
		uint8_t ttl = ttlArg;
		if (setsockopt(socket, IPPROTO_IP, IP_MULTICAST_TTL,
			(const char *) &ttl, sizeof(ttl)) < 0) {
				printf("setsockopt(IP_MULTICAST_TTL) error!");
				return 0;
		}
	}
	MAKE_SOCKADDR_IN(dest, address.s_addr, port);
	int bytesSent = sendto(socket, (char *) buffer, bufferSize, 0,
		(struct sockaddr *) &dest, sizeof(dest));
	if (bytesSent != (int) bufferSize) {
		printf("writeSocket(%d), sendTo() error: wrote " 
			"%d bytes instead of %u!\n", socket, bytesSent, bufferSize);
		return 0;
	}

	return 1;
}

int readSocket(int socket, unsigned char *buffer, unsigned int bufferSize,
struct sockaddr_in *fromAddress, struct timeval *timeout) {
	int bytesRead = -1;

	int result = blockUntilReadable(socket, timeout);
	if (timeout != NULL && result == 0) {
		bytesRead = 0;
		return -1;
	} else if (result <= 0) {
		return -1;
	}

	socklen_t addressSize = (socklen_t) sizeof(struct sockaddr_in);
	bytesRead = recvfrom(socket, (char *) buffer, bufferSize, 0,
		(struct sockaddr *) fromAddress, &addressSize);
	if (bytesRead < 0) {
		printf("recvfrom() error!\n");
		return -1;
	}

	return bytesRead;
}

int blockUntilReadable(int socket, struct timeval *timeout) {
	int result = -1;
	fd_set rd_set;
	FD_ZERO(&rd_set);
	if (socket < 0)
		return -1;
	FD_SET((unsigned) socket, &rd_set);
	const unsigned numFds = socket + 1;

	result = select(numFds, &rd_set, NULL, NULL, timeout);
	if (timeout != NULL && result == 0) {
		printf("select timeout!\n");
		return -1; // this is OK - timeout occurred
	} else if (result <= 0) {
		printf("select() error!\n");
		return -1;
	}

	if (!FD_ISSET(socket, &rd_set)) {
		printf("select() error - !FD_ISSET!\n");
		return -1;
	}

	return result;
}

uint8_t badAddress(uint32_t addr) {
	// Check for some possible erroneous addresses:
	uint32_t hAddr = ntohl(addr);
	return (hAddr == 0x7F000001 /* 127.0.0.1 */ 
		|| hAddr == 0 || hAddr == (uint32_t) (~0));
}

uint8_t socketLeaveGroup(int socket, uint32_t groupAddress) {
	if (!IsMulticastAddress(groupAddress))
		return 1; // ignore this case

	struct ip_mreq imr;
	imr.imr_multiaddr.s_addr = groupAddress;
	imr.imr_interface.s_addr = INADDR_ANY;
	if (setsockopt(socket, IPPROTO_IP, IP_DROP_MEMBERSHIP, (const char *) &imr,
		sizeof(struct ip_mreq)) < 0) {
			return 0;
	}

	return 1;
}

uint8_t IsMulticastAddress(uint32_t address) {
	// Note: We return False for addresses in the range 224.0.0.0
	// through 224.0.0.255, because these are non-routable
	// Note: IPv4-specific #####
	uint32_t addressInHostOrder = ntohl(address);
	return addressInHostOrder > 0xE00000FF && addressInHostOrder <= 0xEFFFFFFF;
}
