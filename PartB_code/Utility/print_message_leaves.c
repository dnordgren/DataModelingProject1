#include "../Data_models/node.h"
#include "../../Code/Data_models/message.h"

int main(int argc, char **argv){
	char filename[1024];
	sprintf(filename, "%s", argv[1]);
	node_t* root = read_node(filename);
	
	node_t* child = read_node(root->children[0]);
	while(!child->is_leaf)
		child = read_node(child->children[0]);

	while(true){
		int i;
		printf("Node %i:\n", child->id);
		for(i = 0; i < child->child_num-1; i++){
			sprintf(filename, "%s", child->compare[i]);
			FILE *infile = fopen(filename, "rb");
			message_t *message = read_message(infile);
			print_message(message);
			free_message(message);
			fclose(infile);
		}
		child = read_node(child->right_sibling);
	}
	return 0;
}
