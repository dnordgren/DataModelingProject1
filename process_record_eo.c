#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <string.h>

#include "location.h"
#include "user.h"

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

  location_t *location, *location2;

  /* read in current middle to compare to matchstring */
  FILE *ifp = NULL;
  char filename [1024];
  sprintf(filename, "location_%06d.dat", middle);
  ifp = fopen(filename, "rb");
  location = read_location(ifp);
  fclose(ifp);
  stateMid = location->state;
  
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
  //location = malloc(sizeof(location_t));
  
  sprintf(filename, "location_%06d.dat", middle + direction);
  ifp = fopen(filename, "rb");
  location2 = read_location(ifp);
  fclose(ifp);
  stateMidBelow = location2->state;


    if(strcmp(matchString, stateMid) < 0 || (direction == -1 && strcmp(matchString, stateMidBelow) == 0))
  {
    //printf("%s is less than %s or %s is equal to %s 1", matchString, stateMid, matchString, stateMidBelow);
    free_location(location);
    free_location(location2);
    return findLocationEdge(min, middle - 1, matchString, direction, edge);
  } 
  else if (strcmp(matchString, stateMid) > 0 || (direction == 1 && strcmp(matchString, stateMidBelow) == 0))
  {
    free_location(location);
    free_location(location2);

    return findLocationEdge(middle + 1, max, matchString, direction, edge);
  }
  else
  {
    free_location(location);
    free_location(location2);
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

  user_t *user;

  /* read in current middle to compare to matchstring */
  FILE *ifp = NULL;
  char filename [1024];
  sprintf(filename, "user_%06d.dat", middle);
  ifp = fopen(filename, "rb");
  user = read_user(ifp);
  fclose(ifp);
  locMid = user->locationID;
  free_user(user);

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
  //user = malloc(sizeof(user_t));
  sprintf(filename, "user_%06d.dat", middle + direction);
  ifp = fopen(filename, "rb");
  user = read_user(ifp);
  fclose(ifp);
  locBelow = user->locationID;
  free_user(user);

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

int main(int argc, char **argv)
{
  int i, matches = 0;
  /* print usage if needed */
  if (argc != 1) 
  {
    fprintf(stderr, "No args needed");
    exit(0);
  }
  
  char filename[1024];
  FILE *file = NULL, *fp = NULL;

  sprintf(filename, "tableinfo.dat");
  file = fopen(filename, "rb");
 
  int locationNum, userNum, messageNum;
  fread(&locationNum, sizeof(int), 1, file);
  fread(&userNum, sizeof(int), 1, file);
  // fread(&messageNum, sizeof(int), 1, file);
  fclose(file);

  struct timeval time_start, time_end;

  int matchingRecords = 0;
  char *matchString = "Nebraska";

  /* start time */
  gettimeofday(&time_start, NULL);

  int nebraskaStart, nebraskaEnd;
  nebraskaStart = findLocationEdge(0, locationNum, matchString, -1, 0);
  nebraskaEnd = findLocationEdge(0, locationNum, matchString, 1, locationNum);
  //printf("NE Start: %d, NE End: %d", nebraskaStart, nebraskaEnd);

  for ( i = nebraskaStart; i <= nebraskaEnd; i++) 
  {
    /* open the corresponding file */
    sprintf(filename, "location_%06d.dat", i);

    fp = fopen(filename,"rb");

    if (!fp) {
      fprintf(stderr, "Cannot open %s\n", filename);
      exit(0);
    }

    location_t *loc = read_location(fp);
    //printf("%s, %s\n", loc->city, loc->state);
    fclose(fp);

  	//printf("%s, %s\n", loc->city, loc->state);
  	int locationStart, locationEnd;
  	locationStart = findUserEdge(0, userNum, loc->locationID, -1, 0);
  	locationEnd = findUserEdge(0, userNum, loc->locationID, 1, userNum);
  	//printf("Start: %i, End: %i\n", locationStart, locationEnd);
  	matches += (locationEnd - locationStart + 1);

    free_location(loc);
   }

   /* end time */
   gettimeofday(&time_end, NULL);

   float totaltime = (time_end.tv_sec - time_start.tv_sec)
   + (time_end.tv_usec - time_start.tv_usec) / 1000000.0f;

   printf("\n\nMatching Records %i\n", matches);
   printf("\n\nProcess time %f seconds\n", totaltime);

   return 0;
}
