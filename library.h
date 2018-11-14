#include <stdio.h>
#include <vector>

/*
 * Data strcutre declaration
 */

/******* Section 2 ********/

typedef const char* V;
typedef std::vector<V> Record;

/******* Section 3 ********/

typedef struct {
    void *data;
    int page_size;
    int slot_size;
    int directory_slots_count;
} Page;

/******* Section 4 ********/

typedef struct {
    FILE *file_ptr;
    int page_size;
}Heapfile;

typedef int PageID;

typedef struct {
    int page_id;
    int slot;
}RecordID;

class RecordIterator{
    public:
        RecordIterator(Heapfile *heapfile);
        Record next();
        bool hasNext();
};

/*
 * Functiton declarations
 */

/******** Section 2 *******/

/**
 * Compute the number of bytes required to serialize record
 */
int fixed_len_sizeof(Record *record);


/**
 * Serialize the record to a byte array to be stored in buf.
 */
void fixed_len_write(Record *record, void *buf);


/**
 * Deserializes `size` bytes from the buffer, `buf`, and
 * stores the record in `record`.
 */
void fixed_len_read(void *buf, int size, Record *record);

/******** Section 3 *******/

/**
 * Initializes a page using the given slot size
 */
void init_fixed_len_page(Page *page, int page_size, int slot_size);
 
/**
 * Calculates the maximal number of records that fit in a page
 */
int fixed_len_page_capacity(Page *page);
 
/**
 * Calculate the free space (number of free slots) in the page
 */
int fixed_len_page_freeslots(Page *page);
 
/**
 * Add a record to the page
 * Returns:
 *   record slot offset if successful,
 *   -1 if unsuccessful (page full)
 */
int add_fixed_len_page(Page *page, Record *r);
 
/**
 * Write a record into a given slot.
 */
void write_fixed_len_page(Page *page, int slot, Record *r);
 
/**
 * Read a record from the page from a given slot.
 */
void read_fixed_len_page(Page *page, int slot, Record *r);

/******** Section 4 *******/

void init_heapfile(Heapfile *heapfile, int page_size, FILE *file);
PageID alloc_page(Heapfile *heapfile);
void read_page(Heapfile *heapfile, PageID pid, Page *page);
void write_page(Page *page, Heapfile *heapfile, PageID pid);

/******** Helpful Functions ********/

int entry_per_directory_page(int page_size);
PageID last_directory_pid(int directory_id, int page_size);
int directory_page_offset(int directory_id, int page_size);
int data_page_offset(PageID page_id, int page_size);
int page_directory_id(PageID page_id, int page_size);
int directory_page_offset(PageID page_id, int page_size);
int directory_entry_offset(PageID page_id, int page_size);
void init_directory_page(FILE* file, int page_size);
