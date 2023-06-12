#include <iostream>
#include "./assembler/assembler.h"
#include "./parser/parser.h"
#include <string>
#include "./instruction/instruction.h"
#include <vector>
#include <sstream>
#include <thread>
#include <fstream>
#include <chrono>
#include "./register_file/register_file.h"
#include "./Mips32/Mips32.h"
#include <iomanip>
#include "./label_struct/label_struct.h"




int main(int argc, char *argv[]){



  std::string partial_instruction_string="";
  std::string program_string="";
  Mips32 simulator;
  std::vector<instruction> instructions;
  std::vector<label> labels;
  int instruction_counter=0;


  program_string=parser::read_and_uncomment(argv[1]);



  parser::split_into_instructions(&instructions,&labels,program_string);
  std::stringstream ss;

  std::ofstream instructionOut("Instructions.txt");
  for(int i=0; i<instructions.size(); i++){
    std::cout<<"Instruction "<<std::dec<<i<<": "<<instructions[i].operation<<" "<<instructions[i].argument0<<" "<<instructions[i].argument1<<" "<<instructions[i].argument2<<"\n";
    ss<<"Instruction "<<std::dec<<i<<": "<<instructions[i].operation<<" "<<instructions[i].argument0<<" "<<instructions[i].argument1<<" "<<instructions[i].argument2<<"\n";
  }
  for(int i=0; i<labels.size(); i++){
  std::cout<<labels[i].name<<" "<<labels[i].pc<<"\n";
  ss<<labels[i].name<<" "<<labels[i].pc<<"\n";
}
labels[0].pc+=1;
  instructionOut<<ss.str();
  instructionOut.close();

  uint32_t *instructionsHex= new uint32_t[instructions.size()];
  ss.str(std::string());
  for(int i=0; i<instructions.size(); i++){
    uint32_t op=assembler::createInstruction(instructions[i],labels,i);
    // std::cout<<"\n"<<instructions[i].getString()<<" : 0x"<<std::setfill('0')<<std::setw(8)<<std::hex<<instructions[i].encoding.word;
    instructionsHex[i]=op;
    ss<<"0x"<<std::setfill('0')<<std::setw(8)<<std::hex<<instructions[i].encoding.word<<"\n";
    // ss<<std::dec<<i<<" : "<<std::setfill('0')<<std::setw(8)<<std::hex<<instructions[i].encoding.word<<";\n";

  }
  std::cout<<"\n";
  std::ofstream assemblerOutput("bin.txt");
  assemblerOutput<<ss.str();
  assemblerOutput.close();

  // std::cout<<program_string;

//find labels match them to a line and the split the string into lines of only instructions


  std::ofstream outFstream("dump.txt");
  std::ofstream finalStream;
  
  if(argv[2]!=NULL){
    finalStream.open(argv[2]);
  }else
  {
    std::cout<<"\nPlease Give as argument the output file name.\nExample:\nsimulator.exe inputfilename.s outputfilename.txt ( 1st cycle to print) ( 2nd cycle to print)\nExiting...\n";
    exit(-1);
  }
  finalStream<<"Name: Charalambos Rotsides\nID: 915289\n";
  std::vector<int> cyclesToPrint;

  for(int i=3; i<argc; i++){
    cyclesToPrint.push_back(std::atoi(argv[i]));
  }
  
  simulator.loadIMEM(instructionsHex,instructions,instructions.size());
  int cycle=0;
  auto start = std::chrono::high_resolution_clock::now();
    while(simulator.tick(cycle++)){
      // std::cout<<"\n\033[1;31m"<<"-----Cycle: "<<std::dec<<cycle<<"-----\033[0m\n";
      if(cyclesToPrint.size()==0){
        finalStream<<simulator.getState();
      }else{
        for(int i=0; i<cyclesToPrint.size(); i++){
          if(cycle==cyclesToPrint[i]){
            finalStream<<simulator.getState();
          }
        }
      }
      if(cycle>400){
        // break;
      }
    }
    std::cout<<"\nTotal Cycles: "<<cycle<<"\n";
    auto finish = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start);
    outFstream<<(simulator.getStringForPrint());
    finalStream<<simulator.getFinalState();

    finalStream<<"\nTotal Cycles:\n"<<std::dec<<cycle-1<<"\n";
    finalStream<<"\nTotal Execution Time:\n"<<std::dec<<duration.count()<<"\n";

    outFstream.close();
    finalStream.close();


  // for(int i=0; i<32; i++)
  //   std::cout<<assembler::getBinary(i,5)<<"\n";






  return 0;
}
