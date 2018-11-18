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
        cout << "The current pid is " << riterator->get_current_pid() << endl;
        cout << "The current did is " << riterator->get_current_did() << endl;
        cout << "The next did is " << riterator->get_next_did() << endl;
        riterator->next();
        print_page_records(riterator->get_page());
        riterator->current_pid_add_one();
    }
/*
    int num_entries_per_page = entry_per_directory_page(page_size);
    int next_did = 1;
    int current_pid = 0;
    int current_did = next_did-1;
    int freespace = 0;
    
    
    Page* page = (Page*)malloc(sizeof(Page));
    init_fixed_len_page(page, page_size, RECORD_SIZE);
          
    while(next_did>0){
        fseek(file_ptr,directory_page_offset(current_did, page_size),SEEK_SET);
        fread(&next_did,sizeof(int), 1, file_ptr);
        cout << "Now on directory page: " << current_did << " Next did is " << next_did << endl;
        for(int i=0; i< num_entries_per_page; i++){
            fread(&current_pid,sizeof(int),1,file_ptr);
            fread(&freespace,sizeof(int),1,file_ptr);
            if(freespace < page_size){
                read_page(heapfile, current_pid, page);
                cout << "The current pid is " << current_pid << endl;
                cout << "The freespace of this page is " << freespace << endl;
                print_page_records(page);
            }
            fseek(file_ptr,directory_entry_offset(current_pid+1, page_size),SEEK_SET);
        }           
        current_did++;
    }

    free(page); 
*/
    fclose(file_ptr);
    free(heapfile);
    free(riterator);

    return 0;
}
