#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "../inc/structures.h"

#include <vector>
#include <string>

using namespace std;


class Assembler {
public:
    Assembler() {}
    ~Assembler() {}

    //directives
    virtual void process_global_dir(const string& name) {}
    virtual void process_extern_dir(const string& name) {}
    virtual void process_section_dir(const string& name) {}
    virtual void process_word_dir(const string& name) {}
    virtual void process_word_dir(const int literal) {}
    virtual void process_skip_dir(const int literal) {}
    virtual void process_end_dir() {}
    
    virtual void process_label(const string& name);

    virtual void process_instruction(const int count);

    virtual void process_IRET() {}
    virtual void process_HALT() {}
    virtual void process_INT() {}
    virtual void process_RET() {}

    virtual void process_CALL(const string& name) {}
    virtual void process_CALL(const int literal) {}

    virtual void process_JMP(const string& name) {}
    virtual void process_JMP(const int literal) {}

    virtual void process_BEQ(const int, const int, const int){}
    virtual void process_BEQ(const int, const int, const string&){}

    virtual void process_BNE(const int, const int, const int){}
    virtual void process_BNE(const int, const int, const string&){}

    virtual void process_BGT(const int, const int, const int){}
    virtual void process_BGT(const int, const int, const string&){}

    virtual void process_PUSH(const int) {}
    virtual void process_POP(const int) {}

    virtual void process_XCHG(const int, const int) {}
    virtual void process_ADD(const int, const int) {}
    virtual void process_SUB(const int, const int) {}
    virtual void process_MUL(const int, const int) {}
    virtual void process_DIV(const int, const int) {}
    virtual void process_NOT(const int){}
    virtual void process_AND(const int, const int) {}
    virtual void process_OR(const int, const int) {}
    virtual void process_XOR(const int, const int) {}
    virtual void process_SHL(const int, const int) {}
    virtual void process_SHR(const int, const int) {}

    virtual void process_CSRRD(const int, const int){}
    virtual void process_CSRWR(const int, const int){}

    virtual void process_LD_IMM(const int, const int) {}
    virtual void process_LD_IMM(const string&, const int) {}
    virtual void process_LD_REG_DIR(const int, const int) {}
    virtual void process_LD_REG_IND(const int, const int) {}
    virtual void process_LD_REG_IND_OFF(const int, const int, const int) {}
    virtual void process_LD_MEM_DIR(const int, const int) {}
    virtual void process_LD_MEM_DIR(const string&, const int) {}

    virtual void process_ST_MEM_DIR(const int, const int) {}
    virtual void process_ST_MEM_DIR(const int, const string&) {}
    virtual void process_ST_REG_IND(const int, const int) {}
    virtual void process_ST_REG_IND_OFF(const int, const int, const int) {}
    virtual void process_ST_REG_IND_OFF(const int, const int, const string&) {}

    static void init_symbol_table();
    static void print_elf_file(ofstream& );

    static void merge_tables();
    
protected:
    
    static vector<symbol_record> symbol_table;
    static vector<section_record> section_table;

    static vector<symbol_record> elf_symbol_table;

private:

    
};

#endif  // ASSEMBLER_H