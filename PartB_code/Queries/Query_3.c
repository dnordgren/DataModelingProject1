#include "../Data_models/node.h"
#include <sys/times.h>

bool searchNodeForState(node_t* node, char* match_string);
bool searchNodeForLocationId(node_t* node, int key);
bool searchNodeForHour(node_t* node, int key);
char* find_location_edge(char* node_path, char* match_string, int min, int max);
char* find_user_edge(char *node_path, int key, int min, int max);
char* find_message_edge(char* node_path, int key, int min, int max);

int main(int argc, char** argv) {
	if (argc != 4) {
		fprintf(stderr, "Usage: root node file path pls\n");
		exit(0);
	}

	int matching_records = 0;
	char* match_string = "Nebraska";

		struct timeval time_start, time_end;

	// start time
	gettimeofday(&time_start, NULL);

	node_t* location_root = read_node(argv[1]);
	node_t* user_root = read_node(argv[2]);
	node_t* message_root = read_node(argv[3]);

	char* left_edge = find_location_edge(location_root->filepath, match_string, 0, location_root->child_num-2);
	node_t* left_edge_node = read_node(left_edge);
	print_node(left_edge_node);
	bool isNebraska = true;

	while (isNebraska) {
		int i;
		for (i = 0; i < left_edge_node->child_num-1; ++i) {
			FILE *infile = fopen(left_edge_node->compare[i], "rb");
			location_t* location = read_location(infile);
			if (strcmp(location->state, match_string) == 0) {
				//printf("%s, %s\n", location->city, location->state);
				char* left_user_edge = find_user_edge(user_root->filepath, location->locationID, 0, user_root->child_num-2);
				node_t* left_user_edge_node = read_node(left_user_edge);
				bool isLocationID = true;
				while(isLocationID) {
					int j;
					for(j = 0; j < left_user_edge_node->child_num-1; j++) {
						FILE *infile_2 = fopen(left_user_edge_node->compare[j], "rb");
						user_t* user = read_user(infile_2);
						bool criteria = false;
						if(user->locationID == location->locationID) {
							char* left_message_edge = find_message_edge(message_root->filepath, user->id, 0, user_root->child_num-2);
							node_t* left_message_edge_node = read_node(left_message_edge);
							bool isHour = true;
							while(isHour) {
								int k;
								for(k = 0; k < left_message_edge_node->child_num-1; k++) {
									FILE *infile_3 = fopen(left_message_edge_node->compare[k], "rb");
									message_t* message = read_message(infile_3);
									if(480 <= message->hour * 60 + message->minute && 540 >= message->hour *60 +message->minute) {
										criteria = true;
										break;
									}
								}
								if(strcmp(left_message_edge_node->right_sibling, "") == 0) {
									break;
								} else {
									left_message_edge_node = read_node(left_message_edge_node->right_sibling);
								}
								isHour = searchNodeForHour(left_message_edge_node, user->id);
							}
						}
						if(criteria){
							matching_records++;
						}
					}
					if(strcmp(left_user_edge_node->right_sibling, "") == 0) {
						break;
					} else {
						left_user_edge_node = read_node(left_user_edge_node->right_sibling);
					}
					isLocationID = searchNodeForLocationId(left_user_edge_node, location->locationID);
				}
			}
			fclose(infile);
			free_location(location);
		}
		if(strcmp(left_edge_node->right_sibling, "") == 0) {
			break;
		} else {
			left_edge_node = read_node(left_edge_node->right_sibling);
		}
		isNebraska = searchNodeForState(left_edge_node, match_string);
	}

   	printf("\n\nMatching Records %i\n", matching_records);
// end timei
	gettimeofday(&time_end, NULL);

	float totaltime = (time_end.tv_sec - time_start.tv_sec)
	+ (time_end.tv_usec - time_start.tv_usec) / 1000000.0f;

	printf("\n\nProcess time %f seconds\n", totaltime);

}

bool searchNodeForState(node_t* node, char* match_string) {
	int i;
	for (i = 0; i < node->child_num-1; i++) {
		FILE *infile = fopen(node->compare[i], "rb");
		location_t* location = read_location(infile);
		if (strcmp(location->state, match_string) == 0) {
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
	for (i = 0; i < node->child_num-1; i++) {
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

bool searchNodeForHour(node_t* node, int key) {
	int i;
	for (i = 0; i < node->child_num-1; i++) {
		FILE *infile = fopen(node->compare[i], "rb");
		message_t* message = read_message(infile);
		if (message->hour*60 + message->minute < key) {
			free_message(message);
			return true;
		}
		fclose(infile);
		free_message(message);
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
			return find_location_edge(node->children[min], match_string, 0, node->child_num-2);
		}
	}
	else {

		int mid = (max+min)/2;
		compare_file = fopen(node->compare[mid], "rb");
		location_t *location = read_location(compare_file);

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
			//free_location(location);
			if (node->is_leaf) {
				return node_path;
			}
			else {
				return find_location_edge(node->children[mid], match_string, 0, node->child_num-2);
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
				return find_user_edge(node->children[mid], key, 0, node->child_num-2);
			}
		}
	}
}

char* find_message_edge(char* node_path, int key, int min, int max) {
	FILE *compare_file;

	node_t *node = read_node(node_path);

	if (max < min) {
		if (node->is_leaf) {
			return node_path;
		}
		else {
			return find_message_edge(node->children[min], key, 0, node->child_num-2);
		}
	}
	else {
		int mid = (max+min)/2;
		compare_file = fopen(node->compare[mid], "rb");
		message_t *message = read_message(compare_file);

		// int result = cmp(in_location, location);
		fclose(compare_file);

		if (message->userID < key) {
			free_message(message);
			return find_message_edge(node_path, key, min, mid-1);
		}
		else if (message->userID  > key) {
			free_message(message);
			return find_message_edge(node_path, key, mid+1, max);
		}
		// B+ tree defined to say matches go into right child
		else {
			if (node->is_leaf) {
				return node_path;
			}
			else {
				return find_message_edge(node->children[mid], key, 0, node->child_num-2);
			}
		}
	}
}
