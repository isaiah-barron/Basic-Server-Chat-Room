/*
	creator: Isaiah Barron
	class: CMPE156
	date: 3/7/19
	file: client.c
*/

//includes
#include "client.h"

int main(int argc, char *argv[])
{
	char buf[BUF_SIZE];
	int n = 0;

	//signal handler stuff
	progname = *(argv);
	init_signals();

	ip_main = argv[1];
	port_main = argv[2];
	strcat(client_name, argv[3]);

	//for client to connect to chat_server
	conn_to_mainserv(ip_main, port_main);

	//recieve port number that client will communicate on
	n = Recv(info.socket, buf, BUF_SIZE, 0);
	buf[n] = '\0';
	cli_serv.port = atoi(buf);
	
	//send username to server
	bzero(&buf, sizeof(buf));
	strcpy(buf, argv[3]);
	Send(info.socket, buf, BUF_SIZE, 0);

	//configure client to be a server
	connfig_cli_server();

	//state machine for client application
	state_machine(argv[3]);

	exit(0);
}


void init_signals(void)
{
	sigact.sa_handler = cnt_c_handler;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = 0;
	sigaction(SIGINT, &sigact, (struct sigaction *)NULL);
	return;
}


void cnt_c_handler(int sig)
{
	if(sig == SIGINT)
		sigint_doit();
	return;
}


void sigint_doit()
{
	if(client.state == CHAT)
	{
		rl_free_line_state(); //frees any partial sate asoocited with the current input line
	    rl_cleanup_after_signal(); //resets the terminal to the state before readline() was called
	    rl_replace_line("",0); //clears the current prompt
	    rl_crlf(); //moves the cursor to the next line
	    rl_redisplay();
	    sigint_flag = 1;
	}
	return;
}


void state_machine(char *user)
{
	char *line = (char *)malloc(100);
	char buf[BUF_SIZE];
	bzero(&prompt_name, sizeof(prompt_name));
	prompt_name = user;
	strcat(prompt_name, "> ");
	int num = 1;
	int flag = 0;
	char *ip_address, *port;
	int flag_thrds = 1;
	int n = 0;

	while(1)
	{
		switch(client.state)
		{
			case INFO:
			{
				//recv port to communicate and send user name
				//if cnt^c or quit occured
				if(cntC_flag || recvquit_flag)
				{
					//recieve port number that client will communicate on
					n = Recv(info.socket, buf, BUF_SIZE, 0);
					buf[n] = '\0';
					cli_serv.port = atoi(buf);
		
					//send username to server
					bzero(&buf, sizeof(buf));
					Send(info.socket, client_name, BUF_SIZE, 0);

					//configure client to be a server
					connfig_cli_server();
				}

				//reset important state
				bzero(&line, sizeof(line));
				bzero(&buf, sizeof(buf));
				flag_thrds = 1;
				num = 1;
				flag = 0;
				sendquit_flag = 0;
				recvquit_flag = 0;
				flag_conn = 0;
				flag_read = 0;
				cntC_flag = 0;

				while((line = readline(prompt_name)) != NULL)
				{
					bzero(&buf, sizeof(buf));
					if(strstr(line, "/") != NULL)
					{
						if(strcmp(line, "/list") == 0)
						{
							//get list from server
							bzero(&buf, sizeof(buf));
							Send(info.socket, line, strlen(line), 0);//send list cmd
							Recv(info.socket, buf, BUF_SIZE, 0);//recv list

							if(strcmp(buf, "Empty List") == 0)
								fprintf(stderr, "%s\n", buf);
							else
								fprintf(stdout, buf);
							break;
						}
						else if(strcmp(line, "/wait") == 0)
						{
							Send(info.socket, line, strlen(line), 0);
							Recv(info.socket, buf, BUF_SIZE, 0);
							client.state = WAIT;
							break;
						}
						else if(strcmp(line, "/quit") == 0)
						{
							Send(info.socket, line, strlen(line), 0);
							close(info.socket);
							exit(0);
						}
						else if(strstr(line, "/connect") != NULL)
						{
							Send(info.socket, line, strlen(line), 0);

							//connect to a client
							Recv(info.socket, buf, BUF_SIZE, 0);
							if(strstr(buf, "User") != NULL)//user not on list
								fprintf(stderr, buf);
							else
							{
								//get ip and port
								ip_address = strtok(buf, " ");
								port = strtok(NULL, " ");
								conn_user_id = strtok(NULL, " ");

								//config settings to connect to client
								conn_to_cli(ip_address, port);
								fprintf(stderr, "Connected to %s\n", conn_user_id);

								//send client your user name
								Send(client.clientfd, client_name, strlen(client_name), 0);

								close(info.socket);
								client.prev_state = INFO;
								client.state = CHAT;
								break;
							}
						}
						else
							fprintf(stderr, "Command %s not recognized\n", line);
					}
					else
						fprintf(stderr, "Command %s not recognized\n", line);

					free(line);
				}
				break;
			}
			case WAIT:
			{
				if(strstr(buf, "/") != NULL)//user name already in wait list
				{
					fprintf(stderr, "%s", buf);
					client.state = INFO;
					break;
				}
				else
				{
					//wait until someone connects to you
					fprintf(stderr, "waiting for connection\n");
					bzero(&line, sizeof(line));

					//create seperate threads
					Pthread_create(&t_id_r, NULL, read_line, NULL); /*reads input*/
					Pthread_create(&t_id_c, NULL, accept_conn, NULL); /*waits for connectiong*/

					while(1)
					{
						if(flag_conn)
						{
							pthread_cancel(t_id_r);
							break;
						}
						if(flag_read)
						{
							close(info.socket);
							exit(0);
						}
					}

					//send quit message to server after someone connects to you, to be removed from wait list
					line = "/quit";
					Send(info.socket, line, strlen(line), 0);
					close(info.socket);
					client.prev_state = WAIT;
					client.state = CHAT;
				}
				break;
			}
			case CHAT:
			{	
				//someone either connected to you, or you connected to someone
				if(flag_thrds == 1)
				{
					Pthread_create(&t_id_snd, NULL, send_chat, NULL);
					Pthread_create(&t_id_rcv, NULL, recv_chat, NULL);
					flag_thrds = 0;
				}
				if(sendquit_flag == 1)
					exit(0);
				if(recvquit_flag == 1)
				{
					client.state = INFO;
					conn_to_mainserv(ip_main, port_main);
					break;
				}
				if(cntC_flag == 1)
				{
					pthread_cancel(t_id_snd);
					pthread_cancel(t_id_rcv);
					client.state = INFO;
					conn_to_mainserv(ip_main, port_main);
					break;
				}

				client.state = CHAT; 
				break;
			}
		}
	}
	return;
}


void *send_chat(void *args)
{
	Pthread_detach(pthread_self());

	char *line = (char *)malloc(MSG_SIZE);
	char buffer[MSG_SIZE];
	bzero(&buffer, sizeof(buffer));

	if(client.prev_state == WAIT)//client is server
	{
		//communicate on cli_serv.connfd
		while((line = readline(prompt_name)) != NULL)
		{
			bzero(&buffer, sizeof(buffer));
			if(sigint_flag)
			{
				Serv_Send(cli_serv.connfd, "/quit", BUF_SIZE, 0);
				cntC_flag = 1;
				close(cli_serv.connfd);
				sigint_flag = 0;
				break;
			}
			if(strcmp(line, "/quit") == 0)
			{
				//quit program
				fprintf(stderr, "leaving conversation with %s\n", conn_user_id);
				Serv_Send(cli_serv.connfd, "/quit", BUF_SIZE, 0);
				sendquit_flag = 1;
				pthread_cancel(t_id_rcv);
				close(cli_serv.connfd);
				break;
			}
			strcat(buffer, line);
			Serv_Send(cli_serv.connfd, buffer, MSG_SIZE, 0);
		}
	}
	else if(client.prev_state == INFO)//client is client
	{
		//communicate on client.clientfd
		while((line = readline(prompt_name)) != NULL)
		{
			bzero(&buffer, sizeof(buffer));
			if(sigint_flag)
			{
				Serv_Send(client.clientfd, "/quit", BUF_SIZE, 0);
				cntC_flag = 1;
				close(client.clientfd);
				sigint_flag = 0;
				break;
			}
			if(strcmp(line, "/quit") == 0)
			{
				//quit program
				fprintf(stderr, "leaving conversation with %s\n", conn_user_id);
				Send(client.clientfd, "/quit", MSG_SIZE, 0);
				sendquit_flag = 1;
				pthread_cancel(t_id_rcv);
				close(client.clientfd);
				break;
			}
			strcat(buffer, line);
			Send(client.clientfd, buffer, MSG_SIZE, 0);
		}
	}
	pthread_exit(NULL);
}


void *recv_chat(void *args)
{
	Pthread_detach(pthread_self());

	char buffer[MSG_SIZE];
	bzero(&buffer, sizeof(buffer));
	int n = 0;

	if(client.prev_state == WAIT)//client is server
	{
		//keep receiving until cnt^c or quit
		while(strcmp(buffer, "/quit") != 0)
		{
			bzero(&buffer, sizeof(buffer));
			n = Serv_Recv(cli_serv.connfd, buffer, MSG_SIZE, 0);
			if(strcmp(buffer, "/quit") == 0)
				break;
			if(n > 0)
				fprintf(stdout,"\n%s: %s\n", recv_name, buffer);
		}

		fprintf(stderr, "%s left conversation\n", recv_name);
		close(cli_serv.connfd);
	}
	else if(client.prev_state == INFO)//client is client
	{
		//keep receiving until cnt^c or quit
		while(strcmp(buffer, "/quit") != 0)
		{
			bzero(&buffer, sizeof(buffer));
			n = Serv_Recv(client.clientfd, buffer, MSG_SIZE, 0);
			if(strcmp(buffer, "/quit") == 0)
				break;
			if(n > 0)
				fprintf(stdout,"\n%s: %s\n", conn_user_id, buffer);
		}

		fprintf(stderr, "%s left conversation\n", conn_user_id);
		close(client.clientfd);
	}
	recvquit_flag = 1;
	pthread_cancel(t_id_snd);
	pthread_exit(NULL);
}


void *accept_conn(void *args)
{
	bzero(&recv_name, sizeof(recv_name));
	cli_serv.clisize = sizeof(cli_serv.cliaddr);
	cli_serv.connfd = Accept(cli_serv.listenfd, (struct sockaddr *)&cli_serv.cliaddr, &cli_serv.clisize);

	//receive user name of connected client
	bzero(&recv_name, sizeof(recv_name));
	Serv_Recv(cli_serv.connfd, recv_name, BUF_SIZE, 0);
	fprintf(stderr, "\nConnection from %s\n", recv_name);

	flag_conn = 1;
	pthread_exit(NULL);
}


void *read_line(void *args)
{
	char *line;

	while((line = readline(prompt_name)) != NULL)
	{
		if(strcmp(line, "/quit") == 0)
		{
			Send(info.socket, line, strlen(line), 0);
			flag_read = 1;
			pthread_exit(NULL);
		}
		else
			fprintf(stderr, "Only the command quit can be used while waiting\n");
	}
}


void connfig_cli_server()
{
	cli_serv.optval = 1;
	cli_serv.listenfd = Socket(AF_INET, SOCK_STREAM, 0);
	Setsockopt(cli_serv.listenfd, SOL_SOCKET, SO_REUSEADDR, (const void *) &cli_serv.optval, sizeof(cli_serv.optval));

	bzero(&cli_serv.cli_servaddr, sizeof(cli_serv.cli_servaddr));
	cli_serv.cli_servaddr.sin_family = AF_INET;
	cli_serv.cli_servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	cli_serv.cli_servaddr.sin_port = htons(cli_serv.port);

	Bind(cli_serv.listenfd, (SA *)&cli_serv.cli_servaddr, sizeof(cli_serv.cli_servaddr));
	Listen(cli_serv.listenfd, 1024);

	return;
}


void conn_to_mainserv(char *ip, char *port)
{
	//for client to connect to chat_server
	info.socket = Socket(AF_INET, SOCK_STREAM, 0);
	bzero(&info.servaddr, sizeof(info.servaddr));
	info.servaddr.sin_family = AF_INET;
	info.servaddr.sin_port = htons(atoi(port));
	Inet_pton(AF_INET, ip, &info.servaddr.sin_addr);
	Connect(info.socket, (SA *) &info.servaddr, sizeof(info.servaddr));

	return;
}


void conn_to_cli(char *ip, char *port)
{
	client.clientfd = Socket(AF_INET, SOCK_STREAM, 0);
	bzero(&client.conn_to_cliaddr, sizeof(client.conn_to_cliaddr));
	client.conn_to_cliaddr.sin_family = AF_INET;
	client.conn_to_cliaddr.sin_port = htons(atoi(port));
	Inet_pton(AF_INET, ip, &client.conn_to_cliaddr.sin_addr);
	Connect(client.clientfd, (SA *) &client.conn_to_cliaddr, sizeof(client.conn_to_cliaddr));

	return;
}