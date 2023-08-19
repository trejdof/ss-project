#ifndef LINKER_H
#define LINKER_H

#include "../inc/structures.h"

#include <vector>
#include <string>

using namespace std;

class Linker {
public:
    Linker() {}
    ~Linker() {}

    static void parseSymbolTable(ifstream&, object_file_record&);
    static void parseSectionContent(ifstream&, object_file_record&);
    static void parseRelocationTables(ifstream&, object_file_record&);

    static void fill_memory_from_zero_address() {}
    static void fill_memory(vector<pair<string,unsigned>>);

    static vector<object_file_record> object_files;
    static vector<linker_memory_record> linker_memory;

    static void resolve_symbols();
    static void resolve_relocations();
    static void make_linker_output(ofstream&);

    
protected:
    
private:
    static section_record* find_section_in_file(vector<section_record>&, string );
    static void check_if_overlap(vector<pair<string,unsigned>>);
    static void fill_sections_from_command_line(vector<pair<string,unsigned>>);
    static void fill_remaining_sections(unsigned int);
    static unsigned int find_adress_to_place_sections(vector<pair<string,unsigned>>);
    
    static void get_section_names(vector<string>&); 
    static void resolve_local_or_global_symbol(symbol_record&);

    static void resolve_extern_symbol(symbol_record&);
    static void resolve_global_symbol(symbol_record&);
    static void resolve_local_symbol(symbol_record&);
    static int find_section_in_linker_memory(string);

    static int fillCurrentLineFirst(bool&, linker_memory_record&, ofstream&);
    static int fillWithZerosFirst(bool&, linker_memory_record&, ofstream&);
    static int writeLines(bool&, linker_memory_record&, ofstream&);


};
#endif