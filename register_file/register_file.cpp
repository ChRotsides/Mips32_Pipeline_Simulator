#include <stdint.h>
#include <string>
#include <iostream>
#include "../parser/parser.h"
#include "./register_file.h"


register_file::register_file(){
  for(int i=0; i<32; i++){
    registers[i]=0;
  }
  registers[28]=0x10008000;
  registers[29]=0x7ffffffc;
  // register_keys[0]="zero";
  // register_keys[1]="at";
  // for(int i=2; i<=3; i++){
  //   register_keys[i]="v"+std::to_string(i-2);
  // }
  // for(int i=4; i<=7; i++){
  //   register_keys[i]="a"+std::to_string(i-4);
  // }
  // for(int i=8; i<=15; i++){
  //   register_keys[i]="t"+std::to_string(i-8);
  // }
  // for(int i=16; i<=23; i++){
  //   register_keys[i]="s"+std::to_string(i-16);
  // }
  // for(int i=24; i<=25; i++){
  //   register_keys[i]="t"+std::to_string(i-16);
  // }
  // for(int i=26; i<=27; i++){
  //   register_keys[i]="k"+std::to_string(i-26);
  // }
  // register_keys[28]="gp";
  // register_keys[29]="sp";
  // register_keys[30]="fp";
  // register_keys[31]="ra";

  // for(int i=0; i<32; i++){
  //   // std::cout<<"\n"<<"R"<<i<<" "<<register_keys[i]<<"\n";
  //   std::cout<<'"'<<register_keys[i]<<'"'<<",";
  // }
}

void register_file::set_register_value(std::string register_key,int32_t value){

  register_key=parser::remove_spaces_and_dollar_signs(register_key);

  int register_index=get_index(register_key);

  registers[register_index]=value;
}

void register_file::set_register_value(int8_t key,int32_t value){

  registers[key]=value;
}


int32_t register_file::get_register_value(std::string register_key){
  register_key=parser::remove_spaces_and_dollar_signs(register_key);

  int register_index=get_index(register_key);
  return registers[register_index];

}
int32_t register_file::operator[](std::string register_key){

  int register_index=get_index(register_key);
  return registers[register_index];
}

int32_t register_file::operator[](int register_key){
  return registers[register_key];
}
// void register_file::operator[]=(int register_key,int32_t data){
//   registers[register_key]=data;
// }

int8_t  register_file::get_index(std::string register_key){
  register_key=parser::remove_spaces_and_dollar_signs(register_key);
  if(is_valid_register(register_key)==true){
  int8_t register_index=-1;
  for(int i=0; i<32; i++){
    if(register_keys[i]==register_key){
      register_index=i;
    }
  }
  if(register_index==-1){
    register_index=std::stoi(register_key);
  }
    return register_index;
  }

    std::cerr << "Register "<<register_key<<" Not Found" << '\n';
    exit(-1);
}

bool register_file::is_valid_register(std::string test_register){

  for(int i=0; i<32; i++){
    if(test_register==register_keys[i]){
      return true;
    }
  }
  if(parser::is_type_num(test_register)){
    return true;
  }

  return false;
}
