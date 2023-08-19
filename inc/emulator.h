#ifndef EMUlATOR_H
#define EMUlATOR_H
#include "../inc/structures.h"

#include <fstream>
#include <iostream>

void load_memory_content(ifstream&);
void init_registers();
unsigned char find_target_byte(unsigned int);
instruction get_instruction();
int twelve_bits_2nd_complement(unsigned int);
unsigned int get_content_from_address(unsigned int);
void add_address_to_context(unsigned int, unsigned int);
void pushback_addresses(unsigned int, unsigned int);
void print_processor_context();

extern vector<unsigned int> registers;
extern vector<unsigned int> csr_registers;
extern vector <emulator_memory_record> memory;

extern unsigned int status;
extern unsigned int handler;
extern unsigned int cause;



void halt(bool&);
void int_();
void call_mod0(instruction);
void call_mod1(instruction);
void jmp_m0(instruction);
void jmp_m1(instruction);
void jmp_m2(instruction);
void jmp_m3(instruction);
void jmp_m4(instruction);
void jmp_m5(instruction);
void jmp_m6(instruction);
void jmp_m7(instruction);
void xchg(instruction);
void add(instruction);
void sub(instruction);
void mul(instruction);
void div(instruction);
void not_(instruction);
void and_(instruction);
void or_(instruction);
void xor_(instruction);
void shl(instruction);
void shr(instruction);
void store_m0(instruction);
void store_m1(instruction);
void store_m2(instruction);
void load_m0(instruction);
void load_m1(instruction);
void load_m2(instruction);
void load_m3(instruction);
void load_m4(instruction);
void load_m5(instruction);
void load_m6(instruction);
void load_m7(instruction);

#endif