#ifndef __LOCATION_H__
#define __LOCATION_H__

#include <stdio.h>
#include <stdlib.h>

#define TEXT_SHORT      64
#define TEXT_LONG       1024

/* location structure */
typedef struct {
    int locationID;              /* location id */                                      
    char city[TEXT_SHORT];      /* city */
    char state[TEXT_SHORT];      /* state */
} location_t;


/**
 * print a location
 */
void print_location(location_t *location);

/**
 * read a location from a table
 */
location_t *read_location(FILE *fp);

/**
 * free memory of a location
 */
void free_location(location_t *location);

#endif

