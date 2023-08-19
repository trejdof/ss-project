#include "../inc/helper_testing.h"
#include "../inc/linker.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <iomanip>

using namespace std;

vector<object_file_record> Linker::object_files;

int main(int argc, char* argv[])
{
    if (argc < 5) {
        cout << "Wrong number of arguments\n";
        return -1;
    }

    // find index of output file in command line
    int output_index = -1;
    for (int i = 1; i < argc; ++i) {
        if (string(argv[i]) == "-o") {
            output_index = ++i;
            break;
        }
    }

    // set output file name
    string output_file = string(argv[output_index]);

    // Extract section names and places from command line 
    vector<pair<string, unsigned>> section_places;
    for (int i = 2; i < output_index - 1; i++) {
        string argument = string(argv[i]);
        pair<string, unsigned> myPair = extract_name_and_address(argument);
        section_places.push_back(myPair);
    }
    
    sortSectionPlaces(section_places);

    // Extract input file names
    vector<string> input_files;
    for (int i = ++output_index; i < argc; i++) {
        input_files.push_back(string(argv[i]));
    }

    // cout << "Output file name: " << output_file << endl;
    // cout << "Input file names: ";
    // for (string filename : input_files) {
    //     cout << filename << " ";
    // }
    // cout << endl;
    
    cout << "Pairs of SECTION_NAME   ADDRESS: " << endl;
    for (pair<string, unsigned> myPair : section_places) {
        cout << "\t\t" << myPair.first << "\t" << hex << myPair.second << endl;
        cout << dec;
    }

    // ______________________________PARSING INPUT FILES______________________________
    for (string input : input_files) {
        //cout << input << endl;
        ifstream file(input);
        if (!file.is_open()) {
            cout << "ERROR OPENNING FILE" << endl;
            exit(-1);
        }

        string output_text_file = input.substr(0, input.size() - 1) + "txt";
        //cout << output_text_file << endl;

        std::ofstream outputFile(output_text_file);

        object_file_record record;
        record.name = input;

        Linker::parseSymbolTable(file, record);
        Linker::parseSectionContent(file, record);
        Linker::parseRelocationTables(file, record);

        print_symbol_table_test(outputFile, record);
        //print_section_content_test(outputFile, record);
        //print_relocations_test(outputFile, record);

        file.close(); // Close the file when you're done+
        Linker::object_files.push_back(record);
    }


    // ______________________________Filling memory content______________________________
    if (section_places.size() == 0)
        Linker::fill_memory_from_zero_address();
    Linker::fill_memory(section_places);



    for (int i = 0; i < Linker::linker_memory.size(); i++) {
        cout << left << setw(20) << Linker::linker_memory[i].section_name;
        cout << left << setw(20) << Linker::linker_memory[i].base_address;
        cout << left << Linker::linker_memory[i].memory_content.size() << endl;
    }

    // ______________________________SYMBOL RESOLVE______________________________
    Linker::resolve_symbols();
     for (object_file_record& obj : Linker::object_files) {
        

         string output_text_file = obj.name + "_linker.txt";
        

         std::ofstream outputFile(output_text_file);

         print_symbol_table_test(outputFile, obj);
         //print_section_content_test(outputFile, record);
         //print_relocations_test(outputFile, record);

     }
    cout << "Ime sekcije linker memory record: " << Linker::linker_memory[4].section_name << endl;
    cout << "Bazna adresa sekcije u linker memory record: " << hex << Linker::linker_memory[4].base_address << endl;
    cout << endl;
    cout << "Ime sekcije u object file record: " << Linker::object_files[3].section_table[1].name << endl;
    cout << "Bazna adresa sekcije  u object file record: " << Linker::object_files[3].section_table[1].base_address << endl;

    cout << endl;
    cout << "Ime sekcije u object file record: " << Linker::object_files[4].section_table[1].name << endl;
    cout << "Bazna adresa sekcije  u object file record: " << Linker::object_files[4].section_table[1].base_address << endl;
    cout << dec;

    Linker::resolve_relocations();


    ofstream linker_output(output_file);
    if (!linker_output) {
        std::cout << "Failed to open the file." << endl;
    return 1;
    }

    Linker::make_linker_output(linker_output);

    
}