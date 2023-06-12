// #include <stdint.h>
#ifndef instruction_h
#define instruction_h




class instruction{
public:
  std::string operation;
  std::string argument0;
  std::string argument1;
  std::string argument2;

  union{

    struct{
      uint8_t byte[4];
    };

    uint32_t word;

  }encoding;

  char opType;

  instruction(std::string Operation,std::string Argument0,std::string Argument1,std::string Argument2);
  instruction();
  std::string getString();
};


#endif
