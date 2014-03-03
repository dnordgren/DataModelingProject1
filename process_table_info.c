#include <stdio.h>
#include <stdlib.h>

int main()
{
    char filename[1024];
    FILE *file = NULL;
    
    sprintf(filename, "tableinfo.dat");
    file = fopen(filename, "rb");

    int locationNum, userNum, messageNum;
    fread(&locationNum, sizeof(int), 1, file);
    fread(&userNum, sizeof(int), 1, file);
    fread(&messageNum, sizeof(int), 1, file);

    printf("Number of location files: %i\nNumber of user files: %i\nNumber of message files: %i\n", locationNum, userNum, messageNum);
    return 0;
}
