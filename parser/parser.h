#include <vector>
#include "../instruction/instruction.h"
#include "../label_struct/label_struct.h"
#ifndef parser_h
#define parser_h



class parser{

private:
  // instruction_mine wtd;
  parser();

public:

  static  bool is_instruction(std::string partial_intruction_string);
  static  std::string read_and_uncomment(std:: string filename);
  static  int find_main(std::string program_string);
  static  std::vector<int> v();
  static  std::string remove_spaces_and_dollar_signs(std::string str);
  static  std::string remove_character(std::string str,char character);
  static  void split_into_instructions(std::vector<instruction> *instructions_,std::vector<label> *labels_,std::string program_string);
  static  int getIntFromString(std::string sval);
  static  bool is_hex(std::string str);
  static  bool is_decimal(std::string str);
  static  bool is_num(char ch);
  static  bool is_type_num(std::string sval);
  static  std::string IntFromStringToHexString(std::string sval);
  static  std::string getHexStringFromInt(uint32_t val);
  static std::string  get_register_from_word_instruction(std::string argument,std::string *offset);

};



#endif
