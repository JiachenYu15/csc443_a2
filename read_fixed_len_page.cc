#include <iostream>
#include <fstream>
#include <sstream>
#include <istream>
#include <sys/timeb.h>
#include "library.h"
#include <string>
using namespace std;

/**
 * Measure system time in the units of milliseconds
 */
long current_time() {
    struct timeb t;
    ftime(&t);
    return t.time * 1000 + t.millitm;
}


int main(int argc, const char *argv[]) {
    if (argc != 3) {
        cout << "Wrong number of arguments (" << (argc - 1) << " for 3)\n";
        return 1;
    }

    // Argument 1: page_file
    ifstream page_file;
    page_file.open(argv[1], ios::binary);

    // Argument 2: page_size
    int page_size = stoi(argv[2]);

    FILE *output_file = fopen("output.csv", "w");
    long start_time = current_time();

    while (!page_file.eof()){
        // Initiate page and read from page file to page
        Page page;
        init_fixed_len_page(&page, page_size, 1000);
        page_file.read((char *) page.data, page_size);

        // Check bitmap and only write the record that has been written to csv
        for (int i = 0; i < fixed_len_page_capacity(&page); i++) {
            if (!check_slot_availability(&page, i)) {
                Record record;
                read_fixed_len_page(&page, i, &record);

                // Put record to csv file
                for (int j = 0; j < record.size(); j++) {
                    fputs(record.at(j), output_file);

                    // Append semicolon if the attribute is not the last one
                    if (j != record.size() - 1){
                        fputs(",", output_file);
                    }
                }
                // Append new line for each record
                fputs("\n", output_file);
            }
        }
    }
    fclose(output_file);
    page_file.close();

    long end_time = current_time();
    long duration = end_time - start_time;

    std::cout << "TIME: " << duration << " milliseconds\n";

    return 0;
}
