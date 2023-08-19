#ifndef CONSTANTS_H
#define CONSTANTS_H

#define INSTR_SIZE  4
#define MINUS_4 0b111111111100
#define PLUS_4 0b000000000100
#define VALUE_8 0b000000001000
#define MINUS_8 0xFFF8
const int LITERAL_MAX = (1 << 11) - 1;
const int LITERAL_MIN = -(1 << 11);

#define PC_START 0x40000000
#define MAX_ADDRESS 0xFFFFFFFF


#define GET_BYTE_1(value)   ((value) & 0xFF)
#define GET_BYTE_2(value)   (((value) >> 8) & 0xFF)
#define GET_BYTE_3(value)   (((value) >> 16) & 0xFF)
#define GET_BYTE_4(value)   (((value) >> 24) & 0xFF)

#define GET_HIGH_NIBBLE(value) ((value) >> 8)
#define GET_LOW_BYTE(value) ((value) & 0xFF)

#define HALT          0x00
#define INT           0x10
#define CALL          0x20
#define JUMP          0x30
#define XCHG          0x40
#define ARIT          0x50
#define LOGIC         0x60
#define SHIFT         0x70
#define STORE         0x80
#define LOAD          0x90


#define CALL_MOD0     0x00 // push pc; pc<=gpr[A]+gpr[B]+D;
#define CALL_MOD1     0x01 //push pc; pc<=mem32[gpr[A]+gpr[B]+D];

#define JMP_M0        0x00   //  pc<=gpr[A]+D;
#define JMP_M1        0x01   //  if (gpr[B] == gpr[C]) pc<=gpr[A]+D;
#define JMP_M2        0x02   //  if (gpr[B] != gpr[C]) pc<=gpr[A]+D;
#define JMP_M3        0x03   //  if (gpr[B] signed> gpr[C]) pc<=gpr[A]+D;
#define JMP_M4        0x08   //  pc<=mem32[gpr[A]+D];
#define JMP_M5        0x09   //  if (gpr[B] == gpr[C]) pc<=mem32[gpr[A]+D];
#define JMP_M6        0x0A   //  if (gpr[B] != gpr[C]) pc<=mem32[gpr[A]+D];
#define JMP_M7        0x0B   //  if (gpr[B] signed> gpr[C]) pc<=mem32[gpr[A]+D];

#define ADD_MODE      0x00 // gpr[A]<=gpr[B] + gpr[C];    
#define SUB_MODE      0x01 // gpr[A]<=gpr[B] - gpr[C];   
#define MUL_MODE      0x02 // gpr[A]<=gpr[B] * gpr[C];   
#define DIV_MODE      0x03 // gpr[A]<=gpr[B] / gpr[C];  

#define NOT_MODE      0x00  //  gpr[A]<=~gpr[B];  
#define AND_MODE      0x01  //  gpr[A]<=gpr[B] & gpr[C];
#define OR_MODE       0x02  //  gpr[A]<=gpr[B] | gpr[C];
#define XOR_MODE      0x03  //  gpr[A]<=gpr[B] ^ gpr[C];

#define SHL_MODE      0x00  // gpr[A]<=gpr[B] << gpr[C];
#define SHR_MODE      0x01  // gpr[A]<=gpr[B] >> gpr[C];

#define STORE_M0      0x00  //  mem32[gpr[A]+gpr[B]+D]<=gpr[C];  
#define STORE_M1      0x02  //  mem32[mem32[gpr[A]+gpr[B]+D]]<=gpr[C]; 
#define STORE_M2      0x01  //  gpr[A]<=gpr[A]+D; mem32[gpr[A]]<=gpr[C]; 

#define LOAD_M0       0x0 //  gpr[A]<=csr[B];
#define LOAD_M1       0x1 //  gpr[A]<=gpr[B]+D;
#define LOAD_M2       0x2 //  gpr[A]<=mem32[gpr[B]+gpr[C]+D];
#define LOAD_M3       0x3 //  gpr[A]<=mem32[gpr[B]]; gpr[B]<=gpr[B]+D;
#define LOAD_M4       0x4 //  csr[A]<=gpr[B];
#define LOAD_M5       0x5 //  csr[A]<=csr[B]|D;
#define LOAD_M6       0x6 //  csr[A]<=mem32[gpr[B]+gpr[C]+D];
#define LOAD_M7       0x7 //  csr[A]<=mem32[gpr[B]]; gpr[B]<=gpr[B]+D;

#define STATUS        0
#define HANDLER       1
#define CAUSE         2
#define ACC           13
#define SP            14
#define PC            15




#endif