#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <vector>
#include <string>
using namespace std;


struct symbol_record{
  symbol_record(const string &n);

  string name;
  int section_id = 0;
  int  offset = 0;
  bool isGlobal = false;
  bool isExtern = false;
  bool isSection = false;
};

struct relocation_record{
  relocation_record(const int offset,const int sym_id);

  int offset; //offset u sekciji gde stavljam
  int symbol_id;
  int addend = 0;
  bool type = 0; // 0 for absolute, 1 for relative
};

struct section_record{
  section_record(const string &n);

  string name;
  int size = 0;
  vector<char> memory_content;
  vector<relocation_record> section_relocs;
  bool linkerDone = false;
  unsigned int base_address = 0;

};
struct object_file_record{
  vector<symbol_record> symbol_table;
  vector<section_record> section_table;
  int section_num = 0;
  bool file_processed = false;
  string name;

};

struct linker_memory_record{
  linker_memory_record(const string &n);

  vector<char> memory_content;
  unsigned int base_address = 0;
  string section_name;
};

struct emulator_memory_record{
  unsigned int adress;
  char value;
  bool next_is_consecutive;
};

struct instruction{
  instruction(){}

  unsigned char opcode_mode;
  unsigned char A;
  unsigned char B;
  unsigned char C;
  unsigned int D;
};
#endif