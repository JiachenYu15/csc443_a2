#include <stdio.h>
#include <iostream>
#include <vector>
#include "library.h"
using namespace std;

/******** Section 2 ********/

/**
 * Compute the number of bytes required to serialize record
 */
int fixed_len_sizeof(Record *record) {
    int total_bytes = 0;
    for (vector<V>::iterator itr = record->begin() ; itr != record->end(); ++itr) {
        total_bytes += strlen(*itr);
    }

    return total_bytes;
}


/**
 * Serialize the record to a byte array to be stored in buf.
 */
void fixed_len_write(Record *record, void *buf) {
    // Cast void buffer to char buffer
    char* char_buf = (char *)buf;

    for (vector<V>::iterator itr = record->begin() ; itr != record->end(); ++itr) {
        for (int j = 0; j < strlen(*itr); j++) {
            *(char_buf++) = *itr[j];
        }
    }

}


/**
 * Deserializes `size` bytes from the buffer, `buf`, and
 * stores the record in `record`.
 */
void fixed_len_read(void *buf, int size, Record *record) {
    char* char_buf = (char *)buf;
    int copied_bytes = 0;
    int current_record = 0;

    while (copied_bytes < size) {
        record->at(current_record) = strndup(char_buf + copied_bytes, 10);
        copied_bytes = copied_bytes + strlen(record->at(current_record));
        
        current_record++;
    }
}

/******** Section 3 ********/

/******** Section 4 ********/

void init_heapfile(Heapfile *heapfile, int page_size, FILE *file){
    heapfile->file_ptr = file;
    heapfile->page_size = page_size;
}

PageID alloc_page(Heapfile *heapfile){
    FILE* file_ptr = heapfile->file_ptr;
    int page_size = heapfile->page_size;
    
    fseek(file_ptr, page_size, SEEK_END);
    fputc('\0', file_ptr);
};

void read_page(Heapfile *heapfile, PageID pid, Page *page){
    FILE* file_ptr = heapfile->file_ptr;
    int page_size = heapfile->page_size;

    fseek(file_ptr, page_size*pid, SEEK_SET);
    fread((void*)page, page_size, 1, file_ptr);
}

void write_page(Page *page, Heapfile *heapfile, PageID pid){
    FILE* file_ptr = heapfile->file_ptr;
    int page_size = heapfile->page_size;

    fseek(file_ptr, page_size*pid, SEEK_SET);
    fwrite(page, page_size, 1, file_ptr);
}



