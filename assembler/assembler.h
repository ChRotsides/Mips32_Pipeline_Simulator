#include "../label_struct/label_struct.h"
#include "../instruction/instruction.h"
#include <vector>
#ifndef assembler_h
#define assembler_h

#define RTYPE 0
#define ITYPE 1
#define BRANCH 2
#define MEMOP 3
#define SHIFTOP 4

class assembler{

public:
  inline static const std::string opString[22]={"add","addi","addiu","addu","and","andi","beq","bne","lw",  "nor","or","ori","slt","slti","sltiu","sltu","sll","srl","sub","subu","sw"};
  inline static const uint32_t opCodes[22]={     0x0,  0x8,    0x9,   0x0,   0x0,  0xc,   0x4,  0x5,  0x23,  0x0, 0x0,  0xd,  0x0,  0xa,    0xb,    0x0,  0x0,  0x0,  0x0,  0x0,  0x2b};
  inline static const uint8_t opType[22]=   {     0,    1,      1 ,    0,     0,    1,     2,    2,    3,     0,   0,    1,    0,    1,      1,      0,    4,    4,    0,    0,    3};
  inline static const uint32_t func[22]=   {     0x20, 0x0,    0x2c,  0x21,  0x24, 0x2c,  0x2c, 0x2c, 0x2c,  0x27,0x25, 0x2c, 0x2a, 0x2c,    0x2c,  0x2b, 0x0,  0x2,  0x22, 0x23, 0x2c };
  inline static const uint8_t supportedInstructions=21;


  static std::string getBinary(uint32_t val,int bits);
  static void printValues();
  static uint32_t createInstruction(instruction &Instruction,std::vector<label> labels,int pc);

};

#endif
