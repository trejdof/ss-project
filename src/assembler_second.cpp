
#include "../inc/assembler_second.h"
#include "../inc/assembler.h"
#include "../inc/constants.h"
#include "../inc/structures.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>
#include <bitset>





using namespace std;

int AssemblerSecond::get_symbol_index(const string& name)
{  //MOZDA MENJATI
    for (int i = 0; i < elf_symbol_table.size(); i++) {
        if (elf_symbol_table[i].name == name) {
            return i;
        }
    }
    return -1;
}

int AssemblerSecond::get_section_id(const string& name)
{
    for (int i = 0; i < section_table.size(); i++) {
        if (section_table[i].name == name) {
            return i;
        }
    }
    return -1;
}

void AssemblerSecond::makeRelocation(const int offset, const string& name)
{
    int symbol_id = get_symbol_index(name);
    if (symbol_id == -1) {
        cout << "NO SYMBOL IN TABLE" << endl;
        exit(-1);
    }
    relocation_record reloc(offset, symbol_id);
    section_table[curr_section].section_relocs.push_back(reloc);
}

void AssemblerSecond::write_memory(char b1, char b2, char b3, char b4)
{   
    section_table[curr_section].memory_content.push_back(b1);
    section_table[curr_section].memory_content.push_back(b2);
    section_table[curr_section].memory_content.push_back(b3);
    section_table[curr_section].memory_content.push_back(b4);
    location_counter += INSTR_SIZE;
}

bool AssemblerSecond::check_if_pool_is_needed(const int literal)
{
    if (literal > LITERAL_MAX) return true;
    if (literal < LITERAL_MIN) return true;

    return false;
}

void AssemblerSecond::process_POOL_SYMBOL(vector<char> machine_code, const string& name)
{
    char op_code = machine_code[0];
    char mode = machine_code[1];
    char A = machine_code[2];
    char B = machine_code[3];
    char C = machine_code[4];

    // write into pool, memory looks like this: adr:       INSTRUCTION_REQ_POOL
    //                                          adr+4:     JMP
    //                                          adr+8:     literal
    //                                          adr+12:    NEXT_INSTRUCTION <---- jumps here
    updateSymbolTable();
    updateSectionTable();
    write_memory(op_code | mode, (A << 4) | B, (C << 4) | GET_HIGH_NIBBLE(PLUS_4), GET_LOW_BYTE(PLUS_4)); // real+instruction

    process_JMP(4);
    makeRelocation(location_counter, name);
    write_memory(0, 0, 0, 0); // symbol placeholder
}

void AssemblerSecond::process_POOL_LITERAL(vector<char> no_pool, vector<char> pool, int literal)
{

    if (check_if_pool_is_needed(literal)) {
        // literal pool is needed
        char op_code = pool[0];
        char mode = pool[1];
        char A = pool[2];
        char B = pool[3];
        char C = pool[4];
        updateSymbolTable();
        updateSectionTable();
        write_memory(op_code | mode, (A << 4) | B, (C << 4) | GET_HIGH_NIBBLE(PLUS_4), GET_LOW_BYTE(PLUS_4)); // real+instruction
        process_JMP(4);
        write_memory(GET_BYTE_1(literal), GET_BYTE_2(literal), GET_BYTE_3(literal), GET_BYTE_4(literal));
    } else {
        // literal can be fit into 12 bits
        char op_code = no_pool[0];
        char mode = no_pool[1];
        char A = no_pool[2];
        char B = no_pool[3];
        char C = no_pool[4];
        unsigned literal_on_12bits = literal & 0xFFF;
        write_memory(op_code | mode, (A << 4) | B, (C << 4) | GET_HIGH_NIBBLE(literal_on_12bits), GET_LOW_BYTE(literal_on_12bits)); // real+instruction
    }
}

void AssemblerSecond::insert_jump_for_pool()
{
    //MMMM==0b0000: pc<=gpr[A]+D;
    // JUMP MODE0   PC 0000   0000 DDDD   DDDD DDDD
    // jump is always +4
    write_memory(JUMP | JMP_M0, PC << 4 , GET_HIGH_NIBBLE(PLUS_4), GET_LOW_BYTE(PLUS_4));
}

void AssemblerSecond::updateSymbolTable(){
    for (int i = 0; i < elf_symbol_table.size(); i++) {
        if (elf_symbol_table[i].offset > location_counter &&
            elf_symbol_table[i].section_id == curr_section) {
            elf_symbol_table[i].offset += 8;
        }
    }
}
void AssemblerSecond::updateSectionTable(){
    section_table[curr_section].size += 8;
}

void AssemblerSecond::process_section_dir(const string& name)
{
    location_counter = 0;
    curr_section = get_section_id(name);
}

void AssemblerSecond::process_skip_dir(const int literal) {
    for (int i = 0; i < literal; i++){
        section_table[curr_section].memory_content.push_back(0);
        location_counter++;
    }
}
void AssemblerSecond::process_word_dir(const string &name) {

  makeRelocation(location_counter, name);

  write_memory(0, 0, 0, 0);
}
void AssemblerSecond::process_word_dir(const int literal)
{

    write_memory(GET_BYTE_1(literal), GET_BYTE_2(literal), GET_BYTE_3(literal), GET_BYTE_4(literal));

    
}

void AssemblerSecond::process_end_dir()
{
    //print_elf_file();

    cout << "Assembling finished!" << endl;
}

void AssemblerSecond::process_IRET()
{
    // gprA = gprB + D, gprA = gprB = SP D = 8
    write_memory(LOAD | LOAD_M1, (SP << 4) | SP, GET_HIGH_NIBBLE(VALUE_8) & 0xF, GET_LOW_BYTE(VALUE_8));
    // csrA = mem[gprB + gprC + D], csrA = status, gprB = SP, D = -4
    write_memory(LOAD | LOAD_M6, (STATUS << 4) | SP, GET_HIGH_NIBBLE(MINUS_4) & 0xF, GET_LOW_BYTE(MINUS_4));
    // gprA = mem[gprB + gprC + D], gprA = PC, gprB = SP, D = -8
    write_memory(LOAD | LOAD_M2, (PC << 4) | SP, GET_HIGH_NIBBLE(MINUS_8) & 0xF, GET_LOW_BYTE(MINUS_8));
}

void AssemblerSecond::process_HALT()
{
    write_memory(HALT, 0, 0, 0);
    
}

void AssemblerSecond::process_INT()
{
    write_memory(INT, 0, 0, 0);
    
}

void AssemblerSecond::process_RET()
{
    process_POP(PC);
}

void AssemblerSecond::process_CALL(const string& name)
{
    vector<char> machine_code = { CALL, CALL_MOD1, PC, 0, 0 };
    process_POOL_SYMBOL(machine_code, name);
    
}
void AssemblerSecond::process_CALL(const int literal)
{
    vector<char> no_pool = { CALL, CALL_MOD0, 0, 0, 0 };
    vector<char> pool = { CALL, CALL_MOD1, PC, 0, 0 };
    process_POOL_LITERAL(no_pool, pool, literal);

}

void AssemblerSecond::process_JMP(const string& name)
{
    vector<char> machine_code = { JUMP, JMP_M4, PC, 0, 0 };
    process_POOL_SYMBOL(machine_code, name);
}
void AssemblerSecond::process_JMP(const int literal)
{
    vector<char> no_pool = { JUMP, JMP_M0, PC, 0, 0 };
    vector<char> pool = { JUMP, JMP_M4, PC, 0, 0 };
    process_POOL_LITERAL(no_pool, pool, literal);

}

void AssemblerSecond::process_BEQ(const int gpr1, const int gpr2, const string& name)
{
    vector<char> machine_code = { JUMP, JMP_M5, PC, (char)gpr1, (char)gpr2 };
    process_POOL_SYMBOL(machine_code, name);
}
void AssemblerSecond::process_BEQ(const int gpr1, const int gpr2, const int literal)
{
    vector<char> no_pool = { JUMP, JMP_M1, 0, (char)gpr1, (char)gpr2 };
    vector<char> pool = { JUMP, JMP_M5, PC, (char)gpr1, (char)gpr2 };
    process_POOL_LITERAL(no_pool, pool, literal);

}

void AssemblerSecond::process_BNE(const int gpr1, const int gpr2, const string& name)
{
    vector<char> machine_code = { JUMP, JMP_M6, PC, (char)gpr1, (char)gpr2 };
    process_POOL_SYMBOL(machine_code, name);
}
void AssemblerSecond::process_BNE(const int gpr1, const int gpr2, const int literal)
{
    vector<char> no_pool = { JUMP, JMP_M2, 0, (char)gpr1, (char)gpr2 };
    vector<char> pool = { JUMP, JMP_M6, PC, (char)gpr1, (char)gpr2 };
    process_POOL_LITERAL(no_pool, pool, literal);

}

void AssemblerSecond::process_BGT(const int gpr1, const int gpr2, const string& name)
{
    vector<char> machine_code = { JUMP, JMP_M7, PC, (char)gpr1, (char)gpr2 };
    process_POOL_SYMBOL(machine_code, name);
}
void AssemblerSecond::process_BGT(const int gpr1, const int gpr2, const int literal)
{
    vector<char> no_pool = { JUMP, JMP_M3, 0, (char)gpr1, (char)gpr2 };
    vector<char> pool = { JUMP, JMP_M7, PC, (char)gpr1, (char)gpr2 };
    process_POOL_LITERAL(no_pool, pool, literal);

}

void AssemblerSecond::process_PUSH(const int gpr)
{
    //sp <= sp - 4; mem32[sp] <= gpr;
    //1000 0001 1110 0000 gpr 0000 0000 0000
    write_memory(STORE | STORE_M2, SP << 4, gpr << 4 | GET_HIGH_NIBBLE(MINUS_4), GET_LOW_BYTE(MINUS_4));
}

void AssemblerSecond::process_POP(const int gpr)
{
    //gpr <= mem32[sp]; sp <= sp + 4;
    //  1001 0011 gpr sp 0000 DDDD DDDD DDDD
    write_memory(LOAD | LOAD_M3, (gpr << 4) | SP, (0 << 4) | GET_HIGH_NIBBLE(PLUS_4), GET_LOW_BYTE(PLUS_4));
}

void AssemblerSecond::process_XCHG(const int gpr1, const int gpr2)
{
    //temp <= gprD; gprD <= gprS; gprS <= temp;
    // 0100 0000   0000 gpr1   gpr2 0000   0000 0000
    write_memory(XCHG, gpr1, gpr2 << 4, 0);
}

void AssemblerSecond::process_ADD(const int gprS, const int gprD)
{
    // gprD <= gprD + gprS;
    //  0101 0000   gprD gprD   gprS 0000   0000
    write_memory(ARIT | ADD_MODE, (gprD << 4) | gprD, gprS << 4, 0);
    
}

void AssemblerSecond::process_SUB(const int gprS, const int gprD)
{
    // sub %gprS, %gprD gprD <= gprD - gprS;
    // 0101 0001    gprD gprD   gprS 0000   0000 0000
    write_memory(ARIT | SUB_MODE, (gprD << 4) | gprD, gprS << 4, 0);
}

void AssemblerSecond::process_MUL(const int gprS, const int gprD)
{
    // mul %gprS, %gprD gprD <= gprD * gprS;
    //
    write_memory(ARIT | MUL_MODE, (gprD << 4) | gprD, gprS << 4, 0);
}

void AssemblerSecond::process_DIV(const int gprS, const int gprD)
{
    //
    //
    write_memory(ARIT | DIV_MODE, (gprD << 4) | gprD, gprS << 4, 0);
}

void AssemblerSecond::process_NOT(const int gpr)
{
    //
    //
    write_memory(LOGIC | NOT_MODE, (gpr << 4) | gpr, 0, 0);
}

void AssemblerSecond::process_AND(const int gprS, const int gprD)
{
    //
    //
    write_memory(LOGIC | AND_MODE, (gprD << 4) | gprD, gprS << 4, 0);
}

void AssemblerSecond::process_OR(const int gprS, const int gprD)
{
    //
    //
    write_memory(LOGIC | OR_MODE, (gprD << 4) | gprD, gprS << 4, 0);
}

void AssemblerSecond::process_XOR(const int gprS, const int gprD)
{
    //
    //
    write_memory(LOGIC | XOR_MODE, (gprD << 4) | gprD, gprS << 4, 0);
}

void AssemblerSecond::process_SHL(const int gprS, const int gprD)
{
    //
    //
    write_memory(SHIFT | SHL_MODE, (gprD << 4) | gprD, gprS << 4, 0);
}

void AssemblerSecond::process_SHR(const int gprS, const int gprD)
{
    //
    //
    write_memory(SHIFT | SHR_MODE, (gprD << 4) | gprD, gprS << 4, 0);
}

void AssemblerSecond::process_CSRRD(const int csrS, const int gprD)
{
    //
    //
    write_memory(LOAD | LOAD_M0, (gprD << 4) | csrS, 0, 0);
}

void AssemblerSecond::process_CSRWR(const int gprS, const int csrD)
{
    //
    //
    write_memory(LOAD | LOAD_M4, (csrD << 4) | gprS, 0, 0);
}

void AssemblerSecond::process_LD_IMM(const int literal, const int gprD)
{   
    vector<char> no_pool = { (char)LOAD, LOAD_M1, (char)gprD, 0,  0 };
    vector<char> pool = { (char)LOAD, LOAD_M2, (char)gprD, 0, PC };
    process_POOL_LITERAL(no_pool, pool, literal);

}

void AssemblerSecond::process_LD_IMM(const string& name, const int gprD)
{   
    
    vector<char> machine_code = { char(LOAD), LOAD_M2, (char)gprD, 0, PC };
    process_POOL_SYMBOL(machine_code, name);
}

void AssemblerSecond::process_LD_REG_DIR(const int gprS, const int gprD)
{
    //gpr[A]<=gpr[B]+D;
    //  LOADD MODE1   gprD gprS   0000 0000  0000 0000
    
    write_memory(LOAD | LOAD_M1, (gprD << 4) | gprS, 0, 0);
}

void AssemblerSecond::process_LD_REG_IND(const int gprS, const int gprD)
{
    //  gpr[A]<=mem32[gpr[B]+gpr[C]+D];
    //  LOAD MODE2    gprD gprS   0000 0000   0000 0000
    write_memory(LOAD | LOAD_M2, (gprD << 4) | gprS, 0, 0);
}

void AssemblerSecond::process_LD_REG_IND_OFF(const int gprS, const int literal, const int gprD)
{
    bool needPool = check_if_pool_is_needed(literal);
    if (needPool) {
        cout << "literal is too big and cannot fit into 12bits reserved for it" << endl;
        exit(-1);
    } else {
        unsigned literal_on_12bits = literal & 0xFFF;
        write_memory(LOAD | LOAD_M2, (gprD << 4) | gprS, GET_HIGH_NIBBLE(literal_on_12bits), GET_LOW_BYTE(literal_on_12bits));
    }
}

void AssemblerSecond::process_LD_MEM_DIR(const int literal, const int gprD)
{
    if (check_if_pool_is_needed(literal)) {
        // pool is needed
        // gotta split into 2 instructions
        vector<char> no_pool = { (char)LOAD, LOAD_M0, 0, PC, 0 };
        vector<char> pool = { (char)LOAD, LOAD_M2, char(gprD), PC, 0 };
        process_POOL_LITERAL(no_pool, pool, literal);

        process_LD_REG_IND(gprD, gprD);

    } else {
        // literal can fit into 12 bits
        unsigned literal_on_12bits = literal & 0xFFF;
        write_memory(LOAD | LOAD_M2, (gprD << 4), GET_HIGH_NIBBLE(literal_on_12bits), GET_LOW_BYTE(literal_on_12bits)); // real+instruction
    }
}

void AssemblerSecond::process_LD_MEM_DIR(const string& name, const int gprD)
{
    vector<char> machine_code = { (char)LOAD, LOAD_M2, char(gprD), PC, 0 };
    process_POOL_SYMBOL(machine_code, name);
    
    process_LD_REG_IND(gprD, gprD);
    
}

void AssemblerSecond::process_ST_MEM_DIR(const int gprS, const int literal)
{
    vector<char> no_pool = { (char)STORE, STORE_M0, 0, 0, (char)gprS };
    vector<char> pool = { (char)STORE, STORE_M1, PC, 0, (char)gprS };
    process_POOL_LITERAL(no_pool, pool, literal);

}

void AssemblerSecond::process_ST_MEM_DIR(const int gprS, const string& name)
{   
    vector<char> machine_code = { (char)STORE, STORE_M1, PC, 0, (char)gprS };
    process_POOL_SYMBOL(machine_code, name);
}

void AssemblerSecond::process_ST_REG_IND(const int gprS, const int gprD)
{
    write_memory(STORE | STORE_M0, gprD, gprS << 4, 0);

}

void AssemblerSecond::process_ST_REG_IND_OFF(const int gprS, const int gprD, const int literal)
{
    if(check_if_pool_is_needed(literal)){
        cout << "Literal cant fit into 12 bits reserved for it" << endl;
        exit(-1);
    }
    else{
        unsigned literal_on_12bits = literal & 0xFFF;
        write_memory(STORE | STORE_M0, gprD, (gprS << 4) | GET_HIGH_NIBBLE(literal_on_12bits), GET_LOW_BYTE(literal_on_12bits));
    }

}

void AssemblerSecond::process_ST_REG_IND_OFF(const int gprS, const int gprD, const string& name)
{
}