#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string.h>
#include "library.h"
using namespace std;

int main(int argc, char** argv){
    if (argc!=6){
        fprintf(stderr, "Usage: %s <heapfile> <record_id> <attribute_id> <new_value> <page_size>\n", argv[0]);
        return -1;
    }
    //Assume the max length of a slot id is 5 characters, which means the max is 99999
    //
    int id_size = strlen(argv[2]);
    if(id_size < 5) return -1;
    
    int i;
    char pid_str[id_size-5];
    for(i=0; i < id_size-5; i++){
        pid_str[i] = argv[2][i];
    }

    char rid_str[6];
    for(;i<id_size;i++){
        rid_str[i-(id_size-5)] = argv[2][i];
    }

    PageID pid = atoi(pid_str);
    int slot = atoi(rid_str);

    cout << "The pid is: " << pid << endl;
    cout << "The slot is: " << slot << endl;

    int attribute_id = atoi(argv[3]);
    char* new_value = argv[4];
    int page_size = atoi(argv[5]);

    FILE* heap_file = fopen(argv[1],"r+");
    Heapfile* heapfile = (Heapfile*)malloc(sizeof(Heapfile));
    heapfile->file_ptr = heap_file;
    heapfile->page_size = page_size;

    Page* page = (Page*)malloc(sizeof(Page));
    init_fixed_len_page(page, page_size, RECORD_SIZE);
    read_page(heapfile, pid, page);
    int freeslots = fixed_len_page_freeslots(page);
    int capacity = fixed_len_page_capacity(page);
    if(freeslots == capacity){
        cout << "Error: The page is empty. Page ID: " << pid << endl;
        return -1;
    }
    
    /*
    char* ptr = (char*)page->data + 
    if(((*ptr >> slot) & 1) == 0){
        cout << "Error: The slot is empty. Page ID: " << pid << " Slot id: " << slot << endl;
        return -1;
    }
    */

    Record* record = new Record;
    read_fixed_len_page(page, slot, record);
    record->at(attribute_id) = new_value;
    write_fixed_len_page(page, slot, record);

    write_page(page, heapfile, pid);

    fclose(heap_file);
    free(record);
    free(heapfile);

    return 0;
}
