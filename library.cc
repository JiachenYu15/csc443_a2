#include <stdio.h>
#include <stdlib.h>
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
    PageID pageid;

    fseek(file_ptr, 0, SEEK_END);
    pageid = ftell(file_ptr)/page_size;
    char* buffer = (char*)calloc(page_size, 1);
    fwrite(buffer, page_size, 1, file_ptr);
    free(buffer);
    return pageid;
};

void read_page(Heapfile *heapfile, PageID pid, Page *page){
    FILE* file_ptr = heapfile->file_ptr;
    int page_size = heapfile->page_size;

    fseek(file_ptr, page_size*pid, SEEK_SET);
    fread(page, page_size, 1, file_ptr);
}

void write_page(Page *page, Heapfile *heapfile, PageID pid){
    FILE* file_ptr = heapfile->file_ptr;
    int page_size = heapfile->page_size;

    fseek(file_ptr, page_size*pid, SEEK_SET);
    fwrite(page, page_size, 1, file_ptr);
}



