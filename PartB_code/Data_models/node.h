#ifndef __NODE_H__
#define __NODE_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "user.h"
#include "../../Code/Data_models/location.h"
#include "../../Code/Data_models/message.h"

#define FILENAME_LENGTH 1024

// node structure
typedef struct {
	int id; // the node's id
	char *filepath; // filepath to current node

	// TODO : key property for comparison instead of using file I/O

	int fanout;	// the number of pages that the node can hold
	char **compare;	// pointer to an array of 'fanout-1' files
	char **children;	// pointer to an array of 'fanout' nodes
	int child_num;	// defines the number of pages that the node is holding
	char *left_sibling;
	char *right_sibling;
	bool is_root;	// a node with no parent
	bool is_leaf;	// a node with no children
} node_t;

// "constructor" for node structs
node_t* create_node(int fanout, char *node_path, int id);

// frees the node
void free_node(node_t *node);

// read a node from a file
node_t* read_node(char *node_path);

// rename a node file
char* rename_node(char *filename, int parent_id, int child_index);

// write a node to a file
void write_node(node_t *n, char* node_path);

#endif
