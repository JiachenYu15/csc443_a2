#include <stdio.h>
#include <iostream>
#include "library.h"
using namespace std;

/******** Section 2 ********/

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



