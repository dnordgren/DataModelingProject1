#include "message.h"


/**
 * print a message
 */
void print_message(message_t *message)
{
    /* message cannot be NULL */
    if (message == NULL) {
        fprintf(stderr, "The message is NULL\n");
        exit(0);
    }
    printf("Message ID: %07d\n", message->messageID);
    printf("User ID: %06d\n", message->userID);

    /* print message time */
    printf("Time: %02d/%02d/%04d %02d:%02d\n", message->month, message->day,
           message->year, message->hour, message->minute);

    /* print message text */
    printf("Text: %s\n", message->text);
}


/**
 * read a message from a file
 */
message_t *read_message(FILE *fp)
{

    message_t *message = malloc(sizeof(message_t));

    /* Assume file has been opened */
    if (fp == NULL) {
        fprintf(stderr, "The file stream is NULL\n");
        exit(0);
    }

    /* read message text */
    fread(&(message->text[0]), sizeof(char), TEXT_LONG, fp);

    /* read message time */
    fread(&(message->year), sizeof(int), 1, fp);
    fread(&(message->month), sizeof(int), 1, fp);
    fread(&(message->day), sizeof(int), 1, fp);
    fread(&(message->hour), sizeof(int), 1, fp);
    fread(&(message->minute), sizeof(int), 1, fp);
    fread(&(message->userID), sizeof(int), 1, fp);
    fread(&(message->messageID), sizeof(int), 1, fp);

    return message;
}


/**
 * free memory of a record
 */
void free_message(message_t *message)
{
    if (message == NULL) {
        return;
    }

    /* free record memory */
    free(message);
}
