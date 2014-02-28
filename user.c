#include "user.h"

/**
* print a user
*/
void print_user(user_t *user)
{
   /* user cannot be NULL */
   if (user == NULL) {
       fprintf(stderr, "The user is NULL\n");
       exit(0);
   }

   /* print user id */
   printf("User Id: %d\n", user->id);

   /* print user name */
   printf("Name: %s\n", user->name);

   /* print location id */
   printf("Location ID: %d\n", user->locationID);

   /* print message number */
   printf("Number of messages: %d\n", user->message_num);
}


/**
* read a user from a file
*/
user_t *read_user(FILE *fp)
{

  user_t *user = malloc(sizeof(user_t));

   /* Assume file has been opened */
   if (fp == NULL) {
       fprintf(stderr, "The file stream is NULL\n");
       exit(0);
   }

   /* read user id */
   fread(&(user->id), sizeof(int), 1, fp);

   /* read user name */
   fread(&(user->name[0]), sizeof(char),TEXT_SHORT, fp);

   /* read location id */
   fread(&(user->locationID), sizeof(int), 1, fp);

   /* read message number */
   fread(&(user->message_num), sizeof(int), 1, fp);

   return user;
}


/**
* free memory of a user
*/
void free_user(user_t *user)
{
   if (user == NULL) {
       return;
   }

   /* free record memory */
   free(user);
}
