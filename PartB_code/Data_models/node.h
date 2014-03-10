#ifndef __NODE_H__
#define __NODE_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "user.h"

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

// write a node to a file
void write_node(node_t *n, char* node_path);

// // given an element, add to array if leaf, otherwise add to correct child
// // returns status of operation
// // returns -1 if child is full - will call split_page()
// // int insert_element(FILE *infile);
// int insert_element(char *node_path, user_t *user, char *filepath);
// 
// // find an element, remove it from tree, redistribute if necessary
// // returns status of operation 
// int delete_element(char *filepath);
// 
// // compare infile to elements of compare[] (binary-like)
// // and return the index of the child it belongs to
// // return the index of the element
// int find_element(char *node_path, user_t *user, int min, int max);
// 
// // splits the child index into two, copy the middle element to the current page
// // has to move up call chain recursively 
// // returns status of operation
// int split_page(char *parent_node_path, int child_index);

#endif
