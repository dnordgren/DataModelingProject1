#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>

#include "location.h"


int main(int argc, char **argv)
{
    int i;
    /* print usage if needed */
    if (argc != 2) {
        fprintf(stderr, "Usage: %s total_location_number\n", argv[0]);
        exit(0);
    }

    /* get total record number */
    int total_location_number = atoi(argv[1]);


    char filename[1024];
    FILE *fp = NULL;

    struct timeval time_start, time_end;

    /* start time */
    gettimeofday(&time_start, NULL);

    for (i = 0; i < total_location_number; i++) {
        /* open the corresponding file */
        sprintf(filename, "../../Data/Locations/location_%06d.dat", i);

        fp = fopen(filename,"rb");

        if (!fp) {
            fprintf(stderr, "Cannot open %s\n", filename);
            exit(0);
        }

        /* read the record from the file */
        location_t *rl = read_location(fp);

        /* =========== start of data processing code ================ */

        print_location(rl);

        /* =========== end of data processing code ================ */

        /* free memory */
        free_location(rl);

        /* close the file */
        fclose(fp);
    }

    /* end time */
    gettimeofday(&time_end, NULL);

    float totaltime = (time_end.tv_sec - time_start.tv_sec)
                    + (time_end.tv_usec - time_start.tv_usec) / 1000000.0f;

    printf("\n\nProcess time %f seconds\n", totaltime);

    return 0;
}
