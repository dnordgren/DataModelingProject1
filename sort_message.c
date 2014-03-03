#include "message.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>

int comparison;

int cmp(const void *a, const void *b)
{
  long minutes1, minutes2;
  register message_t *p1=(message_t *)a;
  register message_t *p2=(message_t *)b;
  switch(comparison)
  {
    case 0 :
      minutes1 = (p1->year * 525949) + (p1->month * 43829) + (p1->day * 1440) + (p1->hour * 60) + p1->minute;
      minutes2 = (p2->year * 525949) + (p2->month * 43829) + (p2->day * 1440) + (p2->hour * 60) + p2->minute;
      return minutes1 > minutes2 ? 1:( minutes1 < minutes2 ? -1:0 );
    case 1 : return (p1->hour)>(p2->hour) ? 1 : ( (p1->hour)<(p2->hour) ? -1:0);
    case 2 : return (p1->messageID)>(p2->messageID)? 1:( (p1->messageID)<(p2->messageID) ? -1:0 );
    case 3 : return (p1->userID)>(p2->userID)? 1:( (p1->userID)<(p2->userID) ? -1:0 );
  }

  return 0;
}

int main (int argc, char **argv)
{
  /* print usage if needed */
  if (argc != 2) {
       fprintf(stderr, "Usage: \n0: time\n1: hour\n2: messageID\n3: userID\n");
       exit(0);
   }

   /* get comparison number */
  comparison  = atoi(argv[1]);
  if (comparison < 0 || comparison > 3)
  {
    fprintf(stderr, "Invalid argument given for comparison key");
    exit(0);
  }

  int i = 0, j, k;
  char filename[1024];
  FILE *file = NULL;

  sprintf(filename, "message_%07d.dat",i);

  while ((file = fopen(filename, "r")))
  {
    fclose(file);
    i++;
    sprintf(filename, "message_%07d.dat",i);
  }

  //read files into buffer
  message_t *buffer = malloc(sizeof(message_t) * i);

  FILE *ifp = NULL, *ofp = NULL;

  for (j=0; j < i; j++)
  {
    sprintf(filename,"message_%07d.dat", j);
    ifp = fopen(filename, "rb");
    message_t *message = read_message(ifp);
    buffer[j] = *message;
    fclose(ifp);
  }

  qsort(buffer, i, sizeof(message_t), cmp);

  for (k=0; k < i; k++)
  {
    sprintf(filename, "message_%07d.dat",k);
    ofp = fopen(filename, "wb");
    message_t *message = &buffer[k];
    fwrite(message->text, sizeof(char), TEXT_LONG, ofp);
    fwrite(&message->year, sizeof(int), 1, ofp);
    fwrite(&message->month, sizeof(int), 1, ofp);
    fwrite(&message->day, sizeof(int), 1, ofp);
    fwrite(&message->hour, sizeof(int), 1, ofp);
    fwrite(&message->minute, sizeof(int), 1, ofp);
    fwrite(&message->userID, sizeof(int), 1, ofp);
    fwrite(&message-> messageID, sizeof(int), 1, ofp);
    fclose(ofp);
  }
    
  free(buffer);

  return 0;
}
