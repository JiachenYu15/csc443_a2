#include <stdio.h>
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



