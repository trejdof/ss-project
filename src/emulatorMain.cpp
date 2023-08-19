#include "../inc/emulator.h"
#include "../inc/structures.h"
#include "../inc/constants.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <iomanip>



using namespace std;

vector<unsigned int> registers;
vector<unsigned int> csr_registers;
vector <emulator_memory_record> memory;


int main(int argc, char** argv) {

    if (argc < 2) {
        cout << "Invalid number of arguments!\n";
        exit(-1);
    }
    // Open file to read from
    ifstream inputFile(argv[1]);
    if (!inputFile){
      cout << "Cannot open input file!" << endl;
      exit(-1);
    }

    load_memory_content(inputFile);
    //add_address_to_context(0xFFFFFEFE, 0);
    init_registers();
    bool processing = true;
    cout << "--------------------------------------------------------------------------" << endl;
    while(processing){
      cout << endl << hex << "IZ MAINA SP: " << registers[SP] << "  IZ MAINA PC: " << registers[PC] << endl;
  
      instruction instr = get_instruction();
      //cout << hex << int(instr.opcode_mode) << "   " << (int)instr.A << "  " << (int)instr.B << "  " 
      //<< (int) instr.C << "  " << (int)instr.D << endl;
      switch(instr.opcode_mode){
        case HALT: halt(processing); break;
        case INT: int_(); break;
        case CALL | CALL_MOD0: call_mod0(instr); break;
        case CALL | CALL_MOD1: call_mod1(instr); break;
        case JUMP | JMP_M0: jmp_m0(instr); break;
        case JUMP | JMP_M1: jmp_m1(instr); break;
        case JUMP | JMP_M2: jmp_m2(instr); break;
        case JUMP | JMP_M3: jmp_m3(instr); break;
        case JUMP | JMP_M4: jmp_m4(instr); break;
        case JUMP | JMP_M5: jmp_m5(instr); break;
        case JUMP | JMP_M6: jmp_m6(instr); break;
        case JUMP | JMP_M7: jmp_m7(instr); break;
        case XCHG: xchg(instr); break;
        case ARIT | ADD_MODE: add(instr); break;
        case ARIT | SUB_MODE: sub(instr); break;
        case ARIT | MUL_MODE: mul(instr); break;
        case ARIT | DIV_MODE: div(instr); break;
        case LOGIC | NOT_MODE: not_(instr); break;
        case LOGIC | AND_MODE: and_(instr); break;
        case LOGIC | OR_MODE: or_(instr); break;
        case LOGIC | XOR_MODE: xor_(instr); break;
        case SHIFT | SHL_MODE: shl(instr); break;
        case SHIFT | SHR_MODE: shr(instr); break;
        case STORE | STORE_M0: store_m0(instr); break; 
        case STORE | STORE_M1: store_m1(instr); break; 
        case STORE | STORE_M2: store_m2(instr); break;
        case LOAD | LOAD_M0 : load_m0(instr); break;
        case LOAD | LOAD_M1 : load_m1(instr); break;
        case LOAD | LOAD_M2 : load_m2(instr); break;
        case LOAD | LOAD_M3 : load_m3(instr); break;
        case LOAD | LOAD_M4 : load_m4(instr); break;
        case LOAD | LOAD_M5 : load_m5(instr); break;
        case LOAD | LOAD_M6 : load_m6(instr); break;
        case LOAD | LOAD_M7 : load_m7(instr); break;
           
      }
      //break;
      
    }
    
    //print_context();
}