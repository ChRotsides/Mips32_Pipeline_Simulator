#include <string>
#include <iostream>
#include "../parser/parser.h"
#include "../label_struct/label_struct.h"
#include "../register_file/register_file.h"
#include <vector>
#include "assembler.h"

std::string assembler::getBinary(uint32_t val,int bits){

  std::string binString="00000000000000000000000000000000";
  std::string returnString;

  for(int i=bits-1; i>=0; i--){
    char a;
    a=(std::to_string(val&1)).c_str()[0];
    binString[i]=a;
    val=val>>1;
  }
  for(int i=0; i<bits; i++){
    returnString+=binString[i];
  }

  return returnString;
}


uint32_t assembler::createInstruction(instruction& Instruction,std::vector<label> labels,int pc){
      uint8_t index=0xff;
      uint32_t fullOP=0;
      uint32_t temp;
      uint8_t OPCode;
      uint8_t RS;
      uint8_t RT;
      uint8_t RD;
      uint8_t Shamnt;
      uint8_t Func;
      for(uint8_t i=0; i<supportedInstructions; i++){
          if(Instruction.operation==opString[i]){
            index=i;
            break;
          }
      }

      Instruction.opType=opType[index];

      if(opType[index]==RTYPE){//RTYPE
        Func=func[index];
        OPCode=opCodes[index];

        Shamnt=0;
        RT=register_file::get_index(Instruction.argument2);
        RD=register_file::get_index(Instruction.argument0);
        RS=register_file::get_index(Instruction.argument1);
        temp=OPCode;
        fullOP=temp<<26;
        temp=RS;
        fullOP=fullOP|(temp<<21);
        temp=RT;
        fullOP=fullOP|(temp<<16);
        temp=RD;
        fullOP=fullOP|(temp<<11);
        temp=Shamnt;
        fullOP=fullOP|(temp<<6);
        temp=Func;
        fullOP=fullOP|(temp);
      }else if (opType[index]==ITYPE){//ITYPE
      OPCode=opCodes[index];
      RT=register_file::get_index(Instruction.argument0);
      RS=register_file::get_index(Instruction.argument1);

      temp=OPCode;
      fullOP=temp<<26;
      temp=RS;
      fullOP=fullOP|(temp<<21);
      temp=RT;
      fullOP=fullOP|(temp<<16);
      int16_t immi=parser::getIntFromString(Instruction.argument2);
      temp=(immi<<16);
      fullOP=fullOP|(temp>>16);

      }else if (opType[index]==BRANCH){//BRANCH
        OPCode=opCodes[index];
        RT=register_file::get_index(Instruction.argument1);
        RS=register_file::get_index(Instruction.argument0);
        temp=OPCode;
        fullOP=temp<<26;
        temp=RS;
        fullOP=fullOP|(temp<<21);
        temp=RT;
        fullOP=fullOP|(temp<<16);
        int16_t branchAddress=label::get_branch_address(labels,Instruction.argument2);
        // std::cout<<"\nInstruction in branch: "<<Instruction.getString();
        // std::cout<<"\nBranch address: "<<branchAddress;
        // std::cout<<"\nPC: "<<pc;
        int16_t offset=branchAddress-pc;
        // std::cout<<"\nBranch Offset: "<<offset;
        temp=(offset<<16);
        fullOP=fullOP|(temp>>16);

      }else if(opType[index]==MEMOP){
        std::string offsetS;
        OPCode=opCodes[index];
        RT=register_file::get_index(Instruction.argument0);
        RS=register_file::get_index(parser::get_register_from_word_instruction(Instruction.argument1,&offsetS));
        temp=OPCode;
        fullOP=temp<<26;
        temp=RS;
        fullOP=fullOP|(temp<<21);
        temp=RT;
        fullOP=fullOP|(temp<<16);
        int16_t offset=parser::getIntFromString(offsetS);
        temp=(offset<<16);
        fullOP=fullOP|(temp>>16);
      }else if(opType[index]==SHIFTOP){
        Func=func[index];
        OPCode=opCodes[index];
        int16_t Shamnt=parser::getIntFromString(Instruction.argument2);

        RD=register_file::get_index(Instruction.argument0);
        RT=register_file::get_index(Instruction.argument1);
        RS=0;
        temp=OPCode;
        fullOP=temp<<26;
        temp=RS;
        fullOP=fullOP|(temp<<21);
        temp=RT;
        fullOP=fullOP|(temp<<16);
        temp=RD;
        fullOP=fullOP|(temp<<11);
        temp=Shamnt;
        fullOP=fullOP|(temp<<6);
        temp=Func;
        fullOP=fullOP|(temp);




      }

      Instruction.encoding.word=fullOP;

return fullOP;
}



void assembler::printValues(){

  for(uint8_t i=0; i<supportedInstructions; i++){

    if(opType[i]==0){
      std::cout<<"\n"<<opString[i]<<": "<<getBinary(opCodes[i],6)<<" func: "<<getBinary(func[i],6)<<"  RTYPE";
    }
    else if (opType[i]==1){
      std::cout<<"\n"<<opString[i]<<": "<<getBinary(opCodes[i],6)<<"  ITYPE";
    }else if (opType[i]==2){
      std::cout<<"\n"<<opString[i]<<": "<<getBinary(opCodes[i],6)<<"  BRANCH";
    }else if (opType[i]==3){
      std::cout<<"\n"<<opString[i]<<": "<<getBinary(opCodes[i],6)<<"  MEMOP";
    }

  }
  std::cout<<"\n";

}
