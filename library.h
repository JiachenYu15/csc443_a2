#include <stdio.h>
#include <vector>

/*
 * Data strcutre declaration
 */
#define ATTRIBUTE_SIZE 10
#define ATTRIBUTE_NUM 100
#define RECORD_SIZE 10*100

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
    private:
        Heapfile* heapfile;
        int current_pid;
        int next_did;
        int current_did;
        Page* page;

    public:
        RecordIterator(Heapfile *heapfile);
        int get_next_did();
        int get_current_did();
        int get_current_pid();
        Page* get_page();
        void current_pid_add_one();
        Record next();
        bool hasNext();
        ~RecordIterator();
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
/**
 * Return the number of directory slots per heapfile directory page.
 */
int entry_per_directory_page(int page_size);

/**
 * Return the last page id on the given directory page plus one. For example,
 * if the last pid on the directory page is 5, the function will return 6.
 */
PageID last_directory_pid(int directory_id, int page_size);

/**
 * Return the position of the directory page in the heapfile  given directory id.
 */
int directory_page_offset(int directory_id, int page_size);

/**
 * Return the position of the data page in the heapfile given pid.
 */
int data_page_offset(PageID page_id, int page_size);

/**
 * Return the directory of the pid it belongs.
 */
int page_directory_id(PageID page_id, int page_size);

/**
 * Return the position of the directory entry in the heapfile of the given pid.
 */
int directory_entry_offset(PageID page_id, int page_size);
void init_directory_page(FILE* file, int page_size, PageID start);
int read_csv_records(FILE* file, std::vector<Record*>* records);
void show_records(std::vector<Record*>* records);
void print_page_records(Page* page);
void show_single_record(Record* record);
PageID alloc_space(Heapfile* heapfile, int space_size);
