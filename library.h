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
}Page;

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

/******** Section 4 *******/

void init_heapfile(Heapfile *heapfile, int page_size, FILE *file);
PageID alloc_page(Heapfile *heapfile);
void read_page(Heapfile *heapfile, PageID pid, Page *page);
void write_page(Page *page, Heapfile *heapfile, PageID pid);
