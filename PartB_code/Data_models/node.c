#include "node.h"

node_t create_node(int fanout) {
	node_t n;
	n.fanout = fanout;
	n.compare = malloc(sizeof(char)*(fanout-1));
	int i;
	for (i = 0; i < fanout-1; i++) {
		n.compare[i] = malloc(sizeof(char)*FILENAME_LENGTH);
	}
	n.children = malloc(sizeof(node_t*)*fanout);
	n.child_num = 0;
	
	return n;
}

void free_node(node_t node) {
	int i;
	for (i = 0; i < fanout-1; i++) {
		free(n.compare[i]);
	}
	free(n.compare);
	free(n.children);
}

int find_element(node_t *node, user_t *in_user, int min, int max) {
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
			return find_element(node, key, min, mid-1);
		}
		else if (result == 1) {
			return find_element(node, key, mid+1, max);
		}
		// B+ tree defined to say matches go into right child
		else {
			return min;
		}
	}
}

int insert_element(node_t *node, user_t *user, char *filepath) {
	int find_result = find_element(user, 0, fanout-1);
	// if current node is a leaf
	if (node->isLeaf) {
		// leaf is full
		if (node->fanout != node->child_num) {
			int i;
			for (i = child_num; i > find_result; i--) {
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
		if (insert_element(user, filepath, node->children[find_result]) == -1) {
			// cannot split because current node is full
			if (node->fanout == node->child_num) {
				return -1;
			}
			// split children
			else {
				split_page(node, find_result);
				find_result = find_element(user, 0, fanout-1);
			}
		}
	}
	// insert is a success
	insert_element(user, filepath, node->children[find_result]);
	return 0;
}

int split_page(node_t *parent_node, int child_index) {
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
	parent_node->compare[child_index] = parent_node->children[child_index]->compare[fanout/2];
	// make new child
	parent_node->children[child_index+1] = malloc(sizeof(node_t));
	// move right half of previous child to new child 
	for (k = fanout/2, l = 0; k < fanout; k++, l++) {
		parent_node->children[child_index+1]->compare[l] = parent_node->children[child_index]->compare[k];
		// clear the previous child elements (compares) after moving
		parent_node->children[child_index]->compare[k] = NULL;
	}
	
	// TODO handle if a split is needed, but can't split up because the root is full
}

//TODO int delete_element(char* node) {}

// TODO write compare function cmp()
