/*
	creator: Isaiah Barron
    class: CMPE156
    date: 2/3/19
    file: wrapsock.h
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#ifndef WRAPSOCK_H
#define WRAPSOCK_H

/* Wrapper functions for Unix Sockets */
int Socket(int domain, int type, int protocol);
void Connect(int socket, const struct sockaddr *address, socklen_t address_len);
int Serv_Connect(int socket, const struct sockaddr *address, socklen_t address_len);
void Bind(int socket, const struct sockaddr *address, socklen_t address_len);
void Listen(int socket, int backlog);
int Accept(int socket, struct sockaddr *restrict address, socklen_t *restrict address_len);
void Setsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len);
void Inet_pton(int af, const char *src, void *dst);

/* Wrapper functions for Unix I/O */ 
void Send(int socket, const void *buffer, size_t length, int flags);
size_t Recv(int socket, void *buffer, size_t length, int flags);
void Serv_Send(int socket, const void *buffer, size_t length, int flags);
size_t Serv_Recv(int socket, void *buffer, size_t length, int flags);

/* Wrapper funcitons for POSIX Pthread funcitons */
void Pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(* start_routine)(void*), void *arg);
void Pthread_detach(pthread_t thread);

/* Wrapper functions for file I/O functions */
FILE *Fopen(char *name, char *optval);

#endif