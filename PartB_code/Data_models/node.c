#include "node.h"

node_t* create_node(int fanout, char *node_path, int id) {
	node_t *n = malloc(sizeof(node_t));
	n->id = id;
	n->filepath = malloc(sizeof(char)*FILENAME_LENGTH);
	sprintf(n->filepath, "%s", node_path);
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
	n->right_sibling = malloc(sizeof(char)*FILENAME_LENGTH);
	n->left_sibling = malloc(sizeof(char)*FILENAME_LENGTH);
	n->child_num = 0;

	write_node(n, node_path);
	return n;
}

node_t* read_node(char* node_path) {
	FILE *infile = fopen(node_path, "rb");
	node_t *n = malloc(sizeof(node_t));
	n->filepath = malloc(sizeof(char)*FILENAME_LENGTH);

	fread(&(n->id), sizeof(int), 1, infile);
	fread(&(n->filepath[0]), sizeof(char), FILENAME_LENGTH, infile);
	fread(&(n->fanout), sizeof(int), 1, infile);
	int i;
	n->compare = malloc(sizeof(char)*(n->fanout-1));
	for (i = 0; i < n->fanout-1; i++) {
		n->compare[i] = malloc(sizeof(char)*FILENAME_LENGTH);
		fread((n->compare[i]), sizeof(char), FILENAME_LENGTH, infile);
	}
	n->children = malloc(sizeof(char)*(n->fanout));
	for(i = 0; i < n->fanout; i++) {
		n->children[i] = malloc(sizeof(char)*FILENAME_LENGTH);
		fread((n->children[i]), sizeof(char), FILENAME_LENGTH, infile);
	}
	fread(&(n->child_num), sizeof(int), 1, infile);
	n->right_sibling = malloc(sizeof(char)*FILENAME_LENGTH);
	n->left_sibling = malloc(sizeof(char)*FILENAME_LENGTH);
	fread(&(n->left_sibling[0]), sizeof(char), FILENAME_LENGTH, infile);
	fread(&(n->right_sibling[0]), sizeof(char), FILENAME_LENGTH, infile);
	fread(&(n->is_root), sizeof(bool), 1, infile);
	fread(&(n->is_leaf), sizeof(bool), 1, infile);

	fclose(infile);

	return n;
}

void write_node(node_t *n, char* node_path) {
	FILE *outfile = fopen(node_path, "wb");

	fwrite(&(n->id), sizeof(int), 1, outfile);
	fwrite(&(n->filepath[0]), sizeof(char), FILENAME_LENGTH, outfile);
	fwrite(&(n->fanout), sizeof(int), 1, outfile);
	int i;
	for (i = 0; i < n->fanout-1; i++) {
		fwrite((n->compare[i]), sizeof(char), FILENAME_LENGTH, outfile);
	}
	for(i = 0; i < n->fanout; i++) {
		fwrite((n->children[i]), sizeof(char), FILENAME_LENGTH, outfile);
	}
	fwrite(&(n->child_num), sizeof(int), 1, outfile);
	fwrite(&(n->left_sibling[0]), sizeof(char), FILENAME_LENGTH, outfile);
	fwrite(&(n->right_sibling[0]), sizeof(char), FILENAME_LENGTH, outfile);
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
	free(n->right_sibling);
	free(n->left_sibling);
	free(n->children);
	free(n->compare);
}
