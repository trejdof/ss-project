#include "../inc/linker.h"
#include "../inc/structures.h"
#include "../inc/helper_testing.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
using namespace std;

void print_symbol_table_test(ofstream& outputFile, object_file_record& obj_record)
{
  //std::ofstream outputFile("linker_sym_table.txt");
  if (!outputFile) {
    cout << "CANNOT OPEN THE FILE" << endl;
    return ;
  }

  // Print symbol table
  outputFile << "#.symtab\n";
  outputFile << "------------------------------------------------------------\n";
  outputFile << std::left << std::setw(10) << "Num";
  outputFile << std::left << std::setw(10) << "Value";
  outputFile << std::left << std::setw(10) << "Type";
  outputFile << std::left << std::setw(10) << "Bind";
  outputFile << std::left << std::setw(10) << "Ndx";
  outputFile << std::left << std::setw(10) << "Name";
  outputFile << "\n------------------------------------------------------------\n";
  
  for (int i = 0; i < obj_record.symbol_table.size(); i++) {
    outputFile << std::left << std::setw(10) << i;
    outputFile << std::left << std::setw(10) << (unsigned)obj_record.symbol_table[i].offset;
    outputFile << dec;

    if(obj_record.symbol_table[i].isSection)outputFile << std::left << std::setw(10) << "SCTN";
    else outputFile << std::left << std::setw(10) << "NOTYP";

    if(obj_record.symbol_table[i].isGlobal)outputFile << std::left << std::setw(10) << "GLOB";
    else outputFile << std::left << std::setw(10) << "LOC";

    if(obj_record.symbol_table[i].section_id == 0 )outputFile << std::left << std::setw(10) << "UND";
    else outputFile << std::left << std::setw(10) << obj_record.symbol_table[i].section_id;

    outputFile <<"  "<< std::left << std::setw(10) << obj_record.symbol_table[i].name;
    outputFile << '\n';
  }
}

void print_section_content_test(ofstream& outputFile, object_file_record& obj_record){
  //std::ofstream outputFile("linker_sym_table.txt");
  if (!outputFile) {
    cout << "CANNOT OPEN THE FILE" << endl;
    return ;
  }
  outputFile << endl;
  for (int i = 1; i < obj_record.section_table.size(); i++) {
      outputFile << "#." << obj_record.section_table[i].name << endl;
      int j = 0;
      for (; j < obj_record.section_table[i].memory_content.size(); j++) {
        outputFile << setw(2) << setfill('0') << right << hex << 
          static_cast<int>(static_cast<unsigned char>(obj_record.section_table[i].memory_content[j])) << " ";
        if ((j + 1) % 8 == 0 ) outputFile << "\n";
      
      }
      outputFile << endl;
      if (j % 8 != 0) outputFile << "\n";
      outputFile << std::dec << std::setfill(' ');
  }
}

void print_relocations_test(ofstream &outputFile, object_file_record& obj_record) {
  if (!outputFile) {
    cout << "CANNOT OPEN THE FILE" << endl;
    return ;
  }

  for (int i = 1; i < obj_record.section_table.size(); i++) {
    outputFile << "#.rela." << obj_record.section_table[i].name << endl;
    outputFile << std::left << std::setw(10) << "Offset"
                << setw(10) << "Type" 
                << setw(10) << "Symbol"
                << setw(10) << "Addend" << endl;   
    for(int j = 0; j < obj_record.section_table[i].section_relocs.size(); j++){
      outputFile << std::setw(10) << obj_record.section_table[i].section_relocs[j].offset
                   << std::setw(10) << obj_record.section_table[i].section_relocs[j].type
                   << std::setw(10) << obj_record.section_table[i].section_relocs[j].symbol_id
                   << std::setw(10) << obj_record.section_table[i].section_relocs[j].addend << std::endl;

    }
    outputFile << endl;

  }
}
pair<string,unsigned> extract_name_and_address(string argument){

  pair<string,unsigned> returnValue;

  // Extracting the "data" string
  std::string delimiter = "=";
  size_t pos = argument.find(delimiter);
  std::string extractedString = argument.substr(pos + delimiter.length());

  // Extracting the hexadecimal number
  delimiter = "@";
  pos = extractedString.find(delimiter);
  std::string text = extractedString.substr(0, pos);
  std::string hexString = extractedString.substr(pos + delimiter.length());
  std::istringstream iss(hexString);
  unsigned int hexNumber;
  iss >> std::hex >> hexNumber;
  
  
  returnValue.first = text;
  returnValue.second = hexNumber;

  return returnValue;

}
bool compareBySecond(const std::pair<std::string, unsigned>& lhs, const std::pair<std::string, unsigned>& rhs) {
    return lhs.second < rhs.second;
}

void sortSectionPlaces(std::vector<std::pair<std::string, unsigned>>& section_places) {
    std::sort(section_places.begin(), section_places.end(), compareBySecond);
}

