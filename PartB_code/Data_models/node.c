#include "node.h"

node_t* create_node(int fanout, char *node_path) {
	node_t *n;
	n->fanout = fanout;
	n->compare = malloc(sizeof(char)*(fanout-1));
	int i;
	for (i = 0; i < fanout-1; i++) {
		n->compare[i] = malloc(sizeof(char)*FILENAME_LENGTH);
	}
	n->children = malloc(sizeof(char)*(fanout));
	for(i = 0; i < fanout; i++) {
		n->children[i] = malloc(sizeof(char)*FILENAME_LENGTH);
	}
	n->child_num = 0;
	
	write_node(n, node_path);
	return n;
}

node_t* read_node(char* node_path) {
	FILE *infile = fopen(node_path, "rb");
	node_t *n;

	fread(&(n->fanout), sizeof(int), 1, infile);
	fread((n->compare), sizeof(char**), 1, infile);
	fread((n->children), sizeof(char**), 1, infile);
	fread(&(n->child_num), sizeof(int), 1, infile);
	fread((n->left_sibling), sizeof(char*), 1, infile);
	fread((n->right_sibling), sizeof(char*), 1, infile);
	fread(&(n->is_root), sizeof(bool), 1, infile);
	fread(&(n->is_leaf), sizeof(bool), 1, infile);

	fclose(infile);

	return n;
}

void write_node(node_t *n, char* node_path) {
	FILE *outfile = fopen(node_path, "wb");
	
	fwrite(&(n->fanout), sizeof(int), 1, outfile);
	fwrite((n->compare), sizeof(char**), 1, outfile);
	fwrite((n->children), sizeof(char**), 1, outfile);
	fwrite(&(n->child_num), sizeof(int), 1, outfile);
	fwrite((n->left_sibling), sizeof(char*), 1, outfile);
	fwrite((n->right_sibling), sizeof(char*), 1, outfile);
	fwrite(&(n->is_root), sizeof(bool), 1, outfile);
	fwrite(&(n->is_leaf), sizeof(bool), 1, outfile);
	
	fclose(outfile);
}

void free_node(node_t *n) {
	int i;
	for (i = 0; i < n->fanout-1; i++) {
		free(n->compare[i]);
	}
	for (i = 0; i < n->fanout; i++) {
		free(n->children[i]);
	}
	free(n->children);
	free(n->compare);
}
