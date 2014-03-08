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

int find_element(char *node_path, user_t *in_user, int min, int max) {
	node_t *node = read_node(node_path);

	FILE *compare_file;
	if (min < max) {
		return min;
	}
	else {
		int mid = (max+min)/2;
		compare_file = fopen(node->compare[mid], "rb");
		user_t *user = read_user(compare_file);
		
		int result = cmp(in_user, user);
		free_user(user);
		fclose(compare_file);
		
		if (result == -1) {
			return find_element(node_path, user, min, mid-1);
		}
		else if (result == 1) {
			return find_element(node_path, user, mid+1, max);
		}
		// B+ tree defined to say matches go into right child
		else {
			return min;
		}
	}
}

int insert_element(char *node_path, user_t *user, char *filepath) {
	node_t *node = read_node(node_path);

        int find_result = find_element(node_path, user, 0, node->fanout-1);
	// if current node is a leaf
	if (node->is_leaf) {
		// leaf is full
		if (node->fanout != node->child_num) {
			int i;
			for (i = node->child_num; i > find_result; i--) {
				node->compare[i] = node->compare[i-1];
			}
			node->compare[i] = filepath;
		}
		// if leaf is full, can't insert
		else {
			return -1;
		}
	}
	// not a leaf
	else {
		// if child is full
		if (insert_element(filepath, user, node->children[find_result]) == -1) {
			// cannot split because current node is full
			if (node->fanout == node->child_num) {
				return -1;
			}
			// split children
			else {
				split_page(node_path, find_result);
				find_result = find_element(node_path, user, 0, node->fanout-1);
			}
		}
	}
	// insert is a success
	insert_element(filepath, user, node->children[find_result]);
	return 0;
}

int split_page(char *parent_node_path, int child_index) {
	node_t *parent_node = read_node(parent_node_path);

	int i, j, k, l;
	// move children to the right to make space for new child
	for (i = parent_node->child_num; i > child_index; i--) {
		parent_node->children[i] = parent_node->children[i-1];
	}
	// move compares to the right to make space for new compare
	for (j = parent_node->child_num-1; i > child_index; i--) {
		parent_node->compare[j] = parent_node->compare[j-1];
	}
	// move middle child compare element into the parent compare
	node_t *child_node = read_node(parent_node->children[child_index]);
	parent_node->compare[child_index] = child_node->compare[child_node->fanout/2];
	// make new child
	// TODO file names???
	char* new_child_path = new_node_path();
	node_t *new_child_node = create_node(parent_node->fanout, new_child_path);
	parent_node->children[child_index+1] = new_child_path;
	// move right half of previous child to new child 
	for (k = parent_node->fanout/2, l = 0; k < parent_node->fanout; k++, l++) {
		new_child_node->compare[l] = child_node->compare[k];
		// clear the previous child elements (compares) after moving
		child_node->compare[k] = NULL;
	}
	
	// TODO handle if a split is needed, but can't split up because the root is full
}

//TODO int delete_element(char* node) {}



// TODO write compare function cmp()
