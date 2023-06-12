#include <string>
#include <stdint.h>
#include <vector>
#include "../parser/parser.h"
#include "./instruction.h"


instruction::instruction(std::string Operation,std::string Argument0,std::string Argument1,std::string Argument2){
  operation=Operation;
  argument0=Argument0;
  argument1=Argument1;
  argument2=Argument2;
}

instruction::instruction(){}


std::string instruction::getString(){

  if(argument1=="-" && argument2=="-"){
    return operation+" "+argument0;
  }else if(argument2=="-"){
    return operation+" "+argument0+", "+argument1;
  }else if(operation=="-"){
    return "-";
  }
return operation+" "+argument0+", "+argument1+", "+argument2;



}
