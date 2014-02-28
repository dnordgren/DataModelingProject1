#include "location.h"


/**
 * print a location
 */
void print_location(location_t *location)
{
    /* location cannot be NULL */
    if (location == NULL) {
        fprintf(stderr, "The location is NULL\n");
        exit(0);
    }

    /* print location ID */
    printf("ID: %02d\n", location->locationID);

    /* print location text */
    printf("City: %s, State: %s\n", location->city, location->state);
}


/**
 * read a location from a file
 */
location_t *read_location(FILE *fp)
{

  location_t *location = malloc(sizeof(location_t));

    /* Assume file has been opened */
    if (fp == NULL) {
        fprintf(stderr, "The file stream is NULL\n");
        exit(0);
    }

    /* read locationID */	//

    fread(&(location->locationID), sizeof(int), 1 , fp);
	fread(&(location->city[0]), sizeof(int), TEXT_LONG , fp);
	fread(&(location->state[0]), sizeof(int), TEXT_LONG , fp);

    return location;
}


/**
 * free memory of a record
 */
void free_location(location_t *location)
{
    if (location == NULL) {
        return;
    }

    /* free record memory */
    free(location);
}
