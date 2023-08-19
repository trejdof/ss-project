#ifndef ASSEMBLER_SECOND_H
#define ASSEMBLER_SECOND_H
#include "assembler.h"
#include <iostream>
#include <string>

class AssemblerSecond : public Assembler {
public:
    AssemblerSecond() { }

    ~AssemblerSecond() { }

    void process_global_dir(const string& name) override        { }
    void process_extern_dir(const string& name) override        { }
    void process_skip_dir(const int literal) override;
    void process_label(const string& name) override             { }
    void process_instruction(const int count) override          { }


    void process_word_dir(const string& name) override;
    void process_word_dir(const int literal) override;
    void process_section_dir(const string& name) override;
    void process_end_dir() override;

    void process_IRET() override;
    void process_HALT() override;
    void process_INT() override;
    void process_RET() override;

    void process_CALL(const string& name) override;
    void process_CALL(const int literal) override;

    void process_JMP(const string& name) override;
    void process_JMP(const int literal) override;

    void process_BEQ(const int, const int, const string&) override;
    void process_BEQ(const int, const int, const int) override;

    void process_BNE(const int, const int, const string&) override;
    void process_BNE(const int, const int, const int) override;

    void process_BGT(const int, const int, const string&) override;
    void process_BGT(const int, const int, const int) override;

    void process_PUSH(const int) override;
    void process_POP(const int) override;
    void process_XCHG(const int, const int) override;
    void process_ADD(const int, const int) override;
    void process_SUB(const int, const int) override;
    void process_MUL(const int, const int) override;
    void process_DIV(const int, const int) override;
    void process_NOT(const int) override;
    void process_AND(const int, const int) override;
    void process_OR(const int, const int) override;
    void process_XOR(const int, const int) override;
    void process_SHL(const int, const int) override;
    void process_SHR(const int, const int) override;

    void process_CSRRD(const int, const int) override;
    void process_CSRWR(const int, const int) override;

    void process_LD_IMM(const int, const int) override;
    void process_LD_IMM(const string&, const int) override;
    void process_LD_REG_DIR(const int, const int) override;
    void process_LD_REG_IND(const int, const int) override;
    void process_LD_REG_IND_OFF(const int, const int, const int) override;
    void process_LD_MEM_DIR(const int, const int) override;
    void process_LD_MEM_DIR(const string&, const int) override;

    void process_ST_MEM_DIR(const int, const int) override;
    void process_ST_MEM_DIR(const int, const string&) override;
    void process_ST_REG_IND(const int, const int) override;
    void process_ST_REG_IND_OFF(const int, const int, const int) override;
    void process_ST_REG_IND_OFF(const int, const int, const string&) override;

private:
    int location_counter = 0;
    int curr_section = 0;
    int get_section_id(const string& name);

    void makeRelocation(const int,const string&);
    int get_symbol_index(const string& name);
    void write_memory(char, char, char, char);
    bool check_if_pool_is_needed(const int);
    void process_POOL_SYMBOL(vector<char>, const string& name);
    void process_POOL_LITERAL(vector<char>, vector<char> , int);
    void insert_jump_for_pool();
    void updateSymbolTable();
    void updateSectionTable();
};

#endif