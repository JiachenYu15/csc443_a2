#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <string.h>
#include "library.h"
using namespace std;

int main(int argc, char** argv){
    
    if(argc != 6){
        fprintf(stderr, "Usage: %s <heapfile> <attribute_id> <start> <end> <page_size>\n", argv[0]);
        return -1;
    }

    int attribute_id = atoi(argv[2]);
    char* start  = argv[3];
    char* end = argv[4];
    int page_size = atoi(argv[5]);
    int matched = 0;

    Heapfile* heapfile = (Heapfile*)malloc(sizeof(Heapfile));
    FILE* heap_file = fopen(argv[1], "r");
    heapfile->page_size = page_size;
    heapfile->file_ptr = heap_file;
    
    RecordIterator* riterator = new RecordIterator(heapfile);

    while(riterator->hasNext()){
        riterator->next();
        matched += search_page(riterator->get_page(), attribute_id, start, end);
        riterator->current_pid_add_one();
    }

    fclose(heap_file);
    free(heapfile);
    free(riterator);
    return 0;
}
