#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include "library.h"
using namespace std;

int main(int argc, char** argv){
    if(argc != 4){
        fprintf(stderr, "Usage %s <heapfile> <csv_file> <page_size>\n", argv[0]);
        return -1;
    }

    int page_size = atoi(argv[3]);
    
    //load records from csv file
    std::vector<Record*> records;
    FILE* csv_ptr = fopen(argv[2], "r");
    int csv_error = read_csv_records(csv_ptr, &records);
    cout << "Number of records in the csv file: " << records.size() << endl;

    //Open heapfile 
    FILE* heap_file = fopen(argv[1], "r+");
    Heapfile* heapfile = (Heapfile*)malloc(sizeof(Heapfile));
    heapfile->file_ptr = heap_file;
    heapfile->page_size = page_size;
    
    //Prepare for iteration
    PageID pid = alloc_space(heapfile, RECORD_SIZE);
    Page* page = (Page*)malloc(sizeof(Page));
    init_fixed_len_page(page, page_size, RECORD_SIZE);
    read_page(heapfile, pid, page);
    for(int i=0; i< records.size(); i++){
        if(add_fixed_len_page(page, records.at(i)) == -1){
            cout << "Insert: Page " << pid << " is full after insertion of " << i-1 << endl;
            write_page(page, heapfile, pid);
            pid = alloc_space(heapfile, RECORD_SIZE);
            read_page(heapfile, pid, page);
            int second = add_fixed_len_page(page, records.at(i));
            cout << "Insert: second attempts return: " << second << endl;
        }
        cout << "Record " << i << " stored at page: " << pid << endl;
    }

    write_page(page, heapfile, pid);

    fclose(heap_file);
    fclose(csv_ptr);
    free(heapfile);
    return 0;
}

