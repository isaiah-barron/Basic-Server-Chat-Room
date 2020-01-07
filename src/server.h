/*
	creator: Isaiah Barron
	class: CMPE156
	date: 3/12/19
	file: server.h
*/

#ifndef SERVER_H
#define SERVER_H

//includes 
#include "wrapnetwork.h"
#include "LinkedList.h"

//defines
#define SA const struct sockaddr
#define UPPER 65535
#define LOWER 5000
#define BUF_SIZE 512

//enum
typedef enum{
	START, LIST, WAIT, CONNECT
}state_machine;

//structs
typedef struct cli_info{
	int connfd;
	struct sockaddr_in chat_cliaddr;
	char *ip_addr;
	char *user;
	char *port_num;
}cli_info;

//global variables
List wait_list;
int serv_port;
pthread_mutex_t waitlist_mutex;
int num_inc = 0;


/* 
 * Function that is called by pthread_create.
 * 
 * This function acts as the middle man between clients
 * 
 */
void *mediator_server(void *socket);

/*
 * Function that searches the wait list by user name
 * returns the data associated with the user name
 *
 */
Node search(char *name, List L);

/* 
 * Function that is called within mediator_server
 * 
 * This function does all the heavy lifting and service each client 
 * that connects to the server
 * 
 */
void service_client(struct cli_info *cli);

/*
 * Function that is called in int main()
 * 
 * Once the client connects to the server, before creating a new thread,
 * server gets all the clients information (user name, ip, and port)
 *
 */
void cli_settings(struct cli_info *set);

/*
 * Function that gets the wait list.
 *
 * This function converts the linked list into an array
 * and sends it to a client
 *
 */
void send_waitlist(int sock, List L);


/*
 * Function that generates a random port number 
 * for the client to communicate on with other clients
 *
 */
int RNG_portnum();

#endif /* SERVER_H*/

