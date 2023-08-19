#include "../inc/assembler_first.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <iomanip>

#include "../inc/assembler.h"
#include "../inc/constants.h"
#include "../inc/structures.h"

using namespace std;



int AssemblerFirst::get_symbol_index(const string &name) {
  for (int i = 0; i < symbol_table.size(); i++) {
    if (symbol_table[i].name == name) {
      return i;
    }
  }
  return -1;
}

int AssemblerFirst::add_to_symbol_table(const string &name) {
  symbol_record record(name);
  symbol_table.push_back(record);
  
  return symbol_table.size() - 1;
}

void AssemblerFirst::process_global_dir(const string &name) {
  int symbol_index = get_symbol_index(name);
  bool isNew = symbol_index == -1;

  if (isNew) {
    symbol_index = add_to_symbol_table(name);
    symbol_table[symbol_index].isGlobal = true;
  }
}

void AssemblerFirst::process_extern_dir(const string &name) {
  int symbol_index = get_symbol_index(name);
  bool isNew = symbol_index == -1;
  //cout <<"PROCESS EXTERN DIR: " << endl;
  if (isNew) {
    //cout << "SYMBOL IS NEW "<< endl;

    symbol_index = add_to_symbol_table(name);

    symbol_table[symbol_index].isGlobal = true;
    symbol_table[symbol_index].isExtern = true;
  }
}

void AssemblerFirst::process_section_dir(const string &name) {
  if (curr_section != 0) {
    section_table[curr_section].size = location_counter;
  }
  location_counter = 0;
  curr_section = section_table.size();
  section_record record(name);
  section_table.push_back(record);
}

void AssemblerFirst::process_word_dir(const string &name) {
  location_counter += 4;
}
void AssemblerFirst::process_word_dir(const int literal) {
  location_counter += 4;
}
void AssemblerFirst::process_skip_dir(const int literal) {
  location_counter += literal;
}

void AssemblerFirst::process_end_dir() {
  if (curr_section != 0) {
    section_table[curr_section].size = location_counter;
  }
  merge_tables();
  location_counter = 0;
  print();
}

void AssemblerFirst::print() {
  std::ofstream outputFile("tables.txt");
  if (!outputFile) {
    // Handle file opening error
    return;
  }

  // Print symbol table
  outputFile << "# Symbol Table\n";
  outputFile << "------------------------------------------------------------\n";
  outputFile << std::left << std::setw(10) << "Name";
  outputFile << std::left << std::setw(10) << "Section";
  outputFile << std::left << std::setw(10) << "Offset";
  outputFile << std::left << std::setw(10) << "Global";
  outputFile << std::left << std::setw(10) << "Extern";
  outputFile << "\n------------------------------------------------------------\n";

  for (int i = 0; i < symbol_table.size(); i++) {
    outputFile << std::left << std::setw(10) << symbol_table[i].name;
    outputFile << std::left << std::setw(10) << symbol_table[i].section_id;
    outputFile <<"  "<< std::left << std::setw(10) << symbol_table[i].offset;
    outputFile <<" "<< std::left << std::setw(10) << symbol_table[i].isGlobal;
    outputFile << std::left << std::setw(10) << symbol_table[i].isExtern;
    outputFile << '\n';
  }

  outputFile << "\n\n";

  // Print section table
  outputFile << "# Section Table\n";
  outputFile << "------------------------------------------------------------\n";
  outputFile << std::left << std::setw(10) << "Name";
  outputFile << std::left << std::setw(10) << "Size";
  outputFile << "\n------------------------------------------------------------\n";

  for (int i = 0; i < section_table.size(); i++) {
    outputFile << std::left << std::setw(10) << section_table[i].name;
    outputFile << std::left << std::setw(10) << section_table[i].size;
    outputFile << '\n';
  }

  outputFile.close();
}

void AssemblerFirst::process_label(const string &name) {
  string label_name = name.substr(0, name.length() - 1);
  int i = get_symbol_index(label_name);
  bool isNew = i == -1;

  if (!isNew) {
    if (symbol_table[i].section_id != 0) {
      cout << "Label is already defined as symbol somewhere\n";
      exit(-1);
    }
    symbol_table[i].section_id = curr_section;
    symbol_table[i].offset = location_counter;
  } else {
    i = add_to_symbol_table(label_name);
    symbol_table[i].offset = location_counter;
    symbol_table[i].section_id = curr_section;
  }
}

void AssemblerFirst::process_instruction(const int count) {
  location_counter += INSTR_SIZE * count;
}
