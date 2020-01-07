/*
	creator: Isaiah Barron
	class: CMPE156
	date: 3/6/19
	file: server.c
*/

//includes 
#include "server.h"

int main(int argc, char *argv[])
{
	int listenfd, acceptfd, optval;
	struct sockaddr_in servaddr, cliaddr;
	socklen_t clisize;
	pthread_t t_id;

	if(argc != 2)
	{
		fprintf(stderr, "Not enough arguments for program %s: %s\n", argv[0], strerror(errno));
		exit(1);
	}

	serv_port = atoi(argv[1]);
	optval = 1;
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);
	Setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void *) &optval, sizeof(optval));

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(argv[1]));
	clisize = sizeof(cliaddr);

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));
	Listen(listenfd, 1024);

	//create wait_list
	wait_list = newList();

	//main server loop
	for(;;)
	{
		acceptfd = Accept(listenfd, (struct sockaddr *) &cliaddr, &clisize);
		fprintf(stderr, "Client connected to server\n");

		int *sd = malloc(sizeof(int));
		*sd = acceptfd;

		cli_info *client = (cli_info *) malloc(sizeof(cli_info));

		//get the connected clients ip address
		struct sockaddr_in* cli_sockaddr = (struct sockaddr_in*)&cliaddr;
		struct in_addr cli_IP_addr = cli_sockaddr->sin_addr;
		char *temp_ip = (char *) malloc(sizeof(INET_ADDRSTRLEN));
		inet_ntop( AF_INET, &cli_IP_addr, temp_ip, INET_ADDRSTRLEN);
		client->ip_addr = temp_ip;
		client->connfd = *sd;

		//get client settings
		cli_settings(client);

		Pthread_create(&t_id, NULL, mediator_server, (void *)client);
	}

	exit(0);
}

/* 
 * Function that is called by pthread_create.
 * 
 * This function acts as the middle man between clients
 * 
 */
void *mediator_server(void *args)
{
	Pthread_detach(pthread_self());
	cli_info *cli_info_copy = (cli_info *) args;

	//main functionality of the server program
	service_client(cli_info_copy);

	fprintf(stderr, "closing connection\n");
	close(cli_info_copy->connfd);
	free(cli_info_copy);
	pthread_exit(NULL);
}

/* 
 * Function that is called within mediator_server
 * 
 * This function does all the heavy lifting and service each client 
 * that connects to the server
 * 
 */
void service_client(struct cli_info *cli)
{
	char cmd[BUF_SIZE];
	int n = 0;
	int flag = 0;
	char *name_user, *save_ptr;
	char send_info[BUF_SIZE];

	while(1)
	{
		Node N = malloc(sizeof(NodeObj));
		N = NULL;

		//get commands
		bzero(&cmd, sizeof(cmd));
		n = Serv_Recv(cli->connfd, cmd, BUF_SIZE, 0);
		cmd[n] = '\0';
		fprintf(stderr, "command %s\n", cmd);

		if(strcmp(cmd, "/list") == 0)
		{
			pthread_mutex_lock(&waitlist_mutex);
			//send wait list to client
			if(length(wait_list) != 0)
				send_waitlist(cli->connfd, wait_list);
			else
				Serv_Send(cli->connfd, "Empty List", BUF_SIZE, 0);
			pthread_mutex_unlock(&waitlist_mutex);
		}
		else if(strcmp(cmd, "/wait") == 0)
		{
			//insert client into wait list
			pthread_mutex_lock(&waitlist_mutex);
			insert_sorted(wait_list, cli->ip_addr, cli->port_num, cli->user, &flag);
			pthread_mutex_unlock(&waitlist_mutex);
			
			//checks to see if client is already in wait list
			if(flag == 1)
				Serv_Send(cli->connfd, "User name already exists. Use /quit and pick a new user name\n", BUF_SIZE, 0);
			else
				Serv_Send(cli->connfd, "*ignore", BUF_SIZE, 0);
		}
		else if(strstr(cmd, "/connect") != NULL)
		{
			bzero(&send_info, sizeof(send_info));

			name_user = strtok_r(cmd, " ", &save_ptr);
			name_user = strtok_r(NULL, " ", &save_ptr);

			pthread_mutex_lock(&waitlist_mutex);
			N = search(name_user, wait_list);
			pthread_mutex_unlock(&waitlist_mutex);

			if(N == NULL)
				Serv_Send(cli->connfd, "User does not exist\n", BUF_SIZE, 0);
			else
			{
				//send ip, user, port of client in list
				strcat(send_info, N->cli_data.ip);
				strcat(send_info, " ");
				strcat(send_info, N->cli_data.port);
				strcat(send_info, " ");
				strcat(send_info, N->cli_data.u_id);
				Serv_Send(cli->connfd, send_info, BUF_SIZE, 0);

				break;
			}
		}
		else if(strcmp(cmd, "/quit") == 0)
		{
			//delete client from wait list
			pthread_mutex_lock(&waitlist_mutex);
			N = search(cli->user, wait_list);
			if(N != NULL)
				delete(wait_list);
			pthread_mutex_unlock(&waitlist_mutex);
			break;
		}
	}
	return;
}


/*
 * Function that searches the wait list by user name
 * returns the data associated with the user name
 *
 */
Node search(char *name, List L)
{
	char *temp;

	moveFront(L);
	while(L->cursor != NULL)
	{
		temp = get_user(L);
		if(strcmp(temp, name) != 0)
			moveNext(L);
		else
			return L->cursor;
	}
	return NULL;
}

/*
 * Function that gets the wait list.
 *
 * This function converts the linked list into an array
 * and sends it to a client
 *
 */
void send_waitlist(int sock, List L)
{
	List temp = copyList(L);
	char *name;
	char buf[BUF_SIZE];
	bzero(&buf, sizeof(buf));
	char *num;
	int i = 1;

	moveFront(temp);
	while(temp->cursor != NULL)
	{
		name = get_user(temp);
		sprintf(num, "%d", i);
		strcat(buf, num);
		strcat(buf, ") ");
		strcat(buf, name);
		strcat(buf, "\n");
		i++;
		moveNext(temp);
	}
	Serv_Send(sock, buf, BUF_SIZE, 0);
	return;
}

/*
 * Function that is called in int main()
 * 
 * Once the client connects to the server, before creating a new thread,
 * server gets all the clients information (user name, ip, and port)
 *
 */
void cli_settings(struct cli_info *set)
{
	int temp_port;
	char *cli_port = malloc(10);
	char buf[BUF_SIZE];
	char *name = malloc(100);
	int n = 0;
	int i = 0;
	int flag = 1;

	//port num client will communicate on
	temp_port = RNG_portnum();

	sprintf(cli_port, "%d", temp_port);
	Serv_Send(set->connfd, cli_port, strlen(cli_port), 0);

	//recv username from client
	n = Serv_Recv(set->connfd, buf, BUF_SIZE, 0);
	buf[n] = '\0';
	strcpy(name, buf);

	//client settings
	set->user = name;
	set->port_num = cli_port;

	return;
}

/*
 * Function that generates a random port number 
 * for the client to communicate on with other clients
 *
 */
int RNG_portnum()
{
	int rng_port = (rand() % (UPPER - LOWER + num_inc)) + LOWER;
	num_inc++;
	int track = 1;

	while(track)
	{
		//make sure client port is not the same as the servers port
		if(rng_port == serv_port)
			rng_port = (rand() % (UPPER - LOWER + 1)) + LOWER;
		else
			track = 0;
	}
	return rng_port;
}