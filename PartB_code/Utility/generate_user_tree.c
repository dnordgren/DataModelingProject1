#include "../Data_models/node.h"

int compare_option, id_counter;

char* create_new_path(int child_id, int parent_id, int child_index);
int find_element(char *node_path, user_t *in_user, int min, int max);
int insert_element(user_t *user, char *filepath);
int get_id();
int split_page(char *parent_node_path, int child_index);
char* split_root(char *root_path);
int cmp(user_t *user_1, user_t *user_2);

int main(int argc, char **argv) {
	if (argc != 4) {
		fprintf(stderr, "Usage: %s", argv[0]);
		exit(0);
	}

	int total_record_number = atoi(argv[1]);
	compare_option = atoi(argv[2]);
	int fanout = atoi(argv[3]);

	// create the root node
	node_t *root = create_node(fanout, "node_000000_root.dat", 0);
	root->is_leaf = true;
	root->child_num = 1;
	write_node(root, root->filepath);

	id_counter = 1;

	int i;
	// add nodes
	for (i = 0; i < total_record_number; i++) {
		char filename[1024];
		sprintf(filename, "../../Data/Users/user_%06d.dat", i);
		FILE *infile = fopen(filename, "rb");
		user_t *user = read_user(infile);
		fclose(infile);

		// node_t *node = create_node(fanout, create_new_path(id_counter), id_counter);

		if (insert_element(user, root->filepath) == -1) {
			//Handle case for splitting root node
			root->filepath = split_root(root->filepath);
		}
		free_user(user);
	}

	return 0;
}

char* create_new_path(int child_id, int parent_id, int child_index) {
	char *filepath = malloc(sizeof(char)*1024);
	sprintf(filepath, "node_%06d_%06d_%06d.dat", child_id, parent_id, child_index);
	return filepath;
}

int find_element(char *node_path, user_t *in_user, int min, int max) {
	node_t *node = read_node(node_path);

	FILE *compare_file;
	if (max < min) {
		return min;
	}
	else {
		int mid = (max+min)/2;
		compare_file = fopen(node->compare[mid], "rb");
		user_t *user = read_user(compare_file);

		int result = cmp(in_user, user);

		fclose(compare_file);

		if (result == -1) {
			return find_element(node_path, in_user, min, mid-1);
		}
		else if (result == 1) {
			return find_element(node_path, in_user, mid+1, max);
		}
		// B+ tree defined to say matches go into right child
		else {
			return min;
		}
	}
}

int insert_element(user_t *user, char *filepath) {
	node_t *node = read_node(filepath);
    int find_result = find_element(filepath, user, 0, node->child_num-2);
	// if current node is a leaf
	if (node->is_leaf) {
		// insert element into leaf (even if into overflow)
		int i;
		for (i = node->child_num-1; i > find_result; i--) {
			node->compare[i] = node->compare[i-1];
		}
		char *temp = malloc(sizeof(char)*1024);
		sprintf(temp, "../../Data/Users/user_%06d.dat", user->id);
		node->compare[find_result] = temp;
		node->child_num++;
		write_node(node, node->filepath);

		// if leaf has overflowed
		if (node->fanout+1 == node->child_num) {
			return -1;
		}
	}
	// not a leaf
	else {
		// if child has overflowed
		if (insert_element(user, node->children[find_result]) == -1) {
			// if current node has overflowed
			if (split_page(node->filepath, find_result) == -1) {
				return -1;
			}
		}
	}
	return 0;
}

int get_id() {
	return id_counter++;
}

int split_page(char *parent_node_path, int child_index) {
	node_t *parent_node = read_node(parent_node_path);
	parent_node->is_leaf = false;

	int i, j, k, l, m, n;
	// move children to the right to make space for new child
	for (i = parent_node->child_num; i > child_index+1; i--) {
		parent_node->children[i] = parent_node->children[i-1];
	}
	// move compares to the right to make space for new compare
	for (j = parent_node->child_num-1; j > child_index+1; j--) {
		parent_node->compare[j] = parent_node->compare[j-1];
	}
	// move middle child compare element into the parent compare
	node_t *child_node = read_node(parent_node->children[child_index]);
	parent_node->compare[child_index] = child_node->compare[child_node->fanout/2];

	// make new child
	int new_child_id = get_id();
	char *new_child_path = create_new_path(new_child_id, parent_node->id, child_index+1);
	node_t *new_child_node = create_node(parent_node->fanout, new_child_path, new_child_id);
	parent_node->children[child_index+1] = new_child_path;
	parent_node->child_num = parent_node->child_num+1;
	write_node(parent_node, parent_node->filepath);

	// checking if value should be copied up
	// value only needs to be copied up if leaf
	// TODO : get fancy
	int no_copy_pls = 0;
	if (!child_node->is_leaf) {
		no_copy_pls = 1;
	}

	// move right half of previous child to new child
	for (k = (parent_node->fanout/2)+no_copy_pls, l = 0; k < parent_node->fanout; k++, l++) {
		new_child_node->compare[l] = child_node->compare[k];
		// clear the previous child elements (compares) after moving
		child_node->compare[k] = "";
	}
	if(!child_node->is_leaf) {
		// moving children to new node
		for (m = (parent_node->fanout/2)+1+no_copy_pls, n = 0; m < parent_node->fanout+1; m++, n++) {
			new_child_node->children[n] = child_node->children[m];
			child_node->children[m] = "";
		}
	}

	// update child numbers
	child_node->child_num = (parent_node->fanout)/2+1;
	new_child_node->child_num = ((parent_node->fanout+1) - (parent_node->fanout/2+1))-no_copy_pls+1;
	
	// set new child to leaf if other child is leaf
	new_child_node->is_leaf = child_node->is_leaf;

	write_node(child_node, child_node->filepath);
	write_node(new_child_node, new_child_node->filepath);

	// parent node is full
	if (parent_node->child_num == parent_node->fanout+1) {
		return -1;
	}

	return 0;
}

char* split_root(char *root_path) {
	int i,j,k,l;

	node_t *root = read_node(root_path);
	// Create new root node
	int new_root_id = get_id();
	char* new_root_filepath = malloc(sizeof(char)*FILENAME_LENGTH);
	sprintf(new_root_filepath, "node_%06d_root.dat", new_root_id);
	node_t *new_root_node = create_node(root->fanout, new_root_filepath, new_root_id);
	new_root_node->is_leaf = false;

	// Give new node middle element of previous root
	new_root_node->compare[0]=root->compare[(root->fanout)/2];

	// Create new root child node
	int new_root_child_id = get_id();
	char *new_root_child_path = create_new_path(new_root_child_id, new_root_node->id, 1);
	char *old_root_child_path = create_new_path(root->id, new_root_node->id, 0);

	node_t *new_root_child = create_node(new_root_node->fanout, new_root_child_path, new_root_child_id);

	new_root_node->children[0] = old_root_child_path;
	new_root_node->children[1] = new_root_child_path;
	new_root_node->child_num = 2;

	write_node(new_root_node, new_root_node->filepath);

	// checking if value should be copied up
	// value only needs to be copied up if leaf
	// TODO : get fancy
	int no_copy_pls = 0;
	if (!root->is_leaf) {
		no_copy_pls = 1;
		root->compare[(root->fanout)/2] = "";
	}

	// Move right half of elements from root to new child
	for (i = ((root->fanout)/2)+no_copy_pls, j = 0; i < root->fanout; i++, j++) {
		new_root_child->compare[j] = root->compare[i];
		// clear the previous child elements (compares) after moving
		root->compare[i] = "";
	}

	// moving children to new node
	for (k = ((root->fanout)/2)+no_copy_pls, l = 0; k < root->fanout+1; k++, l++) {
		new_root_child->children[l] = root->children[k];
		root->children[k] = "";
	}

	// update child numbers
	root->child_num = (root->fanout)/2+1;
	new_root_child->child_num = (root->fanout+1) - ((root->fanout/2)+1)-no_copy_pls+1;


	// New root child has same leaf property as previous root
	new_root_child->is_leaf = root->is_leaf;

	// Give previous root new file name, and delete previous name
	// char* previous_root_filepath = root->filepath;
	remove(root->filepath);
	root->filepath = old_root_child_path;

	write_node(root, root->filepath);
	write_node(new_root_child, new_root_child->filepath);

	return new_root_filepath;
}

//TODO int delete_element(char* node) {
//
// }

int cmp(user_t *user_1, user_t *user_2) {
	switch(compare_option) {
	    case 0 : return (user_1->id)>(user_2->id)? 1:( (user_1->id)<(user_2->id) ? -1:0 );
	    case 1 : return (user_1->locationID)>(user_2->locationID)? 1:( (user_1->locationID)<(user_2->locationID) ? -1:0 );
	    case 2 : return (user_1->message_num)>(user_2->message_num)? 1:( (user_1->message_num)<(user_2->message_num) ? -1:0 );
	}

	return 0;
}
