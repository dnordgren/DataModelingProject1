#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <sys/stat.h>
#include <unistd.h>

#include "record.h"
#include "dictionary.h"

int main(int argc, char **argv)
{
    /* print usage if needed */
    if (argc != 2) {
        fprintf(stderr, "Usage: %s total_record_number\n", argv[0]);
        exit(0);
    }

    /* get record number */
    int total_record_number = atoi(argv[1]);
    struct stat st;
    char filename[1024];

    sprintf(filename, "../../Data");

    if (stat(filename, &st) == -1) {
      mkdir(filename, 0700);
    }

    sprintf(filename, "../../Data/Users");

    if (stat(filename, &st) == -1) {
      mkdir(filename, 0700);
    }

    sprintf(filename, "../../Data/Locations");

    if (stat(filename, &st) == -1) {
      mkdir(filename, 0700);
    }

    sprintf(filename, "../../Data/Messages");

    if (stat(filename, &st) == -1) {
      mkdir(filename, 0700);
    }

    /*create hash table */
    hashtable_t *myHashtable;
    myHashtable = ht_create(4987);

    FILE *ifp = NULL, *ofp = NULL;

    struct timeval time_start, time_end;

    /* start time */
    gettimeofday(&time_start, NULL);

    int i, locationCounter = 0, currentUser=0, currentMessage=0, currentLoc;
    for (i = 0; i < total_record_number; i++){

        if(i%1000 == 0)
        {
            printf("%i\n", i);
        }

        /* Read Record */
        sprintf(filename, "../../Data/Records/record_%06d.dat", i);
	ifp = fopen(filename, "rb");
	record_t *rec = read_record(ifp);
       	fclose(ifp);

        /* Generate Location Files */

	/* check dictionary */
	currentLoc = ht_get(myHashtable, rec->location);

        /* if not found in dictionary */
        if(currentLoc == -1) {
            currentLoc = locationCounter;
            ht_set(myHashtable, rec->location, currentLoc);
            locationCounter++;
	}

        /* parse location into city/state */
        char *city, *state;

	city = strtok(rec->location, ",");
	state = strtok(NULL, ",");

        /* catch corner case of empty location fields */
	if (city == NULL)
	   city = "unknown";

        if (state == NULL)
            state = "unknown";

        /* write files */
        sprintf(filename, "../../Data/Locations/location_%06d.dat", currentLoc);
        ofp = fopen(filename, "wb");
        fwrite(&currentLoc, sizeof(int), 1, ofp);
       	fwrite(city, sizeof(char), TEXT_SHORT, ofp);
        fwrite(state, sizeof(char), TEXT_SHORT, ofp);
        fclose(ofp);

        /* Generate User Files */
       	sprintf(filename, "../../Data/Users/user_%06d.dat", currentUser);
        ofp = fopen(filename, "wb");
       	fwrite(&currentUser, sizeof(int), 1, ofp);
        fwrite(rec->name, sizeof(char), TEXT_SHORT, ofp);
        fwrite(&currentLoc, sizeof(int), 1, ofp);
        fwrite(&rec->message_num, sizeof(int), 1, ofp);
       	fclose(ofp);

        /* Generate Message Files */
	int j, temp;
       	for (j = 0; j < rec->message_num; j++){
            sprintf(filename, "../../Data/Messages/message_%07d.dat", currentMessage + j);
            ofp = fopen(filename, "wb");
            fwrite(rec->messages[j].text, sizeof(char), TEXT_LONG, ofp);
            fwrite(&(rec->messages[j].year), sizeof(int), 1, ofp);
            fwrite(&(rec->messages[j].month), sizeof(int), 1, ofp);
            fwrite(&(rec->messages[j].day), sizeof(int), 1, ofp);
            fwrite(&(rec->messages[j].hour), sizeof(int), 1, ofp);
            fwrite(&rec->messages[j].minute, sizeof(int), 1, ofp);
            fwrite(&currentUser, sizeof(int), 1, ofp);
            temp = currentMessage + j;
            fwrite(&temp, sizeof(int), 1, ofp);
	    fclose(ofp);
        }

        /* Update ID counters */
        currentUser++;
        currentMessage += rec->message_num;
        free_record(rec);
    }

    ht_cleanup(myHashtable);

    sprintf(filename, "../../Data/tableinfo.dat");
    ofp = fopen(filename, "wb");
    fwrite(&locationCounter, sizeof(int), 1, ofp);
    fwrite(&currentUser, sizeof(int), 1, ofp);
    fwrite(&currentMessage, sizeof(int), 1, ofp);
    fclose(ofp);

    /* end time */
    gettimeofday(&time_end, NULL);

    float totaltime = (time_end.tv_sec - time_start.tv_sec)
                    + (time_end.tv_usec - time_start.tv_usec) / 1000000.0f;

    printf("\n\nProcess time %f seconds\n", totaltime);

    return 0;
}
