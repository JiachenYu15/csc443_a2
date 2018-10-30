#include <vector>

typedef const char* V;
typedef std::vector<V> Record;

typedef struct {
    FILE *file_ptr;
    int page_size;
}Heapfile;

typedef int PageID;

typedef struct {
    int page_id;
    int slot;
}RecordID;

typedef struct {
  void *data;
  int page_size;
  int slot_size;
}Page;


