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

/******** Section 3 *******/

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
