#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include <stdio.h>
#include <stdlib.h>

#define TEXT_SHORT      64
#define TEXT_LONG       1024


/* message structure */
typedef struct {
    char text[TEXT_LONG];       /* text */
    int year;                   /* the send time of the message: */    
    int month;                  /* month/day/year hour:minute */
    int day;
    int hour;                   /* 0 - 23 */
    int minute;                 /* 0 - 59 */
    int userID;
    int messageID;
} message_t;



/**
 * print a message 
 */
void print_message(message_t *message);

/**
 * read a message from a table
 */
message_t *read_message(FILE *fp);

/**
 * free memory of a message
 */
void free_message(message_t *message);

#endif


