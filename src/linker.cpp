#include "../inc/linker.h"
#include "../inc/constants.h"
#include "../inc/structures.h"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

object_file_record* current_obj_file;

symbol_record::symbol_record(const string& n)
    : name(n)
{
}
section_record::section_record(const string& n)
    : name(n)
{
}
relocation_record::relocation_record(const int offset, const int sym_id)
    : offset(offset)
    , symbol_id(sym_id)
{
}
linker_memory_record::linker_memory_record(const string& n)
    : section_name(n)
{
}

vector<linker_memory_record> Linker::linker_memory;

int section_num = 0;

void Linker::parseSymbolTable(ifstream& file, object_file_record& obj_record)
{
    section_num = 0;
    string line;
    getline(file, line); // Skip the header line
    getline(file, line); // Skip ---------------
    getline(file, line); // Skip columns names
    getline(file, line); // Skip --------------
    while (getline(file, line)) {
        if (line.empty())
            break;

        stringstream ss(line);
        string num, value, type, bind, ndx, name;
        ss >> num >> value >> type >> bind >> ndx >> name;

        symbol_record record(name);
        int id = 0;
        if (ndx != "UND")
            id = stoi(ndx);
        record.section_id = id;

        record.offset = stoi(value);

        record.isGlobal = (bind == "GLOB");
        record.isSection = (type == "SCTN");
        if (type == "SCTN")
            section_num++;

        obj_record.symbol_table.push_back(record);
    }

    section_num--; // due to UND SECTION
    obj_record.section_num = section_num;
}

void Linker::parseSectionContent(ifstream& file, object_file_record& obj_record)
{
    // Initialize UND section
    section_record record("UND");
    record.linkerDone = true;
    obj_record.section_table.push_back(record);

    string line;
    int section_counter = 0;
    std::streampos currentPosition;
    while (getline(file, line) && section_counter < section_num) {
        if (line.empty())
            break;
        if (line[0] == '#') {

            section_counter++;
            section_record record(line.substr(2));

            string contentLine;
            while (getline(file, contentLine)) {
                currentPosition = file.tellg();
                if (contentLine.empty())
                    break;

                stringstream ss(contentLine);
                string hexNumber;
                while (ss >> hexNumber) {
                    //cout << hexNumber << endl;
                    char byte = static_cast<char>(stoi(hexNumber, nullptr, 16));
                    record.memory_content.push_back(byte);
                }
                record.size = record.memory_content.size();
            }
            obj_record.section_table.push_back(record);
        }
    }
    file.seekg(currentPosition);
}

void Linker::parseRelocationTables(ifstream& file, object_file_record& obj_record)
{
    string line;
    while (getline(file, line)) {
        if (line.empty())
            break;

        if (line[0] == '#') {
            string sectionName = line.substr(line.find_last_of('.') + 1);
            //cout << sectionName << endl;
            string entryLine;

            while (getline(file, entryLine)) {
                if (entryLine.empty())
                    break;
                if (entryLine[0] == 'O')
                    continue;
                stringstream ss(entryLine);
                int offset, symbol, addend;
                bool type;
                ss >> offset >> type >> symbol >> addend;

                for (section_record& section : obj_record.section_table) {
                    if (section.name == sectionName) {

                        relocation_record record(offset, symbol);
                        record.addend = addend;
                        record.type = (type != 0);

                        section.section_relocs.push_back(record);
                        break;
                    }
                }
            }
        }
    }
}

section_record* Linker::find_section_in_file(vector<section_record>& section_table, string name)
{
    for (int i = 1; i < section_table.size(); i++) {
        if (section_table[i].name == name)
            return &section_table[i];
    }
    return nullptr;
}

void Linker::check_if_overlap(vector<pair<string, unsigned>> section_places)
{
    for (int i = 0; i < linker_memory.size() - 1; i++) {
        //cout << linker_memory[i].memory_content.size() << endl;
        //cout << section_places[i].second  << endl;
        //cout << linker_memory[i + 1].base_address  << endl;

        if (linker_memory[i].memory_content.size() + section_places[i].second >= linker_memory[i + 1].base_address) {
            cout << "section overlap, section nums that overlap are: " << i << " and " << i + 1 << endl;
            exit(-1);
        }
    }
}

void Linker::fill_sections_from_command_line(vector<pair<string, unsigned>> section_places)
{
    for (pair<string, unsigned> name_address_pair : section_places) {
        unsigned int current_base = name_address_pair.second;
        //cout << "SECTION NAME  " << name_address_pair.first << endl;
        linker_memory_record record(name_address_pair.first);
        record.base_address = name_address_pair.second;

        for (object_file_record& obj : object_files) {
            section_record* section = find_section_in_file(obj.section_table, name_address_pair.first);
            if (!section)
                continue;
            section->base_address = current_base;
            current_base += section->size;
            //cout << "memory_content.size():  " << section->memory_content.size() << endl;
            for (int i = 0; i < section->memory_content.size(); i++) {

                record.memory_content.push_back(section->memory_content[i]);
                //cout << section->name << "  " << hex << section->base_address << endl;
            }
            section->linkerDone = true;
            obj.section_num--;
            if (obj.section_num == 0)
                obj.file_processed = true;
        }
        linker_memory.push_back(record);
    }
}

void Linker::fill_remaining_sections(unsigned int new_base_adress)
{
    unsigned int current_base_adress = new_base_adress;
    vector<string> section_names;
    get_section_names(section_names);
    for (string section : section_names) { // iterate through list of all sections that are not processed yet
        linker_memory_record record(section);
        record.base_address = current_base_adress;
        //cout << section << endl;
        for (object_file_record& obj : object_files) { // iterate through all obj files in order to check if target section exists
            if (obj.file_processed)
                continue;
            section_record* targetSection = find_section_in_file(obj.section_table, section);
            if (!targetSection)
                continue;
            // Place section into linker memory recrod
            //cout << "New base adress value: " << hex << current_base_adress << endl;
            //cout << dec;
            targetSection->base_address = current_base_adress;
            current_base_adress += targetSection->size;
            for (int i = 0; i < targetSection->memory_content.size(); i++) {
                record.memory_content.push_back(targetSection->memory_content[i]);
                //cout << section->name << "  " << hex << section->base_address << endl;
            }
            targetSection->linkerDone = true;
            obj.section_num--;
            if (obj.section_num == 0)
                obj.file_processed = true;
        }
        linker_memory.push_back(record);
    }
}

void Linker::get_section_names(vector<string>& section_names)
{
    for (object_file_record& obj : object_files) {
        if (obj.section_num == 0)
            continue;

        for (int i = 0; i < obj.section_table.size(); i++) {
            if (obj.section_table[i].linkerDone)
                continue;
            auto it = find(section_names.begin(), section_names.end(), obj.section_table[i].name);
            if (it != section_names.end()) {
                // Section already exists! do nothing
            } else {
                section_names.push_back(obj.section_table[i].name);
            }
        }
    }
}

unsigned int Linker::find_adress_to_place_sections(vector<pair<string, unsigned>> section_places)
{
    string name = section_places.back().first;
    unsigned int new_base_adress;
    for (int i = 0; i < linker_memory.size(); i++) {
        if (linker_memory[i].section_name == name)
            new_base_adress = linker_memory[i].memory_content.size() + linker_memory[i].base_address;
    }

    return new_base_adress;
}

void Linker::fill_memory(vector<pair<string, unsigned>> section_places)
{

    fill_sections_from_command_line(section_places);
    check_if_overlap(section_places);
    // for (object_file_record obj : object_files) {
    //     cout << obj.section_num << endl;
    // }
    unsigned int new_base_adress = find_adress_to_place_sections(section_places);
    fill_remaining_sections(new_base_adress);
}

void Linker::resolve_local_symbol(symbol_record& symbol_record)
{
    int section_id = symbol_record.section_id;
    symbol_record.offset += current_obj_file->section_table[section_id].base_address;
    cout << "LOCAL SYMBOL: " << symbol_record.name << "  VALUE: " << (unsigned)symbol_record.offset << endl;
}

void Linker::resolve_global_symbol(symbol_record& symbol_record)
{
    // TODO exit program if same symbol is declared as global in multiple files
    int section_id = symbol_record.section_id;
    symbol_record.offset += current_obj_file->section_table[section_id].base_address;
    cout << "FILE NAME: " << current_obj_file->name;
    cout << "   GLOBAL SYMBOL: " << symbol_record.name << "  VALUE: " << (unsigned)symbol_record.offset << endl;
}

void Linker::resolve_local_or_global_symbol(symbol_record& symbol_record)
{
    if (symbol_record.section_id == 0)
        return; // extern symbol cannot be resolved now;
    if (symbol_record.isGlobal)
        resolve_global_symbol(symbol_record);
    else
        resolve_local_symbol(symbol_record);
}

void Linker::resolve_extern_symbol(symbol_record& symbol_record)
{
    //iterate through all files to find global symbol with such name
    for (object_file_record& obj : object_files) {
        // iterate through all symbols in that file
        for (int i = 1; i < obj.symbol_table.size(); i++) {
            if (obj.symbol_table[i].name == symbol_record.name && obj.symbol_table[i].isGlobal && obj.symbol_table[i].section_id != 0) {
                symbol_record.offset = obj.symbol_table[i].offset;
                cout << "   EXTERN SYMBOL:  " << symbol_record.name << "  VALUE: " << (unsigned)symbol_record.offset;
                cout << "  imported from file: " << obj.name << endl;
                return;
            }
        }
    }
    cout << symbol_record.name << " EXTERN SYMBOL IS NOWHERE DEFINED AS GLOBAL" << endl;
    exit(-1);
}

void Linker::resolve_symbols()
{
    //Iterate through all object file records
    for (object_file_record& obj : object_files) {
        current_obj_file = &obj;
        //Iterate through all symbols in single file in order to resolve GLOBAL AND LOCAL SYMBOLS
        for (int i = 1; i < obj.symbol_table.size(); i++) {
            resolve_local_or_global_symbol(obj.symbol_table[i]);
        }
    }

    for (object_file_record& obj : object_files) {
        current_obj_file = &obj;
        //Iterate through all symbols in single file in order to resolve extern
        for (int i = 1; i < obj.symbol_table.size(); i++) {
            if (obj.symbol_table[i].section_id == 0) {
                cout << "FILE NAME: " << current_obj_file->name;
                resolve_extern_symbol(obj.symbol_table[i]);
            }
        }
    }
}

int Linker::find_section_in_linker_memory(string section_name)
{

    for (int i = 0; i < linker_memory.size(); i++) {
        if (linker_memory[i].section_name == section_name)
            return i;
    }
    return -1;
}

void Linker::resolve_relocations()
{
    //Iterate through all object file records
    for (object_file_record& obj : object_files) {
        //Iterate through all sections in single file
        for (int i = 0; i < obj.section_table.size(); i++) {
            // iterate through all relocations in single section
            for (int j = 0; j < obj.section_table[i].section_relocs.size(); j++) {
                int symbol_id = obj.section_table[i].section_relocs[j].symbol_id;
                unsigned int symbol_value = obj.symbol_table[symbol_id].offset;

                unsigned int local_offset = obj.section_table[i].section_relocs[j].offset;
                int index_in_linker_memory = find_section_in_linker_memory(obj.section_table[i].name);
                unsigned int real_offset = local_offset + obj.section_table[i].base_address - linker_memory[index_in_linker_memory].base_address;
                linker_memory[index_in_linker_memory].memory_content[real_offset] = GET_BYTE_1(symbol_value);
                linker_memory[index_in_linker_memory].memory_content[real_offset + 1] = GET_BYTE_2(symbol_value);
                linker_memory[index_in_linker_memory].memory_content[real_offset + 2] = GET_BYTE_3(symbol_value);
                linker_memory[index_in_linker_memory].memory_content[real_offset + 3] = GET_BYTE_4(symbol_value);
            }
        }
    }
}


int Linker::writeLines(bool& divided, linker_memory_record& record, ofstream& outputFile) {
    
    if (record.memory_content.size() % 8 == 0) divided = false;
    else divided = true;
    unsigned int current_address = record.base_address;
    
    int i = 0;
    for (; i < record.memory_content.size() / 8; i++) { // iterate through single section

        // Convert the base address to hexadecimal format
        stringstream addressStream;
        addressStream << std::hex << std::setfill('0') << std::setw(4) << current_address;
        string hexAddress = addressStream.str();

        // Output the address followed by a colon
        outputFile << hexAddress << ": ";
        
        for (int j = 0; j < 8; j++) {

            outputFile << setw(2) << setfill('0') << right << hex << static_cast<int>(static_cast<unsigned char>(record.memory_content[j + i * 8])) << " ";
            outputFile << std::dec << std::setfill(' ');

        }

        outputFile << endl;
        current_address += 8;
    }

    if(divided) { // need to print last 4 bytes

        // Convert the base address to hexadecimal format
        stringstream addressStream;
        addressStream << std::hex << std::setfill('0') << std::setw(4) << current_address;
        string hexAddress = addressStream.str();

        // Output the address followed by a colon
        outputFile << hexAddress << ": ";

        for (int j = 0; j < 4; j++) {

            outputFile << setw(2) << setfill('0') << right << hex << static_cast<int>(static_cast<unsigned char>(record.memory_content[j + i * 8])) << " ";
            outputFile << std::dec << std::setfill(' ');
        }
        
        current_address += 4;
    }

    return current_address;
}


int Linker::fillWithZerosFirst(bool& divided, linker_memory_record& record, ofstream& outputFile){

    for (int j = 0; j < 4; j++) {
        outputFile << setw(2) << setfill('0') << right << hex << static_cast<int>(static_cast<unsigned char>(0)) << " ";
        outputFile << std::dec << std::setfill(' ');
    }
    outputFile << endl;

    return writeLines(divided, record, outputFile);
}

int Linker::fillCurrentLineFirst(bool& divided, linker_memory_record& record, ofstream& outputFile){
    for (int j = 0; j < 4; j++) {
        outputFile << setw(2) << setfill('0') << right << hex << static_cast<int>(static_cast<unsigned char>(record.memory_content[j])) << " ";
        outputFile << std::dec << std::setfill(' ');
    }

    outputFile << endl;

    record.memory_content.erase(record.memory_content.begin());
    record.memory_content.erase(record.memory_content.begin());
    record.memory_content.erase(record.memory_content.begin());
    record.memory_content.erase(record.memory_content.begin());

    record.base_address += 4; // potencijalno ce zajebati degavoganje

    return writeLines(divided, record, outputFile);

}



void Linker::make_linker_output(ofstream& outputFile)
{   
    bool lastOutputDivided = false;
    bool consecutiveAdresses = false;
    unsigned int last_address = 0;
    unsigned int current_address = 0;
    for (linker_memory_record record : linker_memory) { // each record contains single merged section
        
        consecutiveAdresses = current_address == record.base_address;
        

        if(lastOutputDivided && consecutiveAdresses) {
            
            current_address = fillCurrentLineFirst(lastOutputDivided, record, outputFile);
            

        }
        else if (lastOutputDivided && !consecutiveAdresses) {
            
            current_address = fillWithZerosFirst(lastOutputDivided, record, outputFile);

        }

        else {
           current_address = writeLines(lastOutputDivided, record, outputFile);

        }
        
    }
    if (lastOutputDivided) {
        for (int j = 0; j < 4; j++) {
            outputFile << setw(2) << setfill('0') << right << hex << static_cast<int>(static_cast<unsigned char>(0)) << " ";
            outputFile << std::dec << std::setfill(' ');
        }
        outputFile << endl;
    }
}
