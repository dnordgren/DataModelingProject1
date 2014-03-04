#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <string.h>

#include "location.h"
#include "user.h"
#include "message.h"

/* direction: -1 for top, 1 for bottom
 * edge: 0 for top, max for bottom
 */
int findLocationEdge(int min, int max, char *matchString, int direction, int edge)
{
  if (max < min)
  {
    return -1;
  }

  int middle;
  char *stateMid;
  char *stateMidBelow;
  middle = (min + max)/2;

  location_t *location = malloc(sizeof(location_t));

  /* read in current middle to compare to matchstring */
  FILE *ifp = NULL;
  char filename [1024];
  sprintf(filename, "location_%06d.dat", middle);
  ifp = fopen(filename, "rb");
  location = read_location(ifp);
  fclose(ifp);
  stateMid = location->state;
  free(location);

   /* catch end-case of mid being first element */
  if (middle == edge)
  {
    if (strcmp(matchString, stateMid) == 0)
    {
      return middle;
    }
    else
    {
      return -1;
    }
  }

  /* Reallocate memory for temporary location*/
  location = malloc(sizeof(location_t));
  sprintf(filename, "location_%06d.dat", middle + direction);
  ifp = fopen(filename, "rb");
  location = read_location(ifp);
  fclose(ifp);
  stateMidBelow = location->state;
  free(location);
    

  if(strcmp(matchString, stateMid) < 0 || (direction == -1 && strcmp(matchString, stateMidBelow) == 0))
  {
      return findLocationEdge(min, middle - 1, matchString, direction, edge);
  } 
  else if (strcmp(matchString, stateMid) > 0 || (direction == 1 && strcmp(matchString, stateMidBelow) == 0))
  {
    return findLocationEdge(middle + 1, max, matchString, direction, edge);
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
  sprintf(filename, "user_%06d.dat", middle);
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
  sprintf(filename, "user_%06d.dat", middle + direction);
  ifp = fopen(filename, "rb");
  user = read_user(ifp);
  fclose(ifp);
  locBelow = user->locationID;
  free(user);
    

  if(key < locMid || (direction == -1 && key == locBelow))
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

int findMessageEdge(int min, int max, int key, int direction, int edge)
{
   if (max < min)
   {
      return -1;
   }

   int middle;
   int userMid;
   int userBelow;
   middle = (min + max)/2;
   message_t *message = malloc(sizeof(message_t));
   
   /* read in current middle to compare to matchstring */
   FILE *ifp = NULL;
   char filename [1024];
   sprintf(filename, "message_%07d.dat", middle);
   ifp = fopen(filename, "rb");
   message = read_message(ifp);
   fclose(ifp);
   userMid = message->userID;
   free(message);

   /* catch end-case of mid being first element */
   if (middle == edge)
   {
      if (key == userMid)
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
   sprintf(filename, "message_%07d.dat", middle + direction);
   ifp = fopen(filename, "rb");
   message = read_message(ifp);
   fclose(ifp);
   userBelow = message->userID;
   free(message);
    
   if(key < userMid  || (direction == -1 && key == userBelow))
   {
      return findMessageEdge(min, middle - 1, key, direction, edge);
   } 
   else if (key > userMid || (direction == 1 && key == userBelow))
   {
      return findMessageEdge(middle + 1, max, key, direction, edge);
   }
   else
   {
      return middle;
   }
}

int main(int argc, char **argv)
{
  int i, currentNebUser, currentUserMessage, matches = 0;
  /* print usage if needed */
  if (argc != 1) 
  {
    fprintf(stderr, "No args needed");
    exit(0);
  }
  
  char filename[1024];
  FILE *file = NULL, *fp = NULL, *userFile = NULL, *messageFile = NULL;

  sprintf(filename, "tableinfo.dat");
  file = fopen(filename, "rb");
 
  int locationNum, userNum, messageNum;
  fread(&locationNum, sizeof(int), 1, file);
  fread(&userNum, sizeof(int), 1, file);
  fread(&messageNum, sizeof(int), 1, file);
  fclose(file);

  struct timeval time_start, time_end;

  int matchingRecords = 0;
  char *matchString = "Nebraska";

  /* start time */
  gettimeofday(&time_start, NULL);

  // Get list of NE cities
  int nebraskaStart, nebraskaEnd;
  nebraskaStart = findLocationEdge(0, locationNum, matchString, -1, 0);
  nebraskaEnd = findLocationEdge(0, locationNum, matchString, 1, locationNum);
  //printf("NE Start: %d, NE End: %d", nebraskaStart, nebraskaEnd);
  
  for (i = nebraskaStart; i <= nebraskaEnd; i++) 
  {
    /* open the corresponding file */
    sprintf(filename, "location_%06d.dat", i);

    fp = fopen(filename,"rb");

    if (!fp) {
      fprintf(stderr, "Cannot open %s\n", filename);
      exit(0);
    }

    // Get location id from NE cities
    location_t *loc = read_location(fp);
    //printf("%s, %s\n", loc->city, loc->state);
    fclose(fp);

    //printf("%s, %s\n", loc->city, loc->state);

    // Get list of users from NE cities
    int firstNebUserAtLocation, lastNebUserAtLocation;
    firstNebUserAtLocation = findUserEdge(0, userNum, loc->locationID, -1, 0);
    lastNebUserAtLocation = findUserEdge(0, userNum, loc->locationID, 1, userNum);
    //printf("  Start: %i, End: %i\n", firstNebUserAtLocation, lastNebUserAtLocation);

    // foreach user locationStart to locationEnd
    for (currentNebUser = firstNebUserAtLocation; currentNebUser <= lastNebUserAtLocation; currentNebUser++)
    {
      // open user file at current ID in loop
      sprintf(filename, "user_%06d.dat", currentNebUser);
      userFile = fopen(filename, "rb");

      if (!userFile) {
        fprintf(stderr, "Cannot open %s\n", filename);
        exit(0);
      }

      // Get userid from NE users
      user_t *usr = read_user(userFile);
      fclose(userFile);
    
      // get list of messages from NE users
      int firstUserMessage, lastUserMessage;
      firstUserMessage = findMessageEdge(0, messageNum, usr->id, -1, 0);
      lastUserMessage = findMessageEdge(0, messageNum, usr->id, 1, messageNum);
     // printf("    UserMessages Start: %i, End: %i\n", firstUserMessage, lastUserMessage);
      if(firstUserMessage == -1 && lastUserMessage == -1)
      {
	printf("Warning, no messages found for user %i\n", usr->id);
      }
      else
      {
        int criteriaMatch = 0;
        // foreach message associated with current user id
        for (currentUserMessage = firstUserMessage; currentUserMessage <= lastUserMessage; currentUserMessage++)
        {
          // open message file
          sprintf(filename, "message_%07d.dat", currentUserMessage);
          messageFile = fopen(filename, "rb");

          if (!messageFile) {
            fprintf(stderr, "Cannot open %s\n", filename);
            exit(0);
          }

          message_t *msg = read_message(messageFile);
          fclose(messageFile);

          // check time; increment
          if (msg->hour == 8 || ((msg->hour == 9) && (msg->minute == 0)))
          {
            criteriaMatch = 1;
            break;
          }
        }
	if (criteriaMatch == 1)
	{
          matchingRecords++;
	}
      }
    }

    free_location(loc);
  }

  /* end time */
  gettimeofday(&time_end, NULL);

  float totaltime = (time_end.tv_sec - time_start.tv_sec)
  + (time_end.tv_usec - time_start.tv_usec) / 1000000.0f;

  printf("\n\nMatching Records %i\n", matchingRecords);
  printf("\n\nProcess time %f seconds\n", totaltime);

  return 0;
}
