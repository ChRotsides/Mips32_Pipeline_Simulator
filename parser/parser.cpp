#include <vector>
#include <stdint.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "../label_struct/label_struct.h"
#include "../instruction/instruction.h"
#include "./parser.h"



bool parser::is_instruction(std::string partial_intruction_string){//Checks if is valid instruction

  std::string instruction_set[]={"add","addi","addiu","addu",
                                "and","andi","beq","bne","lw",
                                "nor","or","ori","slt","slti",
                                "sltiu","sltu","sll","srl","sw",
                                "sub","subu"};
    for(int i=0; i<21; i++){
      if(instruction_set[i]==partial_intruction_string){
        return 1;
      }
    }

return 0;
}


std::string parser::read_and_uncomment(std:: string filename_){//Read the program and removes any comments

    std::ifstream program(filename_,std::fstream::in);
    std::string program_string="";
    char input_char;
    char previous_char;
    std::string partial_intruction_string="";
    bool found_instruction=0;

    if(program.is_open()){
      while(program>>std::noskipws>>input_char){

        if (input_char=='#'){
          while(input_char!='\n'){  //Continue Reading characters until we find a new line
            program>>std::noskipws>>input_char;
        }
      }
        //Append character into the program string
        if(input_char!=9 && input_char!=32 && input_char!='j'){
          if(previous_char=='\n' && input_char=='\n' ){

          }else{
            program_string+=input_char;
              // std::cout<<input_char;
            previous_char=input_char;
          }
        }else if(input_char=='j'){
          // program_string+=input_char;
          while(input_char!='\n'  && !program.eof()){
            program>>std::noskipws>>input_char;
            // program_string+=input_char;
          }
          // input_char='';
        }
      }
      program.close();
    }

    else {
      std::cout<<"\nUnable to open file\n";
    }
    return program_string;
  }


  int parser::find_main(std::string program_string){
    std::string main="";
    for(int i=0; i<program_string.length(); i++){
        main+=program_string[i];
        if(program_string[i]==':'){
          if(main=="main:"){
            std::cout<<"\nFound Main\n";
            return (i-4);

          }

        }else if(program_string[i]=='\n'){
          main="";


        }
      }
      return -1;
  }

std::string parser::remove_spaces_and_dollar_signs(std::string str){
  std::string new_str="";

  for(int i=0; i<str.size(); i++){
    if(str[i]!='$' && str[i]!=' '){
      new_str+=str[i];
    }
  }

    return new_str;
}
std::string parser::remove_character(std::string str,char character){

  std::string new_str="";

  for(int i=0; i<str.size(); i++){
    if(str[i]!=character){
      new_str+=str[i];
    }
  }
  return new_str;
}



void parser::split_into_instructions(std::vector<instruction> *instructions_,std::vector<label> *labels_,std::string program_string){
  std::vector<instruction> instructions;
  std::string partial_instruction_string="";
  int instruction_counter=0;
  std::vector<label> labels;
  for(int i=parser::find_main(program_string); i<program_string.length(); i++){


    partial_instruction_string+=program_string[i];

    if(program_string[i]=='$'){
      partial_instruction_string.erase(partial_instruction_string.length()-1);
      if(parser::is_instruction(partial_instruction_string)){//Check if string matches an operation

        instructions.push_back(instruction(partial_instruction_string,"-","-","-"));
        instruction_counter++;
        partial_instruction_string="";
        int g=0;
        while(program_string[i-1]!='\n'){
          partial_instruction_string+=program_string[i];
          if(program_string[i]==','||program_string[i]=='\n'){
            // partial_instruction_string.erase(partial_instruction_string.length()-1);
            // std::cout<<partial_instruction_string<<" "<<g<<" ";
            switch(g){
              case 0:
              partial_instruction_string=parser::remove_character(partial_instruction_string,10);
              partial_instruction_string=parser::remove_character(partial_instruction_string,',');
                instructions[instruction_counter-1].argument0=partial_instruction_string;
                g++;
                break;
              case 1:
                  partial_instruction_string=parser::remove_character(partial_instruction_string,10);
                  partial_instruction_string=parser::remove_character(partial_instruction_string,',');
                  instructions[instruction_counter-1].argument1=partial_instruction_string;
                  g++;
                break;
              case 2:
                partial_instruction_string=parser::remove_character(partial_instruction_string,10);
                partial_instruction_string=parser::remove_character(partial_instruction_string,',');
                  instructions[instruction_counter-1].argument2=partial_instruction_string;
                  g++;
                break;
              }
              partial_instruction_string="";


            // std::cout<<"\n";
          }

            i++;
        }
          i--;
      }
    }else if(program_string[i]==' '){
      partial_instruction_string.erase(partial_instruction_string.length()-1);
      instructions.push_back(instruction(partial_instruction_string,"-","-","-"));
      instruction_counter++;
      partial_instruction_string="";
      while(program_string[i]!='\n'){
        partial_instruction_string+=program_string[i];
        i++;
      }
      partial_instruction_string=parser::remove_character(partial_instruction_string,10);
      partial_instruction_string=parser::remove_character(partial_instruction_string,',');
      partial_instruction_string=parser::remove_character(partial_instruction_string,' ');
      instructions[instruction_counter-1].argument0=partial_instruction_string;
      partial_instruction_string="";
    }else if(program_string[i]=='\n'){
      partial_instruction_string="";

    }else if(program_string[i]==':'){
      partial_instruction_string.erase(partial_instruction_string.length()-1);
      labels.push_back(label(partial_instruction_string,instruction_counter-1));
      partial_instruction_string="";
    }

  }
  *(instructions_)=instructions;
  *(labels_)=labels;

}

bool parser::is_hex(std::string str){

  if(str.compare(0,2,"0x") == 0 && str.size()>2 && str.find_first_not_of("0123456789abcdefABCDEF",2)==std::string::npos){
    return true;
  }
  return false;

}

bool parser::is_decimal(std::string str){

  if(str.find_first_not_of("0123456789-")==std::string::npos){
    return true;
  }
  return false;
}



bool parser::is_type_num(std::string sval){
  if(is_hex(sval)||is_decimal(sval)){
    return true;
  }
  return false;

}
int parser::getIntFromString(std::string sval){
  if(is_hex(sval)){
    return (int)std::strtol(sval.c_str(),NULL,0);
  }else if(is_decimal(sval)&&sval!="-"){
    // std::cout<<"\n"<<sval<<"\n";
    return (int)std::stoi(sval);
  }

  return -404;

}

std::string  parser::get_register_from_word_instruction(std::string argument ,std::string *offset){
  std::string new_argument;
  int register_string_index;
  for(int i=0; i<argument.size(); i++){
    if(argument[i]!='(')
      (*offset)+=argument[i];
    if(argument[i]=='('){
      register_string_index=i;
      break;
    }
  }
  for(int i=register_string_index; i<argument.size(); i++){

    if(argument[i]!='(' && argument[i]!=')'){
      new_argument+=argument[i];
    }
  }
  return new_argument;
}

std::string parser::IntFromStringToHexString(std::string sval){
  std::stringstream ss;
  int a;
  ss.str(std::string());
  if(is_type_num(sval)&&sval!="-"){
    a=getIntFromString(sval);
    ss<<std::hex<<a;
    return ss.str();
  }
    return sval;
}

std::string parser::getHexStringFromInt(uint32_t val){
    std::stringstream ss;
    ss.str(std::string());
    ss<<std::hex<<val;
    return ss.str();
}

bool parser::is_num(char ch){
  if(ch>=48 && ch<=57){
    return true;
  }
  return false;
}
