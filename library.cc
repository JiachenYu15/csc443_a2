#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "library.h"
using namespace std;

/******** Section 2 ********/

/******** Section 3 ********/

/******** Section 4 ********/

void init_heapfile(Heapfile *heapfile, int page_size, FILE *file){
    int num_directory_entries_per_page = entry_per_directory_page(page_size);
    int remainder = page_size - 2*sizeof(int)*num_directory_entries_per_page - sizeof(int);
    int next_directory_id = 0;
    int freespace = page_size;
    
    fwrite(&next_directory_id,sizeof(int),1,file);
    cout <<"Th number is "<< num_directory_entries_per_page<<endl<<flush;
    for(int i=0; i<num_directory_entries_per_page; i++){
        fwrite(&i, sizeof(int), 1, file);
        fwrite(&freespace, sizeof(int), 1, file);
    }
    
    if(remainder!=0){
        int* buffer = (int*)calloc(remainder,1);
        fwrite(buffer,remainder,1,file);
        free(buffer);

    }
    heapfile->file_ptr = file;
    heapfile->page_size = page_size;
}


PageID alloc_page(Heapfile *heapfile){
    FILE* file_ptr = heapfile->file_ptr;
    int page_size = heapfile->page_size;

    //Set up variables to loop though the file
    PageID current_pid = 0;
    int current_did = 0;
    int next_did = 0;

    //Read the first page
    fread(&next_did,sizeof(int),1,file_ptr);
    int search_scope = last_directory_pid(current_did,page_size);
    cout << "The search_scope is: " << search_scope << endl;    
    while(current_pid < search_scope){
        cout << "--------" << endl;
        cout << "The current directory id: " << current_did;
        cout << "--------" << endl; 
        //Check for available pages
        int freespace;
        fread(&current_pid,sizeof(int),1,file_ptr);
        fread(&freespace,sizeof(int),1,file_ptr);
        
        if(freespace == page_size) 
            return current_pid;
        else
           current_pid++;
        
        //Search the next page
        if(current_pid == search_scope){
            current_did++;
            search_scope=last_directory_pid(current_did,page_size);
            //Go the next page
            if(next_did > 0){
                int pos_next_directory_page = directory_page_offset(current_did,page_size);
                fseek(file_ptr,pos_next_directory_page,SEEK_SET);
                fread(&next_did,sizeof(int),1,file_ptr);

            }
            //Create a new directory page
            else{
                int pos_last_directory_page = directory_page_offset(current_did-1,page_size);
                fseek(file_ptr,pos_last_directory_page,SEEK_SET);
                fwrite(&current_did,sizeof(int),1,file_ptr);

                int pos_new_directory_page = directory_page_offset(current_did,page_size);
                fseek(file_ptr,pos_new_directory_page,SEEK_SET);
                fwrite(&next_did,sizeof(int),1,file_ptr);
                
                init_directory_page(file_ptr,page_size);
            }

            int pos_next_iteration = directory_page_offset(current_did,page_size) + sizeof(int);
        }   
    }
    return -1;

};

void read_page(Heapfile *heapfile, PageID pid, Page *page){
    FILE* file_ptr = heapfile->file_ptr;
    int page_size = heapfile->page_size;
    int pos_page = data_page_offset(pid, page_size);

    fseek(file_ptr, pos_page, SEEK_SET);
    fread(page->data, page_size, 1, file_ptr);
}

void write_page(Page *page, Heapfile *heapfile, PageID pid){
    int page_size = heapfile->page_size;
    FILE* file_ptr = heapfile->file_ptr;
    
    //Write the data
    int pos_data_page = data_page_offset(pid, page_size);
    fseek(file_ptr,pos_data_page,SEEK_SET);
    fwrite(page->data, page->page_size, 1, file_ptr);
    
    //Update the directory entry
    int pos_directory_page = directory_entry_offset(pid, page_size);
    fseek(file_ptr,pos_directory_page + sizeof(int),SEEK_SET);
    int freespace = fixed_len_page_freeslots(page) * page->slot_size;
    fwrite(&freespace,sizeof(int),1,file_ptr);
    
    fflush(file_ptr);
}


/******** Helpful Functions ********/

int entry_per_directory_page(int page_size){
    int entry_size = 2 * sizeof(int);
    return (page_size-sizeof(int))/entry_size;
}

PageID last_directory_pid(int directory_id, int page_size){
    int entries_per_directory = entry_per_directory_page(page_size);
    return entries_per_directory*(directory_id + 1);
}

int directory_page_offset(int directory_id,int page_size){
    int pages_in_directory = entry_per_directory_page(page_size) + 1;
    int pages_before_directory = pages_in_directory * directory_id;
    return pages_before_directory * page_size;
}


int page_directory_id(PageID page_id, int page_size){
    int entries_per_directory = entry_per_directory_page(page_size);
    return page_id/entries_per_directory;
}

int data_page_offset(PageID page_id, int page_size){
    int directory_id = page_directory_id(page_id, page_size);
    int directory_page_size = (directory_id + 1)*page_size;
    int data_page_size = page_id * page_size;
    return data_page_size + directory_page_size;
}

int directory_entry_offset(PageID page_id, int page_size){
    int directory_id = page_directory_id(page_id, page_size);
    int entries_per_directory = entry_per_directory_page(page_size);
    int local_index = page_id % entries_per_directory;
    int local_offset = sizeof(int) + sizeof(int)*2*local_index;
    return local_offset + directory_page_offset(directory_id, page_size);
}

void init_directory_page(FILE* file, int page_size){
    int num_directory_entries_per_page = entry_per_directory_page(page_size);
    int remainder = page_size - 2*sizeof(int)*num_directory_entries_per_page - sizeof(int);
    int freespace = page_size;
    
    for(int i=0; i<num_directory_entries_per_page; i++){
        fwrite(&i, sizeof(int), 1, file);
        fwrite(&freespace, sizeof(int), 1, file);
    }
 
    if(remainder!=0){
        int* buffer = (int*)calloc(remainder,1);
        fwrite(buffer,remainder,1,file);
        free(buffer);

    }
    return;
}


