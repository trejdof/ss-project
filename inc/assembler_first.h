#ifndef ASSEMBLER_FIRST_H
#define ASSEMBLER_FIRST_H
#include "assembler.h"
#include <iostream>
#include <string>
class AssemblerFirst : public Assembler {
public:
    AssemblerFirst() { }

    ~AssemblerFirst() { }

    void process_global_dir(const string& name) override;
    void process_extern_dir(const string& name) override;
    void process_section_dir(const string& name) override;
    void process_word_dir(const string& name) override;
    void process_word_dir(const int literal) override;
    void process_skip_dir(const int literal) override;
    void process_end_dir() override;

    void process_label(const string& name) override;

    void process_instruction(const int count) override;

    void process_IRET() override { }
    void process_HALT() override { }
    void process_INT() override { }
    void process_RET() override { }

    void process_CALL(const string& name) override { }
    void process_CALL(const int literal) override { }

    void process_JMP(const string& name) override { }
    void process_JMP(const int literal) override { }

    void process_BEQ(const int, const int, const int) { }
    void process_BEQ(const int, const int, const string&) { }

    void process_BNE(const int, const int, const int) { }
    void process_BNE(const int, const int, const string&) { }

    void process_BGT(const int, const int, const int) { }
    void process_BGT(const int, const int, const string&) { }

    void process_PUSH(const int) {}
    void process_POP(const int) {}
    void process_XCHG(const int, const int) {}
    void process_ADD(const int, const int) {}
    void process_SUB(const int, const int) {}
    void process_MUL(const int, const int) {}
    void process_DIV(const int, const int) {}
    void process_NOT(const int){}
    void process_AND(const int, const int) {}
    void process_OR(const int, const int) {}
    void process_XOR(const int, const int) {}
    void process_SHL(const int, const int) {}
    void process_SHR(const int, const int) {}

    void process_CSRRD(const int, const int){}
    void process_CSRWR(const int, const int){}

    void process_LD_IMM(const int, const int) {}
    void process_LD_IMM(const string&, const int) {}
    void process_LD_REG_DIR(const int, const int) {}
    void process_LD_REG_IND(const int, const int) {}
    void process_LD_REG_IND_OFF(const int, const int, const int) {}
    void process_LD_MEM_DIR(const int, const int) {}
    void process_LD_MEM_DIR(const string&, const int) {}

    void process_ST_MEM_DIR(const int, const int) {}
    void process_ST_MEM_DIR(const int, const string&) {}
    void process_ST_REG_IND(const int, const int) {}
    void process_ST_REG_IND_OFF(const int, const int, const int) {}
    void process_ST_REG_IND_OFF(const int, const int, const string&) {}
private:
    int location_counter = 0;
    int curr_section = 0;

    int get_symbol_index(const string& name);
    int add_to_symbol_table(const string& name);
    static void print();
};

#endif