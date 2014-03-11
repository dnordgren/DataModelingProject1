#include "node.h"

node_t* create_node(int fanout, char *node_path, int id) {
	node_t *n = malloc(sizeof(node_t));
	n->id = id;
	n->filepath = malloc(sizeof(char)*FILENAME_LENGTH);
	//sprintf(n->filepath, "%s", node_path);
	n->filepath = node_path;

	n->fanout = fanout;
	n->compare = malloc(sizeof(char *)*(fanout));
	int i;
	for (i = 0; i < fanout; i++) {
		n->compare[i] = malloc(sizeof(char)*FILENAME_LENGTH);
	}
	n->children = malloc(sizeof(char *)*(fanout+1));
	for(i = 0; i < fanout+1; i++) {
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
	if (infile == NULL) {
			fprintf(stderr, "The input file stream %s is NULL\n", node_path);
			exit(0);
	}

	node_t *n = malloc(sizeof(node_t));
	n->filepath = malloc(sizeof(char)*FILENAME_LENGTH);

	fread(&(n->id), sizeof(int), 1, infile);
	fread(&(n->filepath[0]), sizeof(char), FILENAME_LENGTH, infile);
	fread(&(n->fanout), sizeof(int), 1, infile);
	int i;
	n->compare = malloc(sizeof(char *)*(n->fanout));
	for (i = 0; i < n->fanout; i++) {
		n->compare[i] = malloc(sizeof(char)*FILENAME_LENGTH);
		fread((n->compare[i]), sizeof(char), FILENAME_LENGTH, infile);
	}
	n->children = malloc(sizeof(char *)*(n->fanout+1));
	for(i = 0; i < n->fanout+1; i++) {
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
	if (outfile == NULL) {
			fprintf(stderr, "The output file stream %s is NULL\n", node_path);
			exit(0);
	}

	fwrite(&(n->id), sizeof(int), 1, outfile);
	fwrite(&(n->filepath[0]), sizeof(char), FILENAME_LENGTH, outfile);
	fwrite(&(n->fanout), sizeof(int), 1, outfile);
	int i;
	// only normally write to compares
	// the fanout-th element is overflow
	for (i = 0; i < n->fanout; i++) {
		fwrite((n->compare[i]), sizeof(char), FILENAME_LENGTH, outfile);
	}
	for(i = 0; i < n->fanout+1; i++) {
		fwrite((n->children[i]), sizeof(char), FILENAME_LENGTH, outfile);
	}
	fwrite(&(n->child_num), sizeof(int), 1, outfile);
	fwrite(&(n->left_sibling[0]), sizeof(char), FILENAME_LENGTH, outfile);
	fwrite(&(n->right_sibling[0]), sizeof(char), FILENAME_LENGTH, outfile);
	fwrite(&(n->is_root), sizeof(bool), 1, outfile);
	fwrite(&(n->is_leaf), sizeof(bool), 1, outfile);

	fclose(outfile);
}

char* rename_node(char *filename, int parent_id, int child_index) {
	node_t *node = read_node(filename);
	char *new_filename = malloc(sizeof(char)*1024);
	sprintf(new_filename, "node_%06d_%06d_%06d.dat", node->id, parent_id, child_index);
	remove(node->filepath);
	node->filepath = new_filename;
	write_node(node, node->filepath);
	return new_filename;
}

void free_node(node_t *n) {
	int i;
	for (i = 0; i < n->fanout; i++) {
		free(n->compare[i]);
	}
	for (i = 0; i < n->fanout+1; i++) {
		free(n->children[i]);
	}
	free(n->right_sibling);
	free(n->left_sibling);
	free(n->children);
	free(n->compare);
}
