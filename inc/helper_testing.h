#ifndef HELPER_TESTING_H
#define HELPER_TESTING_H
#include "../inc/structures.h"


#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>
using namespace std;

void print_symbol_table_test(ofstream& file, object_file_record&);
void print_section_content_test(ofstream& file, object_file_record&);
void print_relocations_test(ofstream&file, object_file_record&);
pair<string,unsigned> extract_name_and_address(string argument);
bool compareBySecond(const pair<string, unsigned>&, const pair<string, unsigned>&);
void sortSectionPlaces(vector<pair<string, unsigned>>& );


#endif
