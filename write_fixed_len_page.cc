#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/timeb.h>
#include <cstring>
#include "library.h"
using namespace std;

/**
 * Measure system time in the units of milliseconds
 */
long current_time() {
    struct timeb t;
    ftime(&t);
    return t.time * 1000 + t.millitm;
}

/*
 * Load records from a csv file, write to page, and append the page to a page file.
 */
int main(int argc, const char *argv[]) {
    if (argc != 4) {
        std::cout << "Wrong number of arguments (" << (argc - 1) << " for 4)\n";
        return 1;
    }

    // Argument 1: csv_file
    ifstream csv_file(argv[1]);

    // Argument 2: page_file
    ofstream page_file;
    page_file.open(argv[2], ios::out | ios::binary);

    // Argument 3: page_size
    int page_size = stoi(argv[3]);

    Page page;
    int initiated = 0;
    int number_of_records = 0;
    int number_of_pages = 0;
    string line;

    long start_time = current_time();
    while (getline(csv_file, line)) {
        Record record;
        stringstream line_stream(line);
        string attribute;

        while (std::getline(line_stream, attribute, ',')) {
            char *attr = (char *) calloc(11, sizeof(char));
            std::strncpy(attr, attribute.c_str(), 11);
            record.push_back(attr);
        }

        if (!initiated) {
            init_fixed_len_page(&page, page_size, fixed_len_sizeof(&record));
            number_of_pages += 1;
            initiated = 1;
        }

        if (add_fixed_len_page(&page, &record) == -1) {
            page_file.write((char *)page.data, page.page_size);
            init_fixed_len_page(&page, page_size, fixed_len_sizeof(&record));
            number_of_pages += 1;
            add_fixed_len_page(&page, &record);
        }
        number_of_records += 1;
    }
    page_file.write((char *) page.data, page.page_size);
    page_file.close();

    long end_time = current_time();
    long duration = end_time - start_time;

    std::cout << "NUMBER OF RECORDS: " << number_of_records << "\n";
    std::cout << "NUMBER OF PAGES: " << number_of_pages << "\n";
    std::cout << "TIME: " << duration << " milliseconds\n";

    return 0;
}
