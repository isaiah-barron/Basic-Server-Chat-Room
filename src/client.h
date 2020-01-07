/*
	creator: Isaiah Barron
	class: CMPE156
	date: 3/16/19
	file: client.c
*/

//includes
#include "wrapnetwork.h"
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
#include <stdarg.h>

//defines
#define SA const struct sockaddr
#define BUF_SIZE 512
#define MSG_SIZE 8124

//enums
typedef enum{
	INFO, WAIT, CHAT
}statemachine;

//structs
typedef struct cli_data{
	statemachine state; /*clients state */
	statemachine prev_state; /*previous state*/
	int clientfd; /*socket it will communicate on between connected client*/
	struct sockaddr_in conn_to_cliaddr;
}cli_data;

typedef struct cli_serv_data{
	struct sockaddr_in cliaddr; /*cliaddr for clients that connect*/ 
	struct sockaddr_in cli_servaddr; /*cli_servaddr for client to act as server*/
	int optval;
	socklen_t clisize;
	int port; /*port to communicate on*/
	int listenfd, connfd; /*to bind, listen, and accept new connections*/
}cli_serv_data;

typedef struct main_Server{
	int socket;
	struct sockaddr_in servaddr;
}main_Server;


//data types
cli_data client = {.state = INFO};
main_Server info;
cli_serv_data cli_serv;

//global variables
char *prompt_name;
char *ip_main, *port_main;
char *conn_user_id;
char client_name[BUF_SIZE];
char recv_name[BUF_SIZE];

//global flags
int sendquit_flag = 0;
int recvquit_flag = 0;
int flag_conn = 0;
int flag_read = 0;
int cntC_flag = 0;

//helper functions
void state_machine(char *user);
void conn_to_mainserv(char *ip, char *port);
void conn_to_cli(char *ip, char *port);
void connfig_cli_server();

//pthread functions
void *read_line(void *args);
void *accept_conn(void *args);
void *send_chat(void *args);
void *recv_chat(void *args);

//pthread global variables
pthread_t t_id_r;
pthread_t t_id_c;
pthread_t t_id_snd;
pthread_t t_id_rcv;

//signal handler functions
void cnt_c_handler(int);
void init_signals(void);
void sigint_doit();

//signal handler global variables
struct sigaction sigact;
char *progname;
int sigint_flag = 0;

