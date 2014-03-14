#include "../Data_models/node.h"

void print_node_info(char* filename, bool print_children, char* parent);

int main(int argc, char **argv) {
	if(argc != 3) {
		fprintf(stderr, "Usage: filepath print_chilren?");
	}
	if(atoi(argv[2]) > 0) {
		print_node_info(argv[1], true, "");
	} else {
		print_node_info(argv[1], false, "");
	}
	return 0;
}

void print_node_info(char* filename, bool print_children, char* parent){
	//char *filename = "node_000001_root.dat";
	node_t *node = read_node(filename);
	printf("Node ID: %i\nNode filepath: %s\nParent: %s\nNode fanout: %i\nNode child_num: %i\n\n", node->id, node->filepath, parent, node->fanout, node->child_num );
	int i;
	

	if(print_children && !node->is_leaf) {
		for (i = 0; i < node->child_num; i++){
			print_node_info(node->children[i], true, node->filepath);
		}
	}
}
