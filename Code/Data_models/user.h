#ifndef __USER_H__
#define __USER_H__

#include <stdio.h>
#include <stdlib.h>

#define TEXT_SHORT      64
#define TEXT_LONG       1024

/* user structure */
typedef struct {
    int id;                     /* user id */                                      
    char name[TEXT_SHORT];      /* user name */
    int locationID;  /* user location */
    int message_num;            /* number of send message */
} user_t;

/**
 * print a user 
 */
void print_user(user_t *user);

/**
 * read a user from a table
 */
user_t *read_user(FILE *fp);

/**
 * free memory of a user
 */
void free_user(user_t *user);

#endif

