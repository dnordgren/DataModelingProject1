#include "../Data_models/node.h"

int compare_option, id_counter = 0;

int main(int argc, char **argv) {
	if (argc != 4) {
		fprintf(stderr, "Usage: %s", argv[0]);
		exit(0);
	}
	
	int total_record_number = atoi(argv[1]);
	compare_option = atoi(argv[2]);
	int fanout = atoi(argv[3]);
	
	int i = 0;
	
	// create the root node
	char filename[1024];
	sprintf(filename, "../../Data/Users/user_%06d.dat", i);
	FILE *infile = fopen(filename, "rb");
	user_t *user = read_user(infile);
	fclose(infile);
	
	node_t *root = create_node(fanout, "node_000000_root.dat", 0);
	free_user(user);
	
	id_counter++;
	i++;
	
	// add nodes
	for (i; i < total_record_number; i++) {
		char filename[1024];
		sprintf(filename, "../../Data/Users/user_%06d.dat", i);
		FILE *infile = fopen(filename, "rb");
		user_t *user = read_user(infile);
		fclose(infile);
		
		// node_t *node = create_node(fanout, create_new_path(id_counter), id_counter);
		
		if (insert_element(user, root->path) == -1) {
			//Handle case for splitting root node
			root = split_root();
			insert_element(user, root->path;
		}
		free_user(user);
	}
}

char* create_new_path(int child_id, int parent_id, int child_index) {
	char filepath[1024];
	sprintf(filepath, "node_%06d_%06d_%06d.dat", child_id, parent_id, child_index);
	id_counter++;
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

int insert_element(user_t *user, char *filepath) {
	node_t *node = read_node(filepath);

    int find_result = find_element(filepath, user, 0, node->fanout-1);
	// if current node is a leaf
	if (node->is_leaf) {
		// leaf is full
		if (node->fanout != node->child_num) {
			int i;
			for (i = node->child_num; i > find_result; i--) {
				node->compare[i] = node->compare[i-1];
			}
			//TODO how are we going to save user filepath
			sprintf(node->compare[i], "user_%06d.dat", user->id);
		}
		// if leaf is full, can't insert
		else {
			return -1;
		}
	}
	// not a leaf
	else {
		// if child is full
		if (insert_element(user, node->children[find_result]) == -1) {
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
	insert_element(user, node->children[find_result]);
	return 0;
}

int get_id() {
	return id_counter++;
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
	int new_child_id = get_id();
	char *new_child_path = create_new_path(new_child_id, parent_node->id, child_index+1);
	node_t *new_child_node = create_node(parent_node->fanout, new_child_path, new_child_id);
	parent_node->children[child_index+1] = new_child_path;
	
	// move right half of previous child to new child 
	for (k = parent_node->fanout/2, l = 0; k < parent_node->fanout; k++, l++) {
		new_child_node->compare[l] = child_node->compare[k];
		// clear the previous child elements (compares) after moving
		child_node->compare[k] = NULL;
	}
}

//TODO int delete_element(char* node) {}


int cmp(user_t *user_1, user_t *user_2) {
	switch(compare_option) {
	    case 0 : return (user_1->id)>(user_2->id)? 1:( (user_1->id)<(user_2->id) ? -1:0 );
	    case 1 : return (user_1->locationID)>(user_2->locationID)? 1:( (user_1->locationID)<(user_2->locationID) ? -1:0 );
	    case 2 : return (user_1->message_num)>(user_2->message_num)? 1:( (user_1->message_num)<(user_2->message_num) ? -1:0 );
	}
	
	return 0;
}
