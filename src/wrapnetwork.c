/*
	creator: Isaiah Barron
    class: CMPE156
    date: 2/3/19
    file: wrapsock.c
*/

//includes
#include "wrapnetwork.h"


/******************************* Wrapper functions for Unix Sockets *******************************/

int Socket(int domain, int type, int protocol)
{
	int sock;

	if((sock = socket(domain, type, protocol)) < 0)
	{
		printf("Socket error: %s\n", strerror(errno));
		exit(0);
	}

	return sock;
}


void Connect(int socket, const struct sockaddr *address, socklen_t address_len)
{
	if(connect(socket, address, address_len) < 0)
	{
		printf("Connection error: %s\n", strerror(errno));
		exit(0);
	}
}

int Serv_Connect(int socket, const struct sockaddr *address, socklen_t address_len)
{
	if(connect(socket, address, address_len) < 0)
		return -1;

	return 1;
}


void Bind(int socket, const struct sockaddr *address, socklen_t address_len)
{
	if((bind(socket, address, address_len)) < 0)
		printf("bind error: %s\n", strerror(errno));
}


void Listen(int socket, int backlog)
{
	if((listen(socket, backlog)) < 0)
		printf("listen error: %s\n", strerror(errno));
}


int Accept(int socket, struct sockaddr *restrict address, socklen_t *restrict address_len)
{
	int acceptSock;

	if((acceptSock = accept(socket, address, address_len)) < 0)
		printf("accept error: %s\n", strerror(errno));

	return acceptSock;
}


void Setsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len)
{
	if((setsockopt(socket, level, option_name, option_value, option_len)) < 0)
		printf("setsockopt() failed: %s\n", strerror(errno));

}

void Inet_pton(int af, const char *src, void *dst)
{
	if(inet_pton(af, src, dst) <= 0)
	{
		printf("inet_pton failed: %s\n", strerror(errno));
		exit(0);
	}
}

/************************ Wrapper functions for Unix I/O ******************************/ 

void Send(int socket, const void *buffer, size_t length, int flags)
{
	int snd;

	if((snd = send(socket, buffer, length, flags)) < 0)
	{
		printf("Can't send to socket: %s\n", strerror(errno));
		exit(0);
	}
}


size_t Recv(int socket, void *buffer, size_t length, int flags)
{
	int rcv;

	if((rcv = recv(socket, buffer, length, flags)) < 0)
	{
		printf("Can't recv from socket: %s\n", strerror(errno));
		exit(0);
	}

	return rcv;
}


void Serv_Send(int socket, const void *buffer, size_t length, int flags)
{
	int snd;

	if((snd = send(socket, buffer, length, flags)) < 0)
	{
		printf("Can't send to socket: %s\n", strerror(errno));
		return;
	}
}

size_t Serv_Recv(int socket, void *buffer, size_t length, int flags)
{
	int rcv;

	if((rcv = recv(socket, buffer, length, flags)) < 0)
	{
		printf("Can't recv from socket: %s\n", strerror(errno));
		return -1;
	}

	return rcv;
}

/* Wrapper funcitons for POSIX Pthread funcitons */

void Pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(* start_routine)(void*), void *arg)
{
	if((pthread_create(thread, attr, start_routine, arg)) < 0)
		printf("failed to create a thread: %s\n", strerror(errno));
}


void Pthread_detach(pthread_t thread)
{
	if((pthread_detach(thread)) != 0)
		printf("pthread_detach failed: %s\n", strerror(errno));
}

/* Wrapper functions for file I/O functions */
FILE *Fopen(char *name, char *optval)
{
	FILE *fp;

	fp = fopen(name, optval);
	if(fp == NULL)
	{
		printf("failed to open file %s : %s\n", name, strerror(errno));
		return NULL;
	}
	return fp;
}