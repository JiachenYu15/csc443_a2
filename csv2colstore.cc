#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <vector>
#include <string>
#include "library.h"
using namespace std;

int main(int argc, char** argv){
    if(argc != 4){
        fprintf(stderr, "Usage: %s <csv_file> <colstore_name> <page_size>\n", argv[0]);
        return -1;
    }
    
    int page_size = atoi(argv[3]);
    char* colstore_name = argv[2];
    char command[50];
    sprintf(command, "mkdir %s", colstore_name);
    if(system(command)){
        return -1;
    }
    cout << "Directory " << colstore_name << " is created." << endl;
    
    //load records from csv_file
    FILE* csv_file = fopen(argv[1], "r");
    vector<Record*> records;
    read_csv_records(csv_file, &records);
    char* id_temp =(char*)malloc(ATTRIBUTE_SIZE);

    Heapfile* heapfile = (Heapfile*)malloc(sizeof(Heapfile));
    
    for(int i = 0; i < ATTRIBUTE_NUM; i++){
        char path[200];
        sprintf(path, "%s/%d", colstore_name, i);
        FILE* attribute_heap = fopen(path, "w+b");
        init_heapfile(heapfile, page_size, attribute_heap);

        PageID pid = alloc_page(heapfile);
        Page* page = (Page*)malloc(sizeof(Page));
        init_fixed_len_page(page, page_size, 2*ATTRIBUTE_SIZE);
        read_page(heapfile, pid, page);

        for(int m=0; m<records.size(); m++){
            std::string s = std::to_string(m);
            const char* csd = s.c_str();
            memcpy(id_temp,csd,s.length());
            Record record;
            record.push_back(id_temp);
            record.push_back(records.at(m)->at(i));
            cout << "Record size is " << record.size() << endl;
            if(add_fixed_len_page(page, &record) == -1){
               cout << "Page " << pid << "is full after insertion of " << i << " " << m << endl; 
               write_page(page, heapfile, pid);
               pid = alloc_page(heapfile);
               read_page(heapfile, pid, page);
               int second = add_fixed_len_page(page, &record);
               cout << "Second attempt returns: " << second << endl;
            }
            cout << "Attribute " << i << " Record " << m << " stored at page: " << pid << endl;
        }
        
        write_page(page, heapfile, pid);
    }
    free(id_temp);
    fclose(csv_file);
    return 0;
}
