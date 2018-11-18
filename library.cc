#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <bitset>
#include <cstring>
#include "library.h"
using namespace std;

RecordIterator::RecordIterator(Heapfile* heapfile){
    current_pid = 0;
    current_did = 0;
    next_did = 1;
    this->heapfile = heapfile;
    this->page = (Page*)malloc(sizeof(Page));
    init_fixed_len_page(page, this->heapfile->page_size, RECORD_SIZE);
}

bool RecordIterator::hasNext(){
    int freespace = 0;
    int search_scope = last_directory_pid(this->current_did, this->heapfile->page_size);
    if((this->current_pid) == search_scope){
        fseek(this->heapfile->file_ptr, directory_page_offset(this->current_did, this->heapfile->page_size), SEEK_SET);
        fread(&(this->next_did), sizeof(int), 1, this->heapfile->file_ptr);
        cout << "The next did is " << this->next_did << endl; 

        if(next_did > 0){
            (this->current_did)++;
            search_scope = last_directory_pid(this->current_did, this->heapfile->page_size);
            cout << "The new search scope is " << search_scope << endl;
        }   
        else{
            return false;
        }
    }
   
    while((this->current_pid) < search_scope){
    
        //cout << "Now on directory page: " << this->current_did << " Next did is " << this->next_did << endl;
        fseek(this->heapfile->file_ptr, directory_entry_offset(this->current_pid, this->heapfile->page_size), SEEK_SET);
        fread(&(this->current_pid), sizeof(int), 1, this->heapfile->file_ptr);
        fread(&freespace, sizeof(int), 1, this->heapfile->file_ptr);

        if(freespace < this->heapfile->page_size){
            return true;
        }


        (this->current_pid)++;

        if((this->current_pid) == search_scope){
            fseek(this->heapfile->file_ptr, directory_page_offset(this->current_did, this->heapfile->page_size), SEEK_SET);
            fread(&(this->next_did), sizeof(int), 1, this->heapfile->file_ptr);
            

            if(next_did > 0){
                (this->current_did)++;
                search_scope = last_directory_pid(this->current_did, this->heapfile->page_size);
            }   
            else{
                return false;
            }
        }
            
    }

    return false;

}

Record RecordIterator::next(){
    Record temp;
    read_page(this->heapfile, this->current_pid, this->page);
    return temp;
}


int RecordIterator::get_next_did(){
    return this->next_did;
}

int RecordIterator::get_current_did(){
    return this->current_did;
}

int RecordIterator::get_current_pid(){
    return this->current_pid;
}

void RecordIterator::current_pid_add_one(){
    (this->current_pid)++;
}

Page* RecordIterator::get_page(){
    return (this->page);
}

RecordIterator::~RecordIterator(){
    free(this->page);
}


/******** Section 2 ********/

/**
 * Compute the number of bytes required to serialize record
 */
int fixed_len_sizeof(Record *record) {
    int total_bytes = 0;
    for (vector<V>::iterator itr = record->begin() ; itr != record->end(); ++itr) {
        total_bytes += ATTRIBUTE_SIZE;
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
        for (int j = 0; j < ATTRIBUTE_SIZE; j++) {
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

void print_page_records(Page* page){
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
            
            if (((ptr[i] >> j) & 1) == 1) {
                bitset<8> aha(ptr[i]);
                cout << "In print the bitmap is "<< aha << endl;
                Record* record = (Record*)malloc(sizeof(Record));
                read_fixed_len_page(page, free_slot_pos, record);
                show_single_record(record);
                //free(record);
            }
            free_slot_pos++;
        }
        data_slots_total_count -= 8;
    }
}

int search_page(Page* page, int attribute_id, char* start, char* end){
    int data_slots_total_count = fixed_len_page_capacity(page);
    int bytes_to_check = ceil(data_slots_total_count / 8.0);
    char* ptr = (char*) page->data;
    int free_slot_pos = 0;
    int matched = 0;

    for (int i = 0; i < bytes_to_check; i++) {
        int end_limit = 8;
        if (data_slots_total_count < end_limit) {
            end_limit = data_slots_total_count;
        }

        for (int j = 0; j < end_limit; j++) {
            
            if (((ptr[i] >> j) & 1) == 1) {
                bitset<8> aha(ptr[i]);
                cout << "In print the bitmap is "<< aha << endl;
                Record* record = (Record*)malloc(sizeof(Record));
                read_fixed_len_page(page, free_slot_pos, record);
                char* attribute = (char*)malloc(ATTRIBUTE_SIZE);
                memcpy(attribute,record->at(attribute_id),ATTRIBUTE_SIZE);
                if(strcmp(attribute, start) >= 0 && strcmp(attribute, end) <= 0){
                    matched++;
                    char* printout = (char*)malloc(5);
                    memcpy(printout, attribute, 5);
                    cout << printout << endl;
                }
                //free(record);
            }
            free_slot_pos++;
        }
        data_slots_total_count -= 8;
    }

    return matched;
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
    int next_directory_id = 0;
    int freespace = page_size;
    
    fwrite(&next_directory_id,sizeof(int),1,file);
    for(int i=0; i<num_directory_entries_per_page; i++){
        fwrite(&i, sizeof(int), 1, file);
        fwrite(&freespace, sizeof(int), 1, file);
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
    fseek(file_ptr,0,SEEK_SET);
    fread(&next_did,sizeof(int),1,file_ptr);
    int search_scope = last_directory_pid(current_did,page_size);
    //cout << "Alloc page running!!" << endl;    
    while(current_pid < search_scope){
        /*
        cout << "--------" << endl;
        cout << "Th current directory id: " << current_did << endl;
        cout << "The current pid is: " << current_pid << endl;
        cout << "The search scope is: " << search_scope << endl;
        cout << "--------" << endl; 
        */
        //Check for available pages
        int freespace;
        fseek(file_ptr, directory_entry_offset(current_pid, page_size), SEEK_SET);
        fread(&current_pid,sizeof(int),1,file_ptr);
        fread(&freespace,sizeof(int),1,file_ptr);
        if(freespace == page_size){
            cout <<"pid is found: " << current_pid << endl; 
            return current_pid;
        }
        else
           current_pid++;
         
        //Search the next page
        if(current_pid == search_scope){
            //cout << "Reach end of page!!!!!!!!!!!!!!!!!!!!" << endl;
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
                //cout << "Creating new page!!!!!!!!" << endl;
                int pos_last_directory_page = directory_page_offset(current_did-1,page_size);
                fseek(file_ptr,pos_last_directory_page,SEEK_SET);
                fwrite(&current_did,sizeof(int),1,file_ptr);
                int pos_new_directory_page = directory_page_offset(current_did,page_size);
                fseek(file_ptr,pos_new_directory_page,SEEK_SET);
                fwrite(&next_did,sizeof(int),1,file_ptr);
                init_directory_page(file_ptr,page_size,current_pid);
            }

            int pos_next_iteration = directory_page_offset(current_did,page_size) + sizeof(int);
        }
        //cout <<"After pid is: " << current_pid << endl;   
    }
    
    return -1;

};

void read_page(Heapfile *heapfile, PageID pid, Page *page){
    FILE* file_ptr = heapfile->file_ptr;
    int page_size = heapfile->page_size;
    int pos_page = data_page_offset(pid, page_size);

    fseek(file_ptr, pos_page, SEEK_SET);
    int read_bytes = fread(page->data, page_size, 1, file_ptr);
    
    if(read_bytes == 0){
        memset(page->data,0,page_size);
    }
    
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
    cout << "The freespace is "<< freespace << endl;
    //int freespace = 8;
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

void init_directory_page(FILE* file, int page_size, PageID start){
    int num_directory_entries_per_page = entry_per_directory_page(page_size);
    int freespace = page_size;
    
    for(int i=start; i< start + num_directory_entries_per_page; i++){
        fwrite(&i, sizeof(int), 1, file);
        fwrite(&freespace, sizeof(int), 1, file);
    }
    return;
}

int read_csv_records(FILE* file, std::vector<Record*>* records){
    while(!feof(file)){
        Record* record = new Record;
        int i = 0;
        
        for(i;i<ATTRIBUTE_NUM;i++){
            char* attribute = (char*)malloc(ATTRIBUTE_SIZE);
            if(fread(attribute, ATTRIBUTE_SIZE, 1, file) == 0){
                break;
            }
            if(ferror(file)){
                return -1;
            }

            fgetc(file);
            record->push_back(attribute);
        }
        
        if(i==ATTRIBUTE_NUM){
            records->push_back(record);
        }
        //cout << "Pushed back: " << i << endl;
    }
    return 0;
}

void show_records(std::vector<Record*>* records){
    for(int i=0;i<(records->size());i++){
        for(int m=0;m<(records->at(i))->size();m++){
            cout << records->at(i)->at(m);
            cout << " || ";
        }
        cout << endl;
    }
}

void show_single_record(Record* record){
    for(int m=0; m<record->size(); m++){
        cout << record->at(m);
        cout << " || ";
    }
    cout << endl;
}

PageID alloc_space(Heapfile *heapfile, int space_size){
    FILE* file_ptr = heapfile->file_ptr;
    int page_size = heapfile->page_size;

    //Set up variables to loop though the file
    PageID current_pid = 0;
    int current_did = 0;
    int next_did = 0;

    //Read the first page
    fseek(file_ptr,0,SEEK_SET);
    fread(&next_did,sizeof(int),1,file_ptr);
    int search_scope = last_directory_pid(current_did,page_size);
    //cout << "Alloc page running!!" << endl;    
    while(current_pid < search_scope){
        /*
        cout << "--------" << endl;
        cout << "Th current directory id: " << current_did << endl;
        cout << "The current pid is: " << current_pid << endl;
        cout << "The search scope is: " << search_scope << endl;
        cout << "--------" << endl; 
        */
        //Check for available pages
        int freespace;
        fseek(file_ptr, directory_entry_offset(current_pid, page_size), SEEK_SET);
        fread(&current_pid,sizeof(int),1,file_ptr);
        fread(&freespace,sizeof(int),1,file_ptr);
        if(freespace >= space_size){
            cout <<"pid is found: " << current_pid << endl;
            cout <<"freespace is: " << freespace<< endl; 
            return current_pid;
        }
        else
           current_pid++;
         
        //Search the next page
        if(current_pid == search_scope){
            //cout << "Reach end of page!!!!!!!!!!!!!!!!!!!!" << endl;
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
                //cout << "Creating new page!!!!!!!!" << endl;
                int pos_last_directory_page = directory_page_offset(current_did-1,page_size);
                fseek(file_ptr,pos_last_directory_page,SEEK_SET);
                fwrite(&current_did,sizeof(int),1,file_ptr);
                int pos_new_directory_page = directory_page_offset(current_did,page_size);
                fseek(file_ptr,pos_new_directory_page,SEEK_SET);
                fwrite(&next_did,sizeof(int),1,file_ptr);
                init_directory_page(file_ptr,page_size,current_pid);
            }

            int pos_next_iteration = directory_page_offset(current_did,page_size) + sizeof(int);
        }
        //cout <<"After pid is: " << current_pid << endl;   
    }
    
    return -1;

};
