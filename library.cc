#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <cmath>
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
            *char_buf = (*itr)[j];
            char_buf++;
        }
    }
}


/**
 * Deserializes `size` bytes from the buffer, `buf`, and
 * stores the record in `record`.
 */
void fixed_len_read(void *buf, int size, Record *record) {
    char* char_buf = (char *)buf;
    for (int i = 0; i < size / 10; i++) {
        char *attr = (char *) calloc(11, sizeof(char));
        memcpy(attr, char_buf + i * 10, 10);
        record->push_back(attr);
    }
}

/******** Section 3 ********/

int fixed_len_page_directory_slots_count(Page *page);
int get_first_free_freeslot(Page *page);

/**
 * Initializes a page using the given slot size
 * Use slotted directory based page layout to store fixed length records.
 */
void init_fixed_len_page(Page *page, int page_size, int slot_size) {
    page->page_size = page_size;
    page->slot_size = slot_size;
    page->data = malloc(page_size);
    page->directory_slots_count = fixed_len_page_directory_slots_count(page);

    int num_slots = floor((float)page_size / slot_size);
    char* ptr = (char*) page->data;

    for (int i = 0; i < page->directory_slots_count * slot_size; i++) {
        for (int j = 0; j < 8; j++) {
            *ptr &= ~(1 << j);
        }
        ptr++;
    }
}


/*
 * Calculates the number of slots required to reserve for directory
 */
int fixed_len_page_directory_slots_count(Page *page) {
    // Total number of slots
    int num_slots = page->page_size / page->slot_size;

    // Number of bytes needed for directory slots
    int directory_slots_bytes = ceil(num_slots / 8.0);

    // Number of slots required to reserve for directory
    return ceil((float)directory_slots_bytes / page->slot_size);
}
 
/**
 * Calculates the maximal number of records that fit in a page
 */
int fixed_len_page_capacity(Page *page) {
    // Total number of slots
    int num_slots = floor((float)page->page_size / page->slot_size);

    return num_slots - page->directory_slots_count;
}
 
/**
 * Calculate the free space (number of free slots) in the page
 */
int fixed_len_page_freeslots(Page *page) {
    int freeslots_count = 0;
    int data_slots_total_count = fixed_len_page_capacity(page);
    int bytes_to_check = ceil(data_slots_total_count / 8.0);
    char* ptr = (char*) page->data;

    for (int i = 0; i < bytes_to_check; i++) {
        int end_limit = 8;
        if (data_slots_total_count < end_limit) {
            end_limit = data_slots_total_count;
        }

        for (int j = 0; j < end_limit; j++) {
            freeslots_count += ((ptr[i] >> j) & 1) == 0;
        }
        data_slots_total_count -= 8;
    }
    return freeslots_count;
}
 
/**
 * Add a record to the page
 * Returns:
 *   record slot offset if successful,
 *   -1 if unsuccessful (page full)
 */
int add_fixed_len_page(Page *page, Record *r) {
    // Return -1 if page is full
    if (fixed_len_page_freeslots(page) < 1) {
        return -1;
    }

    // Find the position of the first free slot
    int first_free_slot_pos = get_first_free_freeslot(page);
    // Set the bit to 1
    char* ptr = (char*) page->data;
    *ptr |= 1 << first_free_slot_pos;


    write_fixed_len_page(page, first_free_slot_pos, r);
    return first_free_slot_pos;
}


/**
 * Find first free slot in page
 */
int get_first_free_freeslot(Page *page) {
    int data_slots_total_count = fixed_len_page_capacity(page);
    int bytes_to_check = ceil(data_slots_total_count / 8.0);
    char* ptr = (char*) page->data;
    int free_slot_pos = 0;

    for (int i = 0; i < bytes_to_check; i++) {
        int end_limit = 8;
        if (data_slots_total_count < end_limit) {
            end_limit = data_slots_total_count;
        }

        for (int j = 0; j < end_limit; j++) {
            if (((ptr[i] >> j) & 1) == 0) {
                return free_slot_pos;
            }
            free_slot_pos++;
        }
        data_slots_total_count -= 8;
    }
    return -1;
}

 
/**
 * Write a record into a given slot.
 */
void write_fixed_len_page(Page *page, int slot, Record *r) {
    char* slot_to_write = ((char *) page->data) + page->slot_size * (slot + page->directory_slots_count);
    fixed_len_write(r, (void *) slot_to_write);
}
 
/**
 * Read a record from the page from a given slot.
 */
void read_fixed_len_page(Page *page, int slot, Record *r) {
    char* slot_to_read = ((char *) page->data) + page->slot_size * (slot + page->directory_slots_count);
    fixed_len_read((void *) slot_to_read, page->slot_size, r);
}

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
    //int freespace = 8;
    fwrite(&freespace,sizeof(int),1,file_ptr);
    
    //fflush(file_ptr);
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


