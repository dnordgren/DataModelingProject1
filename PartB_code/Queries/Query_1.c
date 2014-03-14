#include "../Data_models/node.h"

bool searchNodeForState(node_t* node);
bool searchNodeForLocationId(node_t* node, int key);
char* find_location_edge(char* node_path, char* match_string, int min, int max);
char* find_user_edge(char *node_path, int key, int min, int max);

int main(int argc, char** argv) {
	if (argc != 2) {
		fprintf(stderr, "Usage: root node file path pls\n");
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
			FILE *infile = fopen(left_edge_node->compare[i], "rb");
			location_t* location = read_location(infile);
			if (strcmp(location->state, "Nebraska") == 0) {
                matching_records++;
				printf("%s, %s\n", location->city, location->state);
			}
			fclose(infile);
			free_location(location);
		}
		left_edge_node = read_node(left_edge_node->right_sibling);
		isNebraska = searchNodeForState(left_edge_node);
	}
    
    printf("\n\nMatching Records %i\n", matching_records);

}

bool searchNodeForState(node_t* node) {
	int i;
	// off by one?
	for (i = 0; i < node->child_num; i++) {
		FILE *infile = fopen(node->compare[i], "rb");
		location_t* location = read_location(infile);
		if (strcmp(location->state, "Nebraska") == 0) {
			free_location(location);
			return true;
		}
		fclose(infile);
		free_location(location);
	}

	return false;
}

bool searchNodeForLocationId(node_t* node, int key) {
	int i;
	// off by one?
	for (i = 0; i < node->child_num; i++) {
		FILE *infile = fopen(node->compare[i], "rb");
		user_t* user = read_user(infile);
		if (user->locationID == key) {
			free_user(user);
			return true;
		}
		fclose(infile);
		free_user(user);
	}

	return false;
}

char* find_location_edge(char* node_path, char* match_string, int min, int max) {
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
		fclose(compare_file);

		if (strcmp(match_string, location->state) < 0) {
			free_location(location);
			return find_location_edge(node_path, match_string, min, mid-1);
		}
		else if (strcmp(match_string, location->state) > 0) {
			free_location(location);
			return find_location_edge(node_path, match_string, mid+1, max);
		}
		// B+ tree defined to say matches go into right child
		else {
			free_location(location);
			if (node->is_leaf) {
				return node_path;
			}
			else {
				return find_location_edge(node->children[min], match_string, 0, node->child_num-2);
			}
		}
	}
}

char* find_user_edge(char *node_path, int key, int min, int max) {
	node_t *node = read_node(node_path);
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
		int mid = (max+min)/2;
		compare_file = fopen(node->compare[mid], "rb");
		user_t *user = read_user(compare_file);

		// int result = cmp(in_user, user);

		fclose(compare_file);

		if (key < user->locationID) {
			free_user(user);
			free_node(node);
			return find_user_edge(node_path, key, min, mid-1);
		}
		else if (key > user->locationID) {
			free_user(user);
			free_node(node);
			return find_user_edge(node_path, key, mid+1, max);
		}
		// B+ tree defined to say matches go into right child
		else {
			free_user(user);
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
