#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <string.h>

#include "../Data_models/location.h"

int comparison;

int cmp(const void *a, const void *b)
{
  register location_t *p1=(location_t *)a;
  register location_t *p2=(location_t *)b;
  switch(comparison)
  {
    case 0 : return strcmp(p1->state,p2->state);
    case 1 : return (p1->locationID)>(p2->locationID)? 1:( (p1->locationID)<(p2->locationID) ? -1:0 );
  }

  return 0;
}

int main (int argc, char **argv)
{
  /* print usage if needed */
  if (argc != 2) {
       fprintf(stderr, "Usage: \n0: state\n1: locationID\n");
       exit(0);
   }

   /* get comparison number */
  comparison  = atoi(argv[1]);
  if (comparison < 0 || comparison > 1)
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

  sprintf(filename, "../../Data/tableinfo.dat");
  file = fopen(filename, "rb");

  int locationNum, userNum, messageNum;
  fread(&locationNum, sizeof(int), 1, file);
  fread(&userNum, sizeof(int), 1, file);
  fread(&messageNum, sizeof(int), 1, file);
  fclose(file);

  //read files into buffer
  location_t *buffer = malloc(sizeof(location_t) * locationNum);

  FILE *ifp = NULL, *ofp = NULL;

  for (j=0; j < locationNum; j++)
  {
    sprintf(filename,"../../Data/Locations/location_%06d.dat", j);
    ifp = fopen(filename, "rb");
    location_t *location = read_location(ifp);
    buffer[j] = *location;
    fclose(ifp);
	free_location(location);
  }

  qsort(buffer, locationNum, sizeof(location_t), cmp);

  for (k=0; k < locationNum; k++)
  {
    sprintf(filename, "../../Data/Locations/location_%06d.dat",k);
    ofp = fopen(filename, "wb");
    location_t *location = &buffer[k];
    fwrite(&location->locationID, sizeof(int), 1, ofp);
    fwrite(location->city, sizeof(char), TEXT_SHORT, ofp);
    fwrite(location->state, sizeof(char), TEXT_SHORT, ofp);
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
