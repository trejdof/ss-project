#include "../inc/structures.h"
#include "../inc/constants.h"
#include "../inc/emulator.h"


#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <iomanip>
using namespace std;



void load_memory_content(ifstream& file){
  string line;
  while(getline(file,line)){
    unsigned int adress = stoul(line.substr(0,8), nullptr, 16);
    unsigned int cursor = 10;
    bool isConsecutive = true;
    if( memory.size() ) {
      isConsecutive = (adress - memory.back().adress == 1) ? true : false;
      memory.back().next_is_consecutive = isConsecutive;
    }

    for (int i = 0; i < 8; i++){
      unsigned int value = stoul(line.substr(cursor,2), nullptr, 16);

      emulator_memory_record record;
      record.adress = adress + i;
      record.value = value; 
      record.next_is_consecutive = true;
      memory.push_back(record);
      cursor += 3;

    }
  }
  file.close();
}

void init_registers() {

  for (int i = 0; i < 15; i++){
    unsigned int reg = 0;
    registers.push_back(reg);
  }
  unsigned int reg = PC_START;
  registers.push_back(reg);

  for (int i = 0; i < 3; i++){
    unsigned int reg = 0;
    csr_registers.push_back(reg);
  }
  
}

unsigned char find_target_byte(unsigned int address){
  for(int i = 0; i < memory.size(); i++){
    if (memory[i].adress == address) return memory[i].value;
  }
  cout << "NOTHING AT THIS ADDRESS" << endl;
  exit(-1);
}

instruction get_instruction()
{
    //cout << hex << registers[PC] << dec << endl;
    instruction instr;
    unsigned char byte = find_target_byte(registers[PC]);
    //cout << hex << (int)byte << dec << endl;
    instr.opcode_mode = byte;
    registers[PC]++;

    //cout << hex << registers[PC] << dec << endl;
    byte = find_target_byte(registers[PC]);
    //cout << hex << (int)byte << dec << endl;
    instr.A = (byte & (unsigned char)0xF0) >> 4;
    instr.B = byte & 0x0F;
    registers[PC]++;

    //cout << hex << registers[PC] << dec << endl;
    byte = find_target_byte(registers[PC]);
    //cout << hex << (int)byte << dec << endl;
    instr.C = (byte & (unsigned char)0xF0) >> 4;
    instr.D = byte & 0x0F;
    instr.D = instr.D << 8;
    registers[PC]++;

    //cout << hex << registers[PC] << dec << endl;
    byte = find_target_byte(registers[PC]);
    //cout << hex << (int)byte << dec << endl;
    instr.D = instr.D | byte;
    //cout << instr.D << dec << endl;
    registers[PC]++;

    return instr;
}

int twelve_bits_2nd_complement(unsigned int value){
      if (value & 0x800) {
        // Negative number: Apply 2's complement
        return static_cast<int>(value | 0xFFFFF000);
    } else {
        // Positive number: Return as is
        return static_cast<int>(value);
    }
}

unsigned int get_content_from_address(unsigned int address){
  unsigned int return_value = 0;
  char byte1 = find_target_byte(address);
  char byte2 = find_target_byte(address + 1);
  char byte3 = find_target_byte(address + 2);
  char byte4 = find_target_byte(address + 3);

  return_value |= static_cast<unsigned char>(byte4) << 24;
  return_value |= static_cast<unsigned char>(byte3) << 16;
  return_value |= static_cast<unsigned char>(byte2) << 8;
  return_value |= static_cast<unsigned char>(byte1);

  return return_value;
}

void pushback_addresses(unsigned int address, unsigned int value){
  emulator_memory_record record;

  record.adress = address;
  record.value = GET_BYTE_1(value);
  memory.push_back(record);

  record.adress = address + 1;
  record.value = GET_BYTE_2(value);
  memory.push_back(record);

  record.adress = address + 2;
  record.value = GET_BYTE_3(value);
  memory.push_back(record);

  record.adress = address + 3;
  record.value = GET_BYTE_4(value);
  memory.push_back(record);
}

void add_address_to_context(unsigned int address, unsigned int value){
  int i = 0;
  for (; i < memory.size(); i++){
    if(memory[i].adress == address) break;
  }
  if (i == memory.size()) pushback_addresses(address, value);
  
  memory[i].value = GET_BYTE_1(value);
  memory[i + 1].value = GET_BYTE_2(value);
  memory[i + 2].value = GET_BYTE_3(value);
  memory[i + 3].value = GET_BYTE_4(value);  
  

}

void print_processor_context(){
  cout << "-----------------------------------------------------------------" << endl;
  cout << "Emulated processor executed halt instruction" << endl;
  cout << "Emulated processor state:" << endl;

  for (int i = 0; i < registers.size(); i++) {
    cout << "r" << dec <<  i << "=0x" << setw(8) << setfill('0') << hex << registers[i] << "\t";
    if ((i + 1) % 4 == 0) cout << endl;
  }

}

void halt(bool& processing){
  cout<< "HALT" << endl; 
  cout << endl << endl << endl << endl << endl << endl << endl << endl << endl << endl << endl << endl << endl << endl; 
  print_processor_context();
  processing = false;
}
void int_(){
  cout << "INT" << endl; 
  //push status; push pc; cause<=4; status<=status&(~0x1); pc<=handle;
  
  //push status
  instruction instr;
  instr.opcode_mode = STORE | STORE_M2;
  instr.A = SP;
  instr.B = 0;
  instr.C = STATUS;
  instr.D = -4;
  store_m2(instr);
  
  

  //push pc
  instr.opcode_mode = STORE | STORE_M2;
  instr.A = SP;
  instr.B = 0;
  instr.C = PC;
  instr.D = -4;
  store_m2(instr);
  cout << "PUSHOVAN PC NA STEK  " << hex << registers[PC] << endl; 
  // cause <= 4;
  csr_registers[CAUSE] = 4;

  //status<=status&(~0x1);
  csr_registers[STATUS] = csr_registers[STATUS] & (~0x1);

  //pc<=handle;

  registers[PC] = csr_registers[HANDLER];


}
void call_mod0(instruction instr){
  cout << "CALLMOD0" << endl; 
  // push pc; pc<=gpr[A]+gpr[B]+D;
  //push pc
  instruction pc_push;
  pc_push.opcode_mode = STORE | STORE_M2;
  pc_push.A = SP;
  pc_push.B = 0;
  pc_push.C = PC;
  pc_push.D = -4;
  store_m2(pc_push);
  //pc<=gpr[A]+gpr[B]+D;
  registers[PC] = registers[instr.A] + registers[instr.B] + twelve_bits_2nd_complement(instr.D);
}
void call_mod1(instruction instr){
   cout << "CALLMOD1" << endl; 
  //push pc; pc<=mem32[gpr[A]+gpr[B]+D];
  //push pc
  instruction pc_push;
  pc_push.opcode_mode = STORE | STORE_M2;
  pc_push.A = SP;
  pc_push.B = 0;
  pc_push.C = PC;
  pc_push.D = -4;
  store_m2(pc_push);

  //pc<=mem32[gpr[A]+gpr[B]+D];
  unsigned int address = registers[instr.A] + registers[instr.B] + twelve_bits_2nd_complement(instr.D);
  unsigned int value = get_content_from_address(address);
  registers[PC] = value;
  cout << "U CALL MOD1 PC VALUE: " << registers[PC] << endl;
}

void jmp_m0(instruction instr){
   cout << "JMPMOD0 (vrv se preskace bazen literala)" << endl; 
  //pc<=gpr[A]+D;
  registers[PC] = registers[instr.A] + twelve_bits_2nd_complement(instr.D);
}
void jmp_m1(instruction instr){
   cout << "JMPMOD1" << endl; 
  //if (gpr[B] == gpr[C]) pc<=gpr[A]+D;
  if (registers[instr.B] == registers[instr.C]) registers[PC] = registers[instr.A] + twelve_bits_2nd_complement(instr.D);

}
void jmp_m2(instruction instr){
   cout << "JMPMOD2" << endl; 
  //if (gpr[B] != gpr[C]) pc<=gpr[A]+D;
  if (registers[instr.B] != registers[instr.C]) registers[PC] = registers[instr.A] + twelve_bits_2nd_complement(instr.D);

}
void jmp_m3(instruction instr){
   cout << "JMPMOD3" << endl; 
  //if (gpr[B] signed> gpr[C]) pc<=gpr[A]+D;
  if ((int)registers[instr.B] > (int)registers[instr.C]) registers[PC] = registers[PC] = registers[instr.A] + twelve_bits_2nd_complement(instr.D);
}
void jmp_m4(instruction instr){
   cout << "JMPMOD4" << endl; 
  //pc<=mem32[gpr[A]+D];
  unsigned int address = registers[instr.A] + twelve_bits_2nd_complement(instr.D);
  unsigned int value = get_content_from_address(address);

  registers[PC] = value;
}
void jmp_m5(instruction instr){
   cout << "JMPMOD5 (BEQ)" << endl; 
  //if (gpr[B] == gpr[C]) pc<=mem32[gpr[A]+D];
  if (registers[instr.B] == registers[instr.C]){
    unsigned int address = registers[instr.A] + twelve_bits_2nd_complement(instr.D);
    unsigned int value = get_content_from_address(address);

    cout << "PC: " << hex << registers[PC] << endl;
    cout << "Value koji treba da se smesti u pc: " << hex << value << endl;
    registers[PC] = value;
  }
}
void jmp_m6(instruction instr){
   cout << "JMPMOD6" << endl; 
// if (gpr[B] != gpr[C]) pc<=mem32[gpr[A]+D];
  if (registers[instr.B] != registers[instr.C]){
    unsigned int address = registers[instr.A] + twelve_bits_2nd_complement(instr.D);
    unsigned int value = get_content_from_address(address);

    registers[PC] = value;
  }

}
void jmp_m7(instruction instr)
{
    cout << "JMPMOD7" << endl;
    //if (gpr[B] signed> gpr[C]) pc<=mem32[gpr[A]+D];
    if ((int)registers[instr.B] > (int)registers[instr.C]) {
        unsigned int address = registers[instr.A] + twelve_bits_2nd_complement(instr.D);
        unsigned int value = get_content_from_address(address);

        registers[PC] = value;
    }
}

void xchg(instruction instr){
  cout << "XCHG" << endl; 
  //temp<=gpr[B]; gpr[B]<=gpr[C]; gpr[C]<=temp;
  unsigned int temp = registers[instr.B];
  registers[instr.B] = registers[instr.C];
  registers[instr.C] = temp;
} 
void add(instruction instr) {
  cout << "ADD_MODE" << endl; 
  //gpr[A]<=gpr[B] + gpr[C];
  registers[instr.A] = registers[instr.B] + registers[instr.C];
}
void sub(instruction instr) {
  cout << "SUB_MODE" << endl; 
  //gpr[A]<=gpr[B] - gpr[C];
  registers[instr.A] = registers[instr.B] - registers[instr.C];
}
void mul(instruction instr) {
  cout << "MUL_MODE" << endl; 
//gpr[A]<=gpr[B] * gpr[C];
  registers[instr.A] = registers[instr.B] * registers[instr.C];
}
void div(instruction instr) {
  cout << "DIV_MODE" << endl; 
//gpr[A]<=gpr[B] / gpr[C];
  registers[instr.A] = registers[instr.B] / registers[instr.C];
}
void not_(instruction instr){
  cout << "NOT_MODE" << endl; 
  //gpr[A]<=~gpr[B];
  registers[instr.A] = ~ registers[instr.B];
} 
void and_(instruction instr){
  cout << "AND_MODE" << endl; 
 //gpr[A]<=gpr[B] & gpr[C];
  registers[instr.A] = registers[instr.B] & registers[instr.C];
} 
void or_(instruction instr) {
  cout << "OR_MODE" << endl; 
  //gpr[A]<=gpr[B] | gpr[C]
  registers[instr.A] = registers[instr.B] | registers[instr.C];
}
void xor_(instruction instr){
  cout << "XOR_MODE" << endl; 
 // gpr[A]<=gpr[B] ^ gpr[C];
 registers[instr.A] = registers[instr.B] ^ registers[instr.C];
} 
void shl(instruction instr) {
  cout << "SHL_MODE" << endl; 
  //gpr[A]<=gpr[B] << gpr[C];
  registers[instr.A] = registers[instr.B] << registers[instr.C];
}
void shr(instruction instr) {
  cout << "SHR_MODE" << endl; 
  //gpr[A]<=gpr[B] >> gpr[C];
  registers[instr.A] = registers[instr.B] >> registers[instr.C];

}

void store_m0(instruction instr){
  cout << "STORE_M0" << endl; 
  //mem32[gpr[A]+gpr[B]+D]<=gpr[C];
  unsigned int address = registers[instr.A] + registers[instr.B] + twelve_bits_2nd_complement(instr.D);
  add_address_to_context(address, registers[instr.C]);

}
void store_m1(instruction instr){
  cout << "STORE_M1" << endl; 
  //mem32[mem32[gpr[A]+gpr[B]+D]]<=gpr[C];

  //[gpr[A]+gpr[B]+D]
  unsigned int address = registers[instr.A] + registers[instr.B] + twelve_bits_2nd_complement(instr.D);
  //[mem32[gpr[A]+gpr[B]+D]]
  unsigned int real_address = get_content_from_address(address);

  add_address_to_context(real_address, registers[instr.C]);

}
void store_m2(instruction instr){
  cout << "STORE_M2" << endl; 
  cout << "Registar za koji se radi push: " << hex << (unsigned)instr.C << endl;
  //gpr[A]<=gpr[A]+D; mem32[gpr[A]]<=gpr[C];
  //gpr[A]<=gpr[A]+D;
  //cout << "Vrednost D kao int" <<twelve_bits_2nd_complement(instr.D) << endl;
  registers[instr.A] += twelve_bits_2nd_complement(instr.D);

  //mem32[gpr[A]]<=gpr[C];
  unsigned int address = registers[instr.A];
  add_address_to_context(address, registers[instr.C]);
}

void load_m0(instruction instr){
  cout << "LOAD_M0" << endl; 
  //gpr[A]<=csr[B];
  registers[instr.A] = csr_registers[instr.B];
}
void load_m1(instruction instr){
  cout << "LOAD_M1" << endl; 
  // gpr[A]<=gpr[B]+D;
    registers[instr.A] = registers[instr.B] + twelve_bits_2nd_complement(instr.D);
}
void load_m2(instruction instr){
  cout << "LOAD_M2" << endl; 
  //gpr[A]<=mem32[gpr[B]+gpr[C]+D];
  
  unsigned int address = registers[instr.B] + registers[instr.C] + twelve_bits_2nd_complement(instr.D);
  unsigned int value = get_content_from_address(address);

  registers[instr.A] = value; 

}
void load_m3(instruction instr){
  cout << "LOAD_M3" << endl; 
  //gpr[A]<=mem32[gpr[B]]; gpr[B]<=gpr[B]+D;

  unsigned int address = registers[instr.B];
  unsigned int value = get_content_from_address(address);
  cout << "LOAD M3 POP PC SA ADRESE: " << hex << address << endl;
  cout << "LOAD M3 POP PC: " << hex << value << endl;
  registers[instr.A] = value; 

  registers[instr.B] += twelve_bits_2nd_complement(instr.D);

}
void load_m4(instruction instr){
  cout << "LOAD_M4" << endl; 
  //csr[A]<=gpr[B];

  csr_registers[instr.A] = registers[instr.B];

}
void load_m5(instruction instr){
  cout << "LOAD_M5" << endl; 
  //csr[A]<=csr[B]|D;

  csr_registers[instr.A] = csr_registers[instr.B] | twelve_bits_2nd_complement(instr.D);

}
void load_m6(instruction instr){
  cout << "LOAD_M6" << endl; 
  //csr[A]<=mem32[gpr[B]+gpr[C]+D];

  unsigned int address = registers[instr.B] + registers[instr.C] + twelve_bits_2nd_complement(instr.D);
  unsigned int value = get_content_from_address(address);

  csr_registers[instr.A] = value;
}
void load_m7(instruction instr){
  cout << "LOAD_M7" << endl; 
  //csr[A]<=mem32[gpr[B]]; gpr[B]<=gpr[B]+D;

  unsigned int address = registers[instr.B];
  unsigned int value = get_content_from_address(address);
  csr_registers[instr.A] = value;

  registers[instr.B] += twelve_bits_2nd_complement(instr.D);

}