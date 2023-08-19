#include "../inc/assembler.h"
#include "../inc/assembler_first.h"
#include "../inc/structures.h"
#include <iostream>
#include <fstream>
#include <iomanip>


symbol_record::symbol_record(const string& n) : name(n) {}
section_record::section_record(const string& n) : name(n) {}
relocation_record::relocation_record(const int offset, const int sym_id)
: offset(offset), symbol_id(sym_id) {}


std::vector<symbol_record> Assembler::symbol_table;
std::vector<section_record> Assembler::section_table;
std::vector<symbol_record> Assembler::elf_symbol_table;



void Assembler::process_label(const string& name){}

void Assembler::process_instruction(const int count) {}

void Assembler::merge_tables(){
  elf_symbol_table = symbol_table;
  // fill sections
  for (int i = section_table.size() - 1; i >= 0; i--) {
    symbol_record temp = symbol_record(section_table[i].name);
    temp.isGlobal = false;
    temp.isSection = true;
    temp.section_id = i;
    elf_symbol_table.insert(elf_symbol_table.begin(), temp);
    
  }
}

void Assembler::print_elf_file(ofstream& outputFile)
{
  //std::ofstream outputFile("elf.txt");
  if (!outputFile) {
    // Handle file opening error
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

  for (int i = 0; i < elf_symbol_table.size(); i++) {
    outputFile << std::left << std::setw(10) << i;
    outputFile << std::left << std::setw(10) << elf_symbol_table[i].offset;

    if(elf_symbol_table[i].isSection)outputFile << std::left << std::setw(10) << "SCTN";
    else outputFile << std::left << std::setw(10) << "NOTYP";

    if(elf_symbol_table[i].isGlobal)outputFile << std::left << std::setw(10) << "GLOB";
    else outputFile << std::left << std::setw(10) << "LOC";

    if(elf_symbol_table[i].section_id == 0 )outputFile << std::left << std::setw(10) << "UND";
    else outputFile << std::left << std::setw(10) << elf_symbol_table[i].section_id;

    outputFile <<"  "<< std::left << std::setw(10) << elf_symbol_table[i].name;
    outputFile << '\n';
  }

  // SECTIONS
  outputFile << endl;
  for (int i = 1; i < section_table.size(); i++) {
      outputFile << "#." << section_table[i].name << endl;
      int j = 0;
      for (; j < section_table[i].memory_content.size(); j++) {
        outputFile << setw(2) << setfill('0') << right << hex << 
          static_cast<int>(static_cast<unsigned char>(section_table[i].memory_content[j])) << " ";
        if ((j + 1) % 8 == 0 ) outputFile << "\n";
      
      }
      outputFile << endl;
      if (j % 8 != 0) outputFile << "\n";
      outputFile << std::dec << std::setfill(' ');
  }

  //Relocations
  for (int i = 1; i < section_table.size(); i++) {
    outputFile << "#.rela." << section_table[i].name << endl;
    outputFile << std::left << std::setw(10) << "Offset"
                << setw(10) << "Type" 
                << setw(10) << "Symbol"
                << setw(10) << "Addend" << endl;   
    for(int j = 0; j < section_table[i].section_relocs.size(); j++){
      outputFile << std::setw(10) << section_table[i].section_relocs[j].offset
                   << std::setw(10) << section_table[i].section_relocs[j].type
                   << std::setw(10) << section_table[i].section_relocs[j].symbol_id
                   << std::setw(10) << section_table[i].section_relocs[j].addend << std::endl;

    }
    outputFile << endl;

  }
   

    outputFile.close();

  outputFile << "\n\n";
}

void Assembler::init_symbol_table() {
  section_record record("UND");
  
  section_table.push_back(record);
}
