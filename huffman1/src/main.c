#include <stdio.h>
#include <stdlib.h>
#include "encode.h"
#include "compression.h"

int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "usage: %s <input filename> <output filename>\n",argv[0]);
        exit(1);
    }
    
    Node *root = encode(argv[1]);
    traverse_tree(root, 0, "", 0);
    compress(argv[1], argv[2]);
    
    return EXIT_SUCCESS;
}
