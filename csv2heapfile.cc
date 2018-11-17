#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <sys/timeb.h>
#include "library.h"
using namespace std;

int main(int argc, char** argv){
    if(argc != 4){
        fprintf(stderr, "Usage: %s <csv_file> <heapfile> <page_size>\n", argv[0]);
        return 1;
    }

    if(argv[3] == NULL){
        cout << "Page size is not valid." << endl;
        return -1;
    }
    int page_size = atoi(argv[3]);
    
    //read all the records
    std::vector<Record*> records;
    if(argv[1] == NULL){
        cout << "Error: CSV file cannot be found!" << endl;
        return -1;
    }
    FILE* csv_file = fopen(argv[1],"r");
    int csv_error = read_csv_records(csv_file,&records);
    if(csv_error != 0){
        cout << "read_csv_records: error reading attributes."<<endl;
        fclose(csv_file);
        return -1;
    }
    fclose(csv_file);
    cout << "Number of tuples of csv file: " << records.size()<<endl;
    
    show_records(&records);
    
    //Initialize heapfile
    Heapfile* heapfile = (Heapfile*)malloc(sizeof(Heapfile));
    if(argv[2] == NULL){
        cout << "Error: heap file cannot be found!" << endl;
        return -1;
    }
    FILE* heap_file = fopen(argv[2],"w+b");
    if(!heap_file){
        cout << "Error: Cannot open heap file!" << endl;
        fclose(heap_file);
        free(heapfile);
        return -1;
    }
    init_heapfile(heapfile, page_size, heap_file);
    
    //Prepare for iteration
    PageID pid = alloc_page(heapfile);
    Page* page = (Page*)malloc(sizeof(Page));
    init_fixed_len_page(page,heapfile->page_size,RECORD_SIZE);
    read_page(heapfile,pid,page);
    for(int i=0; i < records.size(); i++){
        if(add_fixed_len_page(page, records.at(i)) == -1){
                cout << "Page " << pid << " is full after inertion of " << i-1 << endl;
                write_page(page,heapfile,pid);
                pid = alloc_page(heapfile);
                read_page(heapfile, pid, page);
                int second = add_fixed_len_page(page, records.at(i));
                cout << "Second attempt returns: " << second << endl;
                
        }
        cout << "Record " << i << " stored at page: " << pid << endl;
    }
    
    write_page(page, heapfile, pid);
    return 0;
}

