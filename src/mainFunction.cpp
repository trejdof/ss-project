#include <fstream>
#include <iostream>
#include "../inc/assembler.h"
#include "../inc/assembler_first.h"
#include "../inc/assembler_second.h"

using namespace std;

extern int yyparse(void);
extern FILE *yyin;
FILE *inputFile;
int main(int argc, char *argv[]) {

  if (argc < 2) {
    cout << "Wrong number of arguments\n";
    return -1;
  }

  inputFile = fopen(argv[3], "r");
  //
 
  if (!inputFile) {
    
    cout << "I can't open a file!" << endl;
    return -1;
  }

  string output_path = "assembler-output/" + (string)argv[2];
  //cout << output_path << endl;
  ofstream outputFile(output_path);
  if (!outputFile) {
    std::cout << "Failed to open the file." << endl;
    return 1;
  }


  extern Assembler* assembler;
  assembler->init_symbol_table();
  assembler = new AssemblerFirst();
  
  yyin = inputFile;
  yyparse();
  assembler = new AssemblerSecond();
  fseek(inputFile, 0, SEEK_SET);
  yyin = inputFile;
  yyparse();

  Assembler::print_elf_file(outputFile);


  
  return 0;
}
