/*
 * List.c
 *
 *  Created on: Apr 15, 2018
 *      Author: Isaiah Barron
 *      Cruzid; ibarron
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "LinkedList.h"

//enums -----------------------------------------------------------------------
enum boolean{
	FALSE, TRUE
};

//helper functions -------------------------------------------------------------
static int isEmpty();
static Node newNode(char *ipaddr, char *portnum, char *username);
static void freeNode(Node *N);

////Access helper functions -------------------------------------------------------------
Node front(List L);
Node back(List L);
Node get(List L);

// Constructors-Destructors/helper functions ---------------------------------------------------

/*
	Returns reference to new empty List
*/
List newList(void)
{
	List L = malloc(sizeof(ListObj));
	L->head = L->tail = NULL;
	L->cursor = NULL;
	L->length = 0;
	L->index = -1;
	return(L);
}


/*
* newNode()
* Returns reference to new Node object. Initializes next and data fields.
*
*/
static Node newNode(char *ipaddr, char *portnum, char *username)
{
	Node N = malloc(sizeof(NodeObj));
	N->next = N->prev = NULL;
	N->cli_data.ip = ipaddr;
	N->cli_data.port = portnum;
	N->cli_data.u_id = username;

	return(N);
}


/*
	Frees all heap memory associated with List *L, and sets *L to NULL
*/
void freeList(List* L)
{
	if((L != NULL) && (*L != NULL))
	{
		Node temp = (*L)->head;
		while(temp != NULL)
		{
			Node delete = temp;
			temp = temp->next;
			if(temp == NULL)
			{
				break;
			}

			free(delete);
		}
		free(*L);
		*L = NULL;
	}
}


/*
 * freeNode()
 *
 * Frees heap memory pointed to by *N, sets *N to NULL.
 */
static void freeNode(Node* N)
{
	if((N != NULL) && (*N != NULL))
	{
		free(*N);
		*N = NULL;
	}
	else
	{
		printf("List Error: NULL node\n");
		exit(1);
	}
}

//Accesses Functions ------------------------------------------------------------------------

/*
 * int length();
 *
 * Returns the number of elements in the list.
 */
int length(List L)
{
	if(L == NULL)
	{
		printf("List Error: calling length() on NULL List reference\n");
		exit(1);
	}
	else
		return (L->length);
}


/*
 * int index();
 *
 * If cursor is defined, returns the number of elements in the list
 * otherwise returns -1.
 */
int Index(List L)
{
	if(L->cursor != NULL)
	{
		return (L->index);
	}
	else
	{
		return -1;
	}
}


/*
 * void printList()
 * 
 * prints the username of each node in the list
 *
 */
void printList(List L)
{
	Node N = malloc(sizeof(NodeObj));
	moveFront(L);

	fprintf(stdout, "\nprinting list....\n");
	while(L->cursor != NULL)
	{
		N = get(L);
		fprintf(stdout, "user %s\nip %s\nport %s\n\n", N->cli_data.u_id, N->cli_data.ip, N->cli_data.port);
		moveNext(L);
	}
	return;
}

/* char *get_user()
 * 
 * gets the user name at cursor
 *
 * returns user name
 */
char *get_user(List L)
{
	if(!isEmpty(L))
		return (L->cursor->cli_data.u_id);//***this is giving me seg fault**//
	else
	{
		printf("List Error: calling get_user() cursor is undefined\n");
		exit(1);
	}
}



/*
 * List copy()
 *
 * Returns a new List representing the same integer sequence as this
 * List. The cursor in the new list is undefined, regardless of the
 * state of the cursor in this List. This List is unchanged.
 */
List copyList(List L)
{
	Node N = malloc(sizeof(NodeObj));
	List copy = newList();
	moveFront(L);

	while(L->cursor != NULL)
	{
		N = get(L);
		append(copy, N->cli_data.ip, N->cli_data.port, N->cli_data.u_id);
		moveNext(L);
	}
	return copy;
}


//Accesses Helper Functions ------------------------------------------------------------------------

/*
 * boolean isEmpty();
 *
 * Checks to see if the list is empty
 *
 * Returns true if empty and false if not empty
 *
 */
static int isEmpty(List L)
{
	if(length(L) != 0)
		return FALSE;
	else
		return TRUE;
}


/*
 * Node front();
 *
 * Returns front element.
 *
 * Pre: length > 0
 */
Node front(List L)
{
	if(!isEmpty(L))
		return (L->head);
	else
	{
		printf("List Error: calling front() on an empty List\n");
		exit(1);
	}
}


/*
 * Node back();
 *
 * Returns back element.
 *
 * Pre: length > 0
 */
Node back(List L)
{
	if(!isEmpty(L))
		return (L->tail);
	else
	{
		printf("List Error: calling back() on an empty List\n");
		exit(1);
	}
}


/*
 * Node get();
 *
 * Returns cursor element.
 *
 * Pre: length > 0, index >= 0
 */
Node get(List L)
{
	if(!isEmpty(L))
		return (L->cursor);
	else
	{
		printf("List Error: calling get() cursor is undefined\n");
		exit(1);
	}
}

// Manipulation procedures ----------------------------------------------------


/*
 * void clear();
 *
 * Resets this List to its original empty state.
 */
void clear(List L)
{
	if(L == NULL)
	{
		printf("List Error: can't clear an empty list\n");
		exit(1);
	}
	else
	{
		Node temp = L->head;

		while(temp != NULL)
		{
			Node delete = temp;
			temp = temp->next;
			free(delete);
		}

		L->head = L->tail = L->cursor = NULL;
		L->length = 0;
		L->index = -1;
	}
}


/*
 * void moveFront();
 *
 * If list is non-empty, places the cursor under the front element,
 * otherwise does nothing
 */
void moveFront(List L)
{
	if(L->head != NULL)
	{
		L->cursor = L->head;
		L->index = 0;
	}
}


/*
 * void moveBack();
 *
 * If List is non-empty, places the cursor under the back element,
 * otherwise does nothing.
 */
void moveBack(List L)
{
	if(L->tail != NULL)
	{
		L->cursor = L->tail;
		L->index = L->length - 1;
	}
}


/*
 * void movePrev();
 *
 * If cursor is defined and not at front, moves cursor one step toward
 * front of this List, if cursor is defined and at front, cursor becomes
 * undefined, if cursor is undefined does nothing.
 */
void movePrev(List L)
{
	if((L->cursor != NULL) && (L->cursor != L->head))
	{
		L->cursor = L->cursor->prev;
		L->index--;
	}
	else if((L->cursor != NULL) && (L->cursor == L->head))
	{
		L->cursor = NULL;
		L->index = -1;
	}
}


/*
 * void moveNext();
 *
 * If cursor is defined and not at back, moves cursor one step toward
 * back of this List, if cursor is defined and at back, cursor becomes
 * undefined, if cursor is undefined does nothing.
 */
void moveNext(List L)
{
	if((L->cursor != NULL) && (L->cursor != L->tail))
	{
		L->cursor = L->cursor->next;
		L->index++;
	}
	else if((L->cursor != NULL) && (L->cursor == L->tail))
	{
		L->cursor = NULL;
		L->index = -1;
	}
}


/*
 * void prepend(int data);
 *
 * Insert new element into the list.
 *
 * If list is not empty, insertion takes place before front element
 */
void prepend(List L, char *ipaddr, char *portnum, char *username)
{
	//create a new node
	Node add_to_front = newNode(ipaddr, portnum, username);

	//check to see if list is empty
	if(!isEmpty(L))
	{
		//make the head previous pointer point to the new node
		L->head->prev = add_to_front;

		//make add_to_front next pointer point to the head
		add_to_front->next = L->head;

		//now add_to_front becomes the head
		L->head = add_to_front;
	}
	else
	{
		//if list is empty, add_to_front is the head and tail
		L->head = add_to_front;
		L->tail = add_to_front;
	}

	L->index++;
	L->length++;
}


/*
 * void append(int data);
 *
 * Insert new element into this List.
 *
 * If List is not empty, insertion takes place after back element.
 */
void append(List L, char *ipaddr, char *portnum, char *username)
{
	//create a new node
	Node add_to_back = newNode(ipaddr, portnum, username);

	//check to see if list is empty
	if(!isEmpty(L))
	{
		//make the tail next pointer point to the new node
		L->tail->next = add_to_back;

		//make add_to_back previous pointer point to the tail
		add_to_back->prev = L->tail;

		//now add_to_back becomes the tail
		L->tail = add_to_back;
	}
	else
	{
		//if list is empty, add_to_back is the head and tail
		L->head = add_to_back;
		L->tail = add_to_back;
	}
	L->length++;
}

/*
 * void insert_sorted(List L, unsigned long ipaddr, char *portnum, char *username);
 *
 * Insert new element into the list in alphabettical order.
 *
 * If list is not empty, insertion takes place before front element
 */
void insert_sorted(List L, char *ipaddr, char *portnum, char *username, int *flag)
{
	Node N = malloc(sizeof(NodeObj));
	if(!isEmpty(L))
	{
		for(moveFront(L); L->cursor != NULL; moveNext(L))
		{	
			N = get(L);
			if(strcmp(username, N->cli_data.u_id) < 0)
			{
				insertBefore(L, ipaddr, portnum, username);
				break;
			}
			else if(strcmp(username, N->cli_data.u_id) == 0)
			{
				*flag = 1;
				break;
			}
		}
		if(L->cursor == NULL)
			append(L, ipaddr, portnum, username);
	}
	else
		append(L, ipaddr, portnum, username);
}


/*void insertBefore(int data);
 *
 * Insert new element before cursor.
 *
 * Pre: length()>0, index()>=0
 */
void insertBefore(List L, char *ipaddr, char *portnum, char *username)
{
	if(L->cursor == NULL)
	{
		printf("List Error: Cursor is undefined\n");
		exit(1);
	}
	else if(L->cursor == L->head)
	{
		prepend(L, ipaddr, portnum, username);//insert new element before head
	}
	else if(!isEmpty(L))
	{
		//create a new node
		Node before_curs = newNode(ipaddr, portnum, username);

		L->cursor->prev->next = before_curs;
		before_curs->prev = L->cursor->prev;
		before_curs->next = L->cursor;
		L->cursor->prev = before_curs;
		L->index++;
		L->length++;
	}
	else
	{
		printf("List Error: List is Empty\n");
		exit(1);
	}
}


/*void insertAfter(int data);
 *
 * Inserts new element after cursor.
 *
 * Pre: length()>0, index()>=0
 */
void insertAfter(List L, char *ipaddr, char *portnum, char *username)
{
	if(L->cursor == NULL)
	{
		printf("List Error: Cursor is undefined\n");
		exit(1);
	}
	else if(L->cursor == L->tail)
	{
		append(L, ipaddr, portnum, username);//insert new element after tail
	}
	else if(!isEmpty(L))
	{
		//create a new node
		Node after_curs = newNode(ipaddr, portnum, username);

		L->cursor->next->prev = after_curs;
		after_curs->next= L->cursor->next;
		after_curs->prev = L->cursor;
		L->cursor->next = after_curs;
		L->length++;
	}
	else
	{
		printf("List Error: List is empty\n");
		exit(1);
	}
}


/*void deleteFront()
 *
 * Deletes the front element.
 *
 * Pre: length()>0
 */
void deleteFront(List L)
{
	Node N = NULL;

	if(isEmpty(L))
	{
		printf("List Error: Can't delete from an empty list\n");
		exit(1);
	}
	else if(L->head == L->tail)
	{
		clear(L);
	}
	else
	{
		N = L->head;
		L->head = L->head->next;
		L->length--;
		L->index--;
		freeNode(&N);
	}

	N = NULL;
}


/*void deleteBack()
 *
 * Deletes the back element.
 *
 * Pre: length()>0
 */
void deleteBack(List L)
{
	Node N = NULL;

	if(isEmpty(L))
	{
		printf("List Error: Can't delete from an empty list\n");
		exit(1);
	}
	else if(L->tail == L->head)
	{
		clear(L);
	}
	else
	{
		N = L->tail;
		L->tail = L->tail->prev;
		L->length--;
		freeNode(&N);
	}

	N = NULL;
}


/*
 * void delete();
 *
 *Deletes cursor element, making cursor undefined.
 *
 *Pre: length()>0, index()>=0
 */
void delete(List L)
{
	if(isEmpty(L))
	{
		printf("List Error: Can't delete from an empty list\n");
		exit(1);
	}
	else if((L->cursor == L->head) && (L->cursor == L->tail))
	{
		clear(L);
	}
	else if(L->cursor == L->head)
	{
		deleteFront(L);
		L->cursor = NULL;
	}
	else if(L->cursor == L->tail)
	{
		deleteBack(L);
		L->cursor = NULL;
	}
	else
	{
		Node delete = L->cursor;
		L->cursor->next->prev = L->cursor->prev;
		L->cursor->prev->next = L->cursor->next;
		L->cursor = NULL;
		L->index = -1;
		L->length--;
		freeNode(&delete);
	}
}

