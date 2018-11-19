#include <iostream>
#include <vector>
#include "library.h"
using namespace std;

int main(int argc, char** argv){
    if(argc < 3){
        fprintf(stderr, "Usage: %s <heap_file> <page_size>\n", argv[0]);
        return -1;
    }
    
    FILE* file_ptr = fopen(argv[1], "r");
    int page_size = atoi(argv[2]);

    Heapfile* heapfile = (Heapfile*)malloc(sizeof(Heapfile));
    heapfile->file_ptr = file_ptr;
    heapfile->page_size = page_size;

    RecordIterator* riterator = new RecordIterator(heapfile);

    while(riterator->hasNext()){
        riterator->next();
        print_page_records(riterator->get_page());
        riterator->current_pid_add_one();
    }
    
    fclose(file_ptr);
    free(heapfile);
    free(riterator);

    return 0;
}
