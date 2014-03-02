#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>

#include "user.h"

int comparison;

int cmp(const void *a, const void *b)
{
  register user_t *p1=(user_t *)a;
  register user_t *p2=(user_t *)b;
  switch(comparison)
  {
    case 0 : return (p1->id)>(p2->id)? 1:( (p1->id)<(p2->id) ? -1:0 );
    case 1 : return (p1->locationID)>(p2->locationID)? 1:( (p1->locationID)<(p2->locationID) ? -1:0 );
    case 2 : return (p1->message_num)>(p2->message_num)? 1:( (p1->message_num)<(p2->message_num) ? -1:0 );
  }

  return 0;
}

int main (int argc, char **argv)
{
  /* print usage if needed */
  if (argc != 2) {
       fprintf(stderr, "Usage: \n0: id\n1: locationID\n2: message_num\n");
       exit(0);
   }

   /* get comparison number */
  comparison  = atoi(argv[1]);
  if (comparison < 0 || comparison > 2)
  {
    fprintf(stderr, "Invalid argument given for comparison key");
    exit(0);
  }

struct timeval time_start, time_end;

    /* start time */
    gettimeofday(&time_start, NULL);

  int j, k;
  char filename[1024];
  FILE *file = NULL;

  sprintf(filename, "tableinfo.dat");

  file = fopen(filename, "rb");
   
  int locationNum, userNum, messageNum;
  fread(&locationNum, sizeof(int), 1, file);
  fread(&userNum, sizeof(int), 1, file);
  fread(&messageNum, sizeof(int), 1, file);
  fclose(file);

  //read files into buffer
  user_t *buffer = malloc(sizeof(user_t) * userNum);

  FILE *ifp = NULL, *ofp = NULL;

  for (j=0; j < userNum; j++)
  {
    sprintf(filename,"user_%06d.dat", j);
    ifp = fopen(filename, "rb");
    user_t *user = read_user(ifp);
    buffer[j] = *user;
    fclose(ifp);
  }

  qsort(buffer, userNum, sizeof(user_t), cmp);

  for (k=0; k < userNum; k++)
  {
    sprintf(filename, "user_%06d.dat",k);
    ofp = fopen(filename, "wb");
    user_t *user = &buffer[k];
    fwrite(&user->id, sizeof(int), 1, ofp);
    fwrite(user->name, sizeof(char), TEXT_SHORT, ofp);
    fwrite(&user->locationID, sizeof(int), 1, ofp);
    fwrite(&user->message_num, sizeof(int), 1, ofp);
    fclose(ofp);
  }
  
  free(buffer);

    /* end time */
    gettimeofday(&time_end, NULL);
    
    float totaltime = (time_end.tv_sec - time_start.tv_sec)
                    + (time_end.tv_usec - time_start.tv_usec) / 1000000.0f;

    printf("\n\nProcess time %f seconds\n", totaltime);
    

  return 0;
}
