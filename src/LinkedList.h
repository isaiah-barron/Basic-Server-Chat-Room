/*
 * List.h
 *
 *  Created on: Apr 15, 2018
 *      Author: Isaiah Barron
 *      Cruzid; ibarron
 */

#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//structs ---------------------------------------------------------------------
typedef struct cli_vars{
	char *ip;
	char *port;
	char *u_id;
}cli_vars;

typedef struct NodeObj{
	struct cli_vars cli_data;
	struct NodeObj *next;
	struct NodeObj *prev;
}NodeObj;

//data types ------------------------------------------------------------------
typedef struct NodeObj *Node;

//structs ---------------------------------------------------------------------
typedef struct ListObj{
	Node head;
	Node tail;
	Node cursor;
	int length;
	int index;
}ListObj;

// data types ---------------------------------------------------------------
typedef struct ListObj *List;

// global variables ---------------------------------------------------------------


// Constructors-Destructors ---------------------------------------------------


//Returns reference to new empty List
List newList(void);


//Frees all heap memory associated with List *L, and sets *L to NULL.
void freeList(List *L);

// Access functions -----------------------------------------------------------


/*
 * int length();
 *
 * Returns the number of elements in the list.
 */
int length(List L);


/*
 * int index();
 *
 * If cursor is defined, returns the number of elements in the list
 * otherwise returns -1.
 */
int Index(List L);

/*
 * List copy()
 *
 * Returns a new List representing the same integer sequence as this
 * List. The cursor in the new list is undefined, regardless of the
 * state of the cursor in this List. This List is unchanged.
 */
List copyList(List L);

/*
 * void printList()
 * 
 * prints the contents of each item in the list
 *
 */
void printList(List L);


/* char *get_user()
 * 
 * gets the user name at cursor
 *
 * returns user name
 */
char *get_user(List L);


// Manipulation procedures ----------------------------------------------------


/*
 * void clear();
 *
 * Resets this List to its original empty state.
 */
void clear(List L);


/*
 * void moveFront();
 *
 * If list is non-empty, places the cursor under the front element,
 * otherwise does nothing
 */
void moveFront(List L);


/*
 * void moveBack();
 *
 * If List is non-empty, places the cursor under the back element,
 * otherwise does nothing.
 */
void moveBack(List L);


/*
 * void movePrev();
 *
 * If cursor is defined and not at front, moves cursor one step toward
 * front of this List, if cursor is defined and at front, cursor becomes
 * undefined, if cursor is undefined does nothing.
 */
void movePrev(List L);


/*
 * void moveNext();
 *
 * If cursor is defined and not at back, moves cursor one step toward
 * back of this List, if cursor is defined and at back, cursor becomes
 * undefined, if cursor is undefined does nothing.
 */
void moveNext(List L);


/*
 * void prepend(int data);
 *
 * Insert new element into the list.
 *
 * If list is not empty, insertion takes place before front element
 */
void prepend(List L, char *ipaddr, char *portnum, char *username);


/*
 * void append(int data);
 *
 * Insert new element into this List.
 *
 * If List is not empty, insertion takes place after back element.
 */
void append(List L, char *ipaddr, char *portnum, char *username);

/*
 * void insert_sorted(List L, unsigned long ipaddr, char *portnum, char *username);
 *
 * Insert new element into the list in alphabettical order.
 *
 * If list is not empty, insertion takes place before front element
 */
void insert_sorted(List L, char *ipaddr, char *portnum, char *username, int *flag);


/*void insertBefore(int data);
 *
 * Insert new element before cursor.
 *
 * Pre: length()>0, index()>=0
 */
void insertBefore(List L, char *ipaddr, char *portnum, char *username);


/*void insertAfter(int data);
 *
 * Inserts new element after cursor.
 *
 * Pre: length()>0, index()>=0
 */
void insertAfter(List L, char *ipaddr, char *portnum, char *username);


/*void deleteFront()
 *
 * Deletes the front element.
 *
 * Pre: length()>0
 */
void deleteFront(List L);


/*void deleteBack()
 *
 * Deletes the back element.
 *
 * Pre: length()>0
 */
void deleteBack(List L);


/*
 * void delete();
 *
 * Deletes cursor element, making cursor undefined.
 *
 * Pre: length()>0, index()>=0
 */
void delete(List L);

#endif /* LIST_H*/
