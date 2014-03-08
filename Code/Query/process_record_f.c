#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <string.h>

#include "message.h"
#include "user.h"

int cmp(const void *a, const void *b);
int compareTime(int minutes1, int minutes2);

/* direction: -1 for top, 1 for bottom
 * edge: 0 for top, max for bottom
 */
int findMessageEdge(int min, int max, int key, int direction, int edge)
{
   if (max < min)
   {
      return -1;
   }

   int middle;
   int hourMid, minuteMid;
   int hourBelow, minuteBelow;
   middle = (min + max)/2;

   message_t *message = malloc(sizeof(message_t));

   /* read in current middle to compare to matchstring */
   FILE *ifp = NULL;
   char filename [1024];
   sprintf(filename, "../../Data/Messages/message_%07d.dat", middle);
   ifp = fopen(filename, "rb");
   message = read_message(ifp);
   fclose(ifp);
   hourMid = message->hour;
   minuteMid = message->minute;
   free(message);

   /* catch end-case of mid being first element */
   if (middle == edge)
   {
      if (key == hourMid)
      {
         return middle;
      }
      else
      {
         return -1;
      }
   }

   /* Reallocate memory for temporary location*/
   message = malloc(sizeof(message_t));
   sprintf(filename, "../../Data/Messages/message_%07d.dat", middle + direction);
   ifp = fopen(filename, "rb");
   message = read_message(ifp);
   fclose(ifp);
   hourBelow = message->hour;
   minuteBelow = message->minute;
   free(message);

   if(compareTime(key, hourMid*60 + minuteMid) < 0 || (direction == -1 && compareTime(key, hourBelow*60 + minuteBelow) == 0))
   {
	   return findMessageEdge(min, middle - 1, key, direction, edge);
   }
   else if (compareTime(key, hourMid*60 + minuteMid) > 0 || (direction == 1 && compareTime(key, hourBelow*60 + minuteBelow)==0) )
   {
      return findMessageEdge(middle + 1, max, key, direction, edge);
   }
   else
   {
      return middle;
   }
}

int findUserEdge(int min, int max, int key, int direction, int edge)
{
   if (max < min)
   {
      return -1;
   }

   int middle;
   int locMid;
   int locBelow;
   middle = (min + max)/2;

   user_t *user = malloc(sizeof(user_t));

   /* read in current middle to compare to matchstring */
   FILE *ifp = NULL;
   char filename [1024];
   sprintf(filename, "../../Data/Users/user_%06d.dat", middle);
   ifp = fopen(filename, "rb");
   user = read_user(ifp);
   fclose(ifp);
   locMid = user->locationID;
   free(user);

   /* catch end-case of mid being first element */
   if (middle == edge)
   {
      if (key == locMid)
      {
         return middle;
      }
      else
      {
         return -1;
      }
   }

   /* Reallocate memory for temporary location*/
   user = malloc(sizeof(user_t));
   sprintf(filename, "../../Data/Users/user_%06d.dat", middle + direction);
   ifp = fopen(filename, "rb");
   user = read_user(ifp);
   fclose(ifp);
   locBelow = user->locationID;
   free(user);


   if(key < locMid  || (direction == -1 && key == locBelow))
   {
      return findUserEdge(min, middle - 1, key, direction, edge);
   }
   else if (key > locMid || (direction == 1 && key == locBelow))
   {
      return findUserEdge(middle + 1, max, key, direction, edge);
   }
   else
   {
      return middle;
   }
}

int main(int argc, char **argv)
{
   int i, k, l, numOfMessages, numOfUniqueUsers, previousUser=0, matches = 0;
   /* print usage if needed */
    if (argc != 1) {
       fprintf(stderr, "No args needed");
       exit(0);
   }

    char filename[1024];
    FILE *file = NULL, *fp = NULL;

    sprintf(filename, "../../Data/tableinfo.dat");
    file = fopen(filename, "rb");

    int locationNum, userNum, messageNum;
    fread(&locationNum, sizeof(int), 1, file);
    fread(&userNum, sizeof(int), 1, file);
    fread(&messageNum, sizeof(int), 1, file);
    fclose(file);

    struct timeval time_start, time_end;

    int lowKey = 8*60;
    int highKey = 9*60;

    /* start time */
    gettimeofday(&time_start, NULL);
    int messageStart, messageEnd;
    messageStart = findMessageEdge(0, messageNum, lowKey, -1, 0);
    messageEnd = findMessageEdge(0, messageNum, highKey, 1, messageNum);
    printf("Message Start: %d, Message End: %d\n", messageStart, messageEnd);

    int *buffer, userStart;
    numOfMessages = messageEnd-messageStart+1;
    buffer = malloc(sizeof(int) * numOfMessages);

    for (k = 0, i = messageStart; i <= messageEnd; k++, i++) {
      /* open the corresponding file */
      sprintf(filename, "../../Data/Messages/message_%07d.dat", i);
      fp = fopen(filename,"rb");
      if (!fp) {
        fprintf(stderr, "Cannot open %s\n", filename);
        exit(0);
      }

      message_t *message = read_message(fp);
      fclose(fp);
      // printf("Hour: %i, User ID: %i, Message ID: %i\n", message->hour, message->userID, message->messageID);

      buffer[k] = message->userID;

      free_message(message);
   }

   qsort(buffer, numOfMessages, sizeof(int), cmp);

   if (buffer != NULL)
   {
      numOfUniqueUsers = 1;
   }

   previousUser = buffer[0];

   for (l = 1; l < numOfMessages; l++)
   {
      if (buffer[l] != previousUser)
      {
        numOfUniqueUsers++;
      }
      previousUser = buffer[l];
   }

   /* end time */
   gettimeofday(&time_end, NULL);

   float totaltime = (time_end.tv_sec - time_start.tv_sec)
   + (time_end.tv_usec - time_start.tv_usec) / 1000000.0f;


   printf("\n\nMatching Records %i\n", numOfUniqueUsers);
   printf("\n\nProcess time %f seconds\n", totaltime);

   return 0;
}

int cmp(const void *a, const void *b)
{
  register int *p1=(int *)a;
  register int *p2=(int *)b;

  return (*p1 > *p2) ? 1 : (*p1 < *p2) ? -1 : 0;
}

int compareTime(int minutes1, int minutes2)
{
    return (minutes1 > minutes2) ? 1 : (minutes1 < minutes2) ? -1 : 0;
}
