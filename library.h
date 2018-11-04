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
