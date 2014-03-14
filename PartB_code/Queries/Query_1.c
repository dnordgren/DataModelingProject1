#include "../Data_models/node.h"

int main(int argc, char** argv) {
	if (argc != 2) {
		fprintf(stderr, "Usage: root node file path pls");
		exit(0);
	}
	
	int matching_records = 0;
	char* match_string = "Nebraska";
	
	// TODO timing functions
	
	node_t* root = read_node(argv[1]);
	
	char* left_edge = find_location_edge(root->filepath, match_string, 0, root->child_num-2);
	node_t* left_edge_node = read_node(left_edge);
	bool isNebraska = true;
	
	while (isNebraska) {
		int i;
		for (i = 0; i < left_edge_node->fanout-1; ++i) {
			if (strcmp(left_edge_node->state, "Nebraska") == 0) {
				printf("%s, %s", left_edge_node->city, left_edge_node->state);
			}
		}
		left_edge_node = read_node(left_edge_node->right_sibling);
		isNebraska = searchNodeForState(left_edge_node->filepath);
	}
	
}

bool searchNodeForState(node_t* node) {
	int i;
	// off by one?
	for (i = 0; i < node->fanout-1; i++) {
		location_t* location = read_location(node->compare[i]);
		if (strcmp(location->state, "Nebraska") == 0) {
			free_location(location);
			return true;
		}
		free_location(location);
	}
	
	return false;
}

bool searchNodeForLocationId(node_t* node, int key) {
	int i;
	// off by one?
	for (i = 0; i < node->fanout-1; i++) {
		user_t* user = read_user(node->compare[i]);
		if (user->locationID == key) {
			free_user(user);
			return true;
		}
		free_user(user);
	}
	
	return false;
}

int find_location_edge(char* node_path, char* match_string, int min, int max) {
	FILE *compare_file;
	
	node_t *node = read_node(node_path);
	
	if (max < min) {
		if (node->is_leaf) {
			return node_path;
		}
		else {
			// TODO how to free these?
			return find_location_edge(node->children[min], match_string, 0, node->child_num-2);
		}
	}
	else {
		
		int mid = (max+min)/2;
		compare_file = fopen(node->compare[mid], "rb");
		location_t *location = read_location(compare_file);

		// int result = cmp(in_location, location);

		free_node(node);
		free_location(location);
		fclose(compare_file);

		if (strcmp(match_string, location->state) < 0) {
			return find_location_edge(node_path, match_string, min, mid-1);
		}
		else if (strcmp(match_string, location->state) > 0) {
			return find_location_edge(node_path, match_string, mid+1, max);
		}
		// B+ tree defined to say matches go into right child
		else {
			if (node->is_leaf) {
				return node_path;
			}
			else {
				return find_location_edge(node->children[min], match_string, 0, node->child_num-2);
			}
		}
	}
}

int find_user_edge(char *node_path, int key, int min, int max) {
	FILE *compare_file;
	if (max < min) {
		if (node->is_leaf) {
			return node_path;
		}
		else {
			// TODO how to free these?
			return find_user_edge(node->children[min], key, 0, node->child_num-2);
		}
	}
	else {
		node_t *node = read_node(node_path);
		int mid = (max+min)/2;
		compare_file = fopen(node->compare[mid], "rb");
		user_t *user = read_user(compare_file);

		// int result = cmp(in_user, user);

		free_node(node);
		free_user(user);
		fclose(compare_file);

		if (key < user->locationID) {
			return find_user_edge(node_path, key, min, mid-1);
		}
		else if (key > user->locationID) {
			return find_user_edge(node_path, key, mid+1, max);
		}
		// B+ tree defined to say matches go into right child
		else {
			if (node->is_leaf) {
				return node_path;
			}
			else {
				// TODO how to free these?
				return find_user_edge(node->children[min], key, 0, node->child_num-2);
			}
		}
	}
}
