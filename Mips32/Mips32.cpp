#include <iostream>
#include "../register_file/register_file.h"
#include "../parser/parser.h"
#include <string>
#include <vector>
#include <sstream>
#include "../assembler/assembler.h"
#include <algorithm>
  #include <thread>
#include "./Mips32.h"

Mips32::Mips32(){
  // for(unsigned int i=0; i<(1073741824); i++){
  //   mips_ram[i]=0;
  // }

}
Mips32::Mips32(int32_t *instructionsBin,std::vector<instruction> instructionsV,int size){
  // for(unsigned int i=0; i<(1073741824); i++){
  //   mips_ram[i]=0;
  // }
  for(int i=0; i<size; i++){
    IMEM[i]=instructionsBin[i];
  }
  IMEM[size]=0x0;
  IMEM[size+1]=0x0;
  IMEM[size+2]=0x0;
  instructions=instructionsV;
}

void Mips32::loadIMEM(uint32_t *instructionsBin,std::vector<instruction> instructionsV,int size){
  for(int i=0; i<size; i++){
    IMEM[i]=instructionsBin[i];
  }
  instructions=instructionsV;

}

Mips32::~Mips32(){

  free(IMEM);
  free(DMEM);

}


void Mips32::IFetch(){
  //Instruction fetch //

      
      PC=IF.Target;
      // std::cout<<"\nPC: "<<PC<<"\n";
      uint32_t instr=IMEM[PC];
      if(PC<instructions.size() && IMEM[PC]==0x0){
        IF.fullInstraction=instructions[PC];
        endFlag=1;
        endFlagTrig=1;
      }else if(endFlag==1){//found sll keep passing bubbles until all instructions are executed
        IF.fullInstraction.encoding.word=0x20000000;
        IF.fullInstraction={"-","","",""};
        IF.fullInstraction.opType=5;
        // endFlag=0;
      }else if(PC<instructions.size()){
        IF.fullInstraction=instructions[PC];
        // endFlag=0;
      }

      //Monitors
      monitors.monitor2=parser::getHexStringFromInt(PC*4);
      monitors.monitor3=IF.fullInstraction.getString();

      if(endFlag==0){
          // PC=PC+1;
        }



      // std::cout<<"\nPC: 0x"<<std::hex<<PC;
      ss<<"\nPC: "<<std::dec<<PC;

      IF.FPC=PC+1;
      IF.instr=instr;
      // std::cout<<"\nIF:"<<IF.fullInstraction.getString();
      ss<<"\nIF:"<<IF.fullInstraction.getString();
      IF.Target=PC+1;
      monitors.monitor1=parser::getHexStringFromInt(IF.Target*4);
      
}

void Mips32::IDecode(int cycle){

    uint32_t FPC=prevIF.FPC;
    uint32_t instr=prevIF.instr;

    uint32_t temp;
    uint8_t rs,rt,rd,opcode,shamnt,func;
    int32_t imm;
    int32_t BranchAddress;
    //Calculate Branch Address
    temp=instr;

    if(((temp&0x00008000))==0x8000){//if the last bit of the 16bit number is 1 (check sign)
      temp=instr;
      temp=temp|(0xffff0000);//Sign extend
      // std::cout<<"\nNEG BRANCH OFFET: "<<std::hex<<(int)(temp);
      BranchAddress=(int)FPC+(int)(temp);
    }else{
      // std::cout<<"\nPOS BRANCH OFFET: "<<std::hex<<(int)(temp&0x0000ffff);
      BranchAddress=FPC+(instr&0x0000ffff);
    }
    // std::cout<<"\nNew PC: 0x"<<BranchAddress;
    ss<<"\nNew PC: "<<std::dec<<BranchAddress;

    //find rs register
    temp=instr;
    temp=temp>>21;
    temp=temp&(0x0000001f);
    rs=temp;

    //find rt
    temp=instr;
    temp=temp>>16;
    temp=temp&(0x0000001f);
    rt=temp;

    //find rd
    temp=instr;
    temp=temp>>11;
    temp=temp&(0x0000001f);
    rd=temp;

    // find opcode
    temp=instr;
    temp=temp>>26;
    temp=temp&(0x0000003f);
    opcode=temp;

    //find imm
    temp=instr;
    if((temp&(0x00008000))==0x8000){//if the last bit of the 16bit number is 1 (check sign)
      temp=instr;
      temp=temp|(0xffff0000);//Sign extend
      imm=(temp);
    }else{
      imm=(instr&0x0000ffff);
    }
    
      // std::cout<<"\nImm: "<<(int)imm<<"\n";
    
    // find shift amount
    temp=instr;
    temp=temp>>6;
    temp=temp&(0x0000001f);
    shamnt=temp;

    //find func
    temp=instr;
    temp=temp&(0x0000003f);
    func=temp;


    ID.BranchAddress=BranchAddress;
    ID.RS=rs;
    ID.RD=rd;
    ID.RT=rt;
    ID.opcode=opcode;
    ID.imm=imm;
    ID.shamnt=shamnt;
    ID.func=func;
    ID.fullInstraction=prevIF.fullInstraction;
    
    int32_t Register1Data=registers[rs];
    int32_t Register2Data=registers[rt];
    ID.branching=0;

    uint8_t sA=0,sB=0;
    bool branchHAZARD;
    // branchHAZARD=dataHazardAndForwadingUnit(rs,rt,prevEX.destinationRegister,prevME.destinationRegister,prevEX.writeToReg,prevME.writeToReg,sA,sB);
      
    int32_t num1=mux_3x1(sA,Register1Data,prevME.AluResult,prevEX.AluResult);

    int32_t num2=mux_3x1(sB,Register2Data,prevME.AluResult,prevEX.AluResult);

    // ss<<"\nID_Stage:\n"<<std::hex<<"sA: "<<(uint)sA<<" num1: "<<num1<<" sB: "<<(uint)sB<<" num2: "<<num2<<" Hazard: "<<branchHAZARD<<"\n";
    ID.Register1Data=Register1Data;
    ID.Register2Data=Register2Data;

    if(opcode==0x0){
        ID.destinationRegister=rd;
    }else if(opcode==0x8 ||opcode==0x9||opcode==0xc||opcode==0xd||opcode==0xa||opcode==0xb||0x23){
        ID.destinationRegister=rt;
      }
      ss<<"\nID:"<<ID.fullInstraction.getString();
    bool hazardDetection=controlLuHazardDetection(rs,rt,opcode,cycle);
    monitors.monitor27=hazardDetection;
    if(hazardDetection==1){
      endFlag=0;
    }

    //we are branching
    if(((opcode==0x4 && num1==num2)||(opcode==0x5 && num1!=num2))&& hazardDetection==0){
        // std::cout<<"\nBranch Resolution: 1 : "<<BranchAddress;
        ss<<"\nBranch Resolution: 1 : "<<BranchAddress;
        IF.instr=0x20000000;
        IF.fullInstraction={"-","","",""};
        IF.fullInstraction.encoding.word=0x20000000;
        IF.fullInstraction.opType=5;
        IF.Target=BranchAddress;
        monitors.monitor1=prevIF.fullInstraction.argument2; //Target Monitor
        ID.branching=1;
        endFlag=0;
        endFlagTrig=0;
      
    }else if(hazardDetection==0){
      std::stringstream tss;
      tss<<std::hex<<(PC+1)*4;
      monitors.monitor1=tss.str();//Target Monitor
      IF.Target=PC+1;
      ID.branching=0;
      if(endFlagTrig==1){
        endFlag=1;
      }
      // std::cout<<"\nBranch Resolution: 0";
    }







    // std::cout<<"\nID:"<<ID.fullInstraction.getString();
    


    // Execute();
    
    //Monitors;
    if(opcode==0x4|| opcode==0x5){
    
      monitors.monitor4=prevIF.fullInstraction.argument2;
      
    }else{
      monitors.monitor4="-";
    }

    if(prevIF.fullInstraction.opType==0){//if instruction is Rtype
      monitors.monitor5=prevIF.fullInstraction.argument1;
      monitors.monitor6=prevIF.fullInstraction.argument2;
      monitors.monitor9=parser::getHexStringFromInt(Register1Data);
      monitors.monitor10=parser::getHexStringFromInt(Register2Data);
      monitors.monitor11="-";
      monitors.monitor12=prevIF.fullInstraction.argument1;
      monitors.monitor13=prevIF.fullInstraction.argument2;
      monitors.monitor14="-";
      monitors.monitor15=prevIF.fullInstraction.argument0;
    }else if(prevIF.fullInstraction.opType==1){//if instruction is Itype
      monitors.monitor5=prevIF.fullInstraction.argument1;
      monitors.monitor6="-";
      monitors.monitor9=parser::getHexStringFromInt(Register1Data);
      monitors.monitor10="-";
      monitors.monitor11=parser::getHexStringFromInt(imm);
      monitors.monitor12=prevIF.fullInstraction.argument1;
      monitors.monitor13="-";
      monitors.monitor14=prevIF.fullInstraction.argument0;
      monitors.monitor15="-";
    }else if(prevIF.fullInstraction.opType==2){
      monitors.monitor5=prevIF.fullInstraction.argument0;
      monitors.monitor6=prevIF.fullInstraction.argument1;
      monitors.monitor9=parser::getHexStringFromInt(Register1Data);
      monitors.monitor10=parser::getHexStringFromInt(Register2Data);
      monitors.monitor11="-";
      monitors.monitor12=prevIF.fullInstraction.argument0;
      monitors.monitor13="-";
      monitors.monitor14="-";
      monitors.monitor15="-";
    }else if(prevIF.fullInstraction.opType==3){
      std::string offset_t="";
      monitors.monitor5=parser::get_register_from_word_instruction(prevIF.fullInstraction.argument1,&offset_t);
      monitors.monitor6=prevIF.fullInstraction.argument0;
      monitors.monitor9=parser::getHexStringFromInt(Register1Data);
      if(opcode==0x23){//lw
        monitors.monitor10="-";
        monitors.monitor13="-";
        monitors.monitor14=prevIF.fullInstraction.argument0;
        
      }else{
        monitors.monitor10=parser::getHexStringFromInt(Register2Data);
        monitors.monitor13=prevIF.fullInstraction.argument0;
        monitors.monitor14="-";
      }
      monitors.monitor11=parser::getHexStringFromInt(imm);
      monitors.monitor12=monitors.monitor5;

      monitors.monitor15="-";
    }else if(prevIF.fullInstraction.opType==4){
      monitors.monitor5="-";
      monitors.monitor6=prevIF.fullInstraction.argument1;
      monitors.monitor9="-";
      monitors.monitor10=parser::getHexStringFromInt(Register2Data);
      monitors.monitor11="-";
      monitors.monitor12="-";
      monitors.monitor13=prevIF.fullInstraction.argument1;
      monitors.monitor14="-";
      monitors.monitor15=prevIF.fullInstraction.argument0;
    }else{//nop
      monitors.monitor5="-";
      monitors.monitor6="-";
      monitors.monitor9="-";
      monitors.monitor10="-";
      monitors.monitor11="-";
      monitors.monitor12="-";
      monitors.monitor13="-";
      monitors.monitor14="-";
      monitors.monitor15="-";
    }
}

void Mips32::Execute(){

      int32_t Register1Data=prevID.Register1Data;
      int32_t Register2Data=prevID.Register2Data;
      uint8_t rs=prevID.RS;
      uint8_t rt=prevID.RT;
      uint8_t rd=prevID.RD;
      uint8_t opcode=prevID.opcode;
      uint8_t shamnt=prevID.shamnt;
      uint8_t func=prevID.func;
      int32_t imm=prevID.imm;
      int32_t AluResult=0;

      // if(prevID.branching==1){
      //   // PC=prevID.BranchAddress;
      //   // IF.Target=prevID.BranchAddress;
      // }
      EX.branching=prevID.branching;
      EX.BranchAddress=prevID.BranchAddress;

      // std::cout<<"\ninA: "<<(int)Register1Data<<" inB: "<<(int)Register2Data<<"\n";
      // std::cout<<"imm: "<<imm;
      uint8_t sA,sB;
      int32_t AluAMuxOut,AluBMuxOut;

      bool forwading=dataHazardAndForwadingUnit(rs,rt,prevEX.destinationRegister,prevME.destinationRegister,prevEX.writeToReg,prevME.writeToReg,sA,sB);
      monitors.monitor28=forwading;
      AluAMuxOut=mux_3x1(sA,Register1Data,WrBa.muxOutData,prevEX.AluResult);
      int32_t inA=AluAMuxOut;
      
      AluBMuxOut=mux_3x1(sB,Register2Data,WrBa.muxOutData,prevEX.AluResult);
      int32_t inB=mux_2x1(((opcode>>3&0x1)||(opcode>>5&0x1)),AluBMuxOut,imm);
      
      ss<<"\nEX_Stage:\n"<<"sA: "<<std::hex<<(int)sA<<" InA: "<<std::hex<<inA<<" sB: "<<std::hex<<(int)sB<<" inB: "<<std::hex<<inB;

      AluResult=ALU(inA,inB,opcode,func,shamnt);
      ss<<" Alu Result: "<<std::hex<<AluResult<<" sB: "<<(int)sB<<" Register2Data: "<<Register2Data;
      EX.AluResult=AluResult;
      EX.WritetoMemData=AluBMuxOut;
      ss<<" WritetoMemData: "<<AluBMuxOut<<"\n";

      ss<<" MEM_rd: "<<(int)prevEX.destinationRegister<<" WB_rd: "<<(int)prevME.destinationRegister<<" MEM WTR: "<<(int)prevEX.writeToReg<<" WB WTR: "<<(int)prevME.writeToReg;
      if(opcode==0x2b){
        EX.memWrite=1;
        EX.memRead=0;
      }else if(opcode==0x23){
        EX.memRead=1;
        EX.memWrite=0;
      }else{
        EX.memRead=0;
        EX.memWrite=0;
      }


      EX.destinationRegister=prevID.destinationRegister;

      EX.writeToReg=1;
      if(opcode==0x4||opcode==0x5||opcode==0x2b){
        EX.writeToReg=0;
      }
      EX.fullInstraction=prevID.fullInstraction;
      // std::cout<<"\nEX:"<<EX.fullInstraction.getString();
      ss<<"\nEX:"<<EX.fullInstraction.getString();
      // std::cout<<"\nRT: "<<(int)rt<<"RS:"<<(int)rs;
      // std::cout<<"\nDST: "<<(int)EX.destinationRegister;
      // MEM();

      if(PC==14){
        // std::cout<<"\n"<<prevID.fullInstraction.getString()<<" opType="<<(int)prevID.fullInstraction.opType<<"\n";
        // std::cout<<"EX_Stage:\n"<<"sA: "<<std::hex<<(int)sA<<" InA: "<<std::hex<<inA<<" sB: "<<std::hex<<(int)sB<<" inB: "<<std::hex<<inB<<"\n";
      }

      //Monitors
      
      if(prevID.fullInstraction.opType==0){//rtype
        monitors.monitor16=parser::getHexStringFromInt(inA);
        monitors.monitor17=parser::getHexStringFromInt(inB);
        monitors.monitor18=parser::getHexStringFromInt(AluResult);
        monitors.monitor19="-";
      }else if(prevID.fullInstraction.opType==1){//itype
        monitors.monitor16=parser::getHexStringFromInt(inA);
        monitors.monitor17=parser::getHexStringFromInt(inB);
        monitors.monitor18=parser::getHexStringFromInt(AluResult);
        monitors.monitor19="-";
      }else if(prevID.fullInstraction.opType==3){//memtype
        monitors.monitor16=parser::getHexStringFromInt(inA);
        monitors.monitor17=parser::getHexStringFromInt(inB);
        monitors.monitor18=parser::getHexStringFromInt(AluResult);
        if(opcode==0x23){//lw
          monitors.monitor19="-";

        }else{
          monitors.monitor19=parser::getHexStringFromInt(AluBMuxOut);
        }
      }else if(prevID.fullInstraction.opType==2){//beq bne
        monitors.monitor16="-";
        monitors.monitor17="-";
        monitors.monitor18="-";
        monitors.monitor19="-";
        
        }else if(prevID.fullInstraction.opType==4){//sll srl
        monitors.monitor16="-";
        monitors.monitor17=parser::getHexStringFromInt(inB);
        monitors.monitor18=parser::getHexStringFromInt(AluResult);
        monitors.monitor19="-";
      }else if(prevID.fullInstraction.opType==5){//nop
        monitors.monitor16="-";
        monitors.monitor17="-";
        monitors.monitor18="-";
        monitors.monitor19="-";
      }


}

void Mips32::MEM(){

      int32_t AluResult=prevEX.AluResult;
      int32_t WriteData=prevEX.WritetoMemData;
      bool memRead=prevEX.memRead;
      bool memWrite=prevEX.memWrite;
      uint8_t destinationRegister=prevEX.destinationRegister;
      bool writeToReg=prevEX.writeToReg;
      ME.writeToReg=writeToReg;
      ME.memToReg=0;
      ME.AluResult=AluResult;
      // ME.memRead=memRead;
      // ME.memWrite=memWrite;
      ss<<"\nAluResult:"<<std::hex<<AluResult;
      ME.destinationRegister=destinationRegister;
      if(memRead==1){
        ME.dataRead=DMEM[(uint32_t)AluResult];
        ME.memToReg=1;
      }
      if(memWrite==1){
        DMEM[(uint32_t)AluResult]=prevEX.WritetoMemData;
        // std::cout<<"\nMemWrite "<<(int)destinationRegister<<"\n";
        // std::cout<<"\nMem address: "<<(int)AluResult<<"\n";
        // std::cout<<"\nData: "<<(int)prevEX.WritetoMemData<<"\n";
        ss<<"\nMemWrite "<<(int)destinationRegister<<"\n";
        ss<<"\nMem address: "<<(int)AluResult<<"\n";
        ss<<"\nData: "<<(int)prevEX.WritetoMemData<<"\n";
        ME.memToReg=0;
        if(std::find(memory_addresses.begin(),memory_addresses.end(),(uint32_t)AluResult)==memory_addresses.end()){
          memory_addresses.push_back((uint32_t)AluResult);
        }
      }
      ME.fullInstraction=prevEX.fullInstraction;
      // std::cout<<"\nME:"<<ME.fullInstraction.getString();
      ss<<"\nME:"<<ME.fullInstraction.getString();
      // std::cout<<"\nDST: "<<(int)destinationRegister;

      // WB();
      ME.branching=prevEX.branching;
      ME.BranchAddress=prevEX.BranchAddress;

      // if(PC==31){
      //   std::cout<<"\nInstr: "<<prevEX.fullInstraction.getString()<<" opType: "<<prevEX.fullInstraction.opType<<" opcode: "
      //             <<std::hex<<"0x"<<(prevEX.fullInstraction.encoding.word>>26)<<"\n";
      // }
    if(prevEX.fullInstraction.opType==0||prevEX.fullInstraction.opType==1){//r and i type
      monitors.monitor20="-";
      monitors.monitor21="-";
      monitors.monitor22="-";
      monitors.monitor23=parser::getHexStringFromInt(prevEX.AluResult);
      monitors.monitor24=prevEX.fullInstraction.argument0;
    }else if(prevEX.fullInstraction.opType==2){//Branches
      monitors.monitor20="-";
      monitors.monitor21="-";
      monitors.monitor22="-";
      monitors.monitor23="-";
      monitors.monitor24="-";
    }else if(prevEX.fullInstraction.opType==3){//Mem ops
      monitors.monitor20=parser::getHexStringFromInt(prevEX.AluResult);
      monitors.monitor23=parser::getHexStringFromInt(prevEX.AluResult);
      if((prevEX.fullInstraction.encoding.word>>26)==0x23){//lw
      // if(PC==31){
      //   std::cout<<"\nInstr: "<<prevEX.fullInstraction.getString()<<"\n";
      // }
        monitors.monitor21="-";
        monitors.monitor22=parser::getHexStringFromInt(ME.dataRead);
        monitors.monitor24=prevEX.fullInstraction.argument0;
      }else{
      monitors.monitor21=parser::getHexStringFromInt(prevEX.WritetoMemData);
      monitors.monitor22="-";
      monitors.monitor24="-";
      }
    }else if(prevEX.fullInstraction.opType==4){//sll/srl
      monitors.monitor20="-";
      monitors.monitor21="-";
      monitors.monitor22="-";
      monitors.monitor23=parser::getHexStringFromInt(prevEX.AluResult);
      monitors.monitor24=prevEX.fullInstraction.argument0;
    }else if(prevEX.fullInstraction.opType==5){//nop
      monitors.monitor20="-";
      monitors.monitor21="-";
      monitors.monitor22="-";
      monitors.monitor23="-";
      monitors.monitor24="-";
    }


}

void Mips32::WB(){

    if(prevME.writeToReg==1){
      if(prevME.memToReg==1){//lw
        WrBa.muxOutData=prevME.dataRead;
        WrBa.WriteToRegData=prevME.dataRead;
        registers.set_register_value(prevME.destinationRegister,prevME.dataRead);
        // registers[ME.destinationRegister]=ME.dataRead;
      }else{// R or I types
        WrBa.muxOutData=prevME.AluResult;
        WrBa.WriteToRegData=prevME.AluResult;
        registers.set_register_value(prevME.destinationRegister,prevME.AluResult);
        // std::cout<<"\nAlu result wb:"<<prevME.AluResult;
      }
    }

    WrBa.writeToReg=prevME.writeToReg;
    WrBa.destinationRegister=prevME.destinationRegister;
    WrBa.fullInstraction=prevME.fullInstraction;
    // std::cout<<"\nWB:"<<WrBa.fullInstraction.getString();
    ss<<"\nWB:"<<WrBa.fullInstraction.getString();


    // std::cout<<"\nAluResult: "<<std::hex<<prevME.AluResult<<" memRead: "<<std::hex<<prevME.dataRead;
    // std::cout<<"\nDST:"<<(int)prevME.destinationRegister;
    if (prevME.fullInstraction.opType==5){
        monitors.monitor7="-";
        monitors.monitor8="-";
        monitors.monitor25="-";
        monitors.monitor26="-";
    }else if(prevME.writeToReg==1){
    if(prevME.memToReg==1){
        monitors.monitor7=prevME.fullInstraction.argument0;
        monitors.monitor8=parser::getHexStringFromInt(prevME.dataRead);
        monitors.monitor25=prevME.fullInstraction.argument0;
        monitors.monitor26=parser::getHexStringFromInt(prevME.dataRead);
      }else{
        monitors.monitor7=prevME.fullInstraction.argument0;
        monitors.monitor8=parser::getHexStringFromInt(prevME.AluResult);
        monitors.monitor25=prevME.fullInstraction.argument0;
        monitors.monitor26=parser::getHexStringFromInt(prevME.AluResult);
      }
    }
}



bool Mips32::tick(int cycle){

  ss<<"\n"<<"-----Cycle: "<<std::dec<<cycle+1<<"-----\n";
  switch (cycle) {
    case 0:
      IFetch();
      prevIF=IF;
    break;
    case 1:
      IFetch();
      IDecode(cycle);
      prevIF=IF;
      prevID=ID;
    break;
    case 2:
      IFetch();
      IDecode(cycle);
      Execute();
      prevIF=IF;
      prevID=ID;
      prevEX=EX;
    break;
    case 3:
      IFetch();
      IDecode(cycle);
      Execute();
      MEM();
      prevIF=IF;
      prevID=ID;
      prevEX=EX;
      prevME=ME;
    break;
    default:
      MEM();
      //  std::cout<<"\nGetting Here 0\n";
      WB();
      //  std::cout<<"\nGetting Here 1\n";
      IFetch();
      //  std::cout<<"\nGetting Here 2\n";
      IDecode(cycle);
      //  std::cout<<"\nGetting Here 3\n";
      Execute();
      //  std::cout<<"\nGetting Here 4\n";
      MEM();
      //  std::cout<<"\nGetting Here 5\n";
     

      // saveState(cycle);
      // cycleMonitors<<prevIF.fullInstraction.getString();
      if(monitors.monitor27==0){
        prevIF=IF;
        prevID=ID;
      }else{
          prevID={{"-","","",""},0,0,0,0,0,0,0,0,0,0,0,0};
          prevID.fullInstraction.encoding.word=0x20000000;
          prevID.fullInstraction.opType=5;
      }
      
      
      prevEX=EX;
      prevME=ME;
      prevWrBa=WrBa;
    break;
  }

  saveState(cycle);

  // std::cout<<"\n Registers:\n";
  ss<<"\n Registers:\n";
  for(int i=0; i<32; i++){
    // std::cout<<registers.register_keys[i]<<": "<<std::hex<<registers[i]<<"\t";
    // ss<<registers_copy.register_keys[i]<<": "<<std::hex<<registers_copy[i]<<"\t";
    ss<<registers.register_keys[i]<<": "<<std::hex<<registers[i]<<"\t";
  }
  // ss<<"\n"<<registers.register_keys[21]<<": "<<std::hex<<registers["s5"]<<"\t";
  // ss<<registers.register_keys[20]<<": "<<std::hex<<registers["s4"]<<"\t";
  // ss<<registers.register_keys[11]<<": "<<std::hex<<registers["t3"]<<"\t";
  // std::cout<<"\n Memory:\n";
  ss<<"\n Memory:\n";
  for(int i=0; i<memory_addresses.size(); i++){
    // std::cout<<std::hex<<memory_addresses[i]<<": "<<DMEM[memory_addresses[i]]<<"\t";
    ss<<std::hex<<memory_addresses[i]<<": "<<DMEM[memory_addresses[i]]<<"\t";
    // ss<<std::hex<<memory_addresses_copy[i]<<": "<<DMEM[memory_addresses_copy[i]]<<"\t";
  }


  if(WrBa.fullInstraction.encoding.word==0x0 && cycle>4){
    std::cout<<WrBa.fullInstraction.getString();

      return false;
  }

return true;
}


int32_t Mips32::ALU(int32_t inA,int32_t inB,uint8_t opcode,uint8_t func,uint8_t shamnt){

        if(opcode==0x0){
              switch (func) {
                case 0x20://add
                // std::cout<<"\nADD: "<<inA+inB;
                  return inA+inB;
                break;
                case 0x21://addu
                  return ((uint32_t)inA+(uint32_t)inB);
                case 0x24://and
                  // std::cout<<"\nAND\n";
                  // return 0;
                  return inA&inB;
                break;
                case 0x27://nor
                  return ~(inA|inB);
                break;
                case 0x25://or
                  return  (inA|inB);
                break;
                case 0x2a://slt
                    return (inA<inB);
                break;
                case 0x2b://sltu
                  return ((uint32_t)inA<(uint32_t)inB);
                break;
                case 0x0://sll
                  return ((uint32_t)inB)<<shamnt;
                break;
                case 0x2://srl
                  return ((uint32_t)inB)>>shamnt;
                break;
                case 0x22://sub
                  return inA-inB;
                break;
                case 0x23://subu
                  return (uint32_t)inA-(uint32_t)inB;
                break;
              }
        }else if(opcode==0x8){//addi
          // std::cout<<"\nADDI: "<<inA+imm;
          return int32_t((int32_t)inA+(int32_t)inB);
        }else if(opcode==0x9){//addiu
          return (uint32_t)inA+(uint32_t)inB;
        }else if(opcode==0xc){//andi
          return inA&inB;
        }else if (opcode==0xd){//ori
          return inA|inB;
        }else if(opcode==0xa){//slti
          return (inA<inB);
        }else if(opcode==0xb){//sltiu
          return ((uint32_t)inA<(uint32_t)inB);
        }else if(opcode==0x23 || opcode ==0x2b){// lw sw
          return inA+inB;
        }
        return 0xffff0000;
}



std::string Mips32::getStringForPrint(){
  return ss.str();
}

void Mips32::saveState(int cycle){
  cycle++;
  cycleMonitors.str(std::string());
  cycleMonitors<<"\n-----Cycle "<<std::dec<<cycle<<"-----\n";
  cycleMonitors<<"Registers:\n";
  cycleMonitors<<std::hex<<PC*4<<"\t";
  
  for(int i=0; i<31; i++){
    cycleMonitors<<std::hex<<registers[i]<<"\t";    
  }
  cycleMonitors<<std::hex<<registers[31]<<"\n";
  
  // for(int i=0; i<31; i++){
  //   cycleMonitors<<registers.register_keys[i]<<": "<<std::hex<<registers[i]<<"\t";    
  // }
  // cycleMonitors<<registers.register_keys[31]<<": "<<std::hex<<registers[31]<<"\n";

  cycleMonitors<<"\nMonitors:\n";

  cycleMonitors<<monitors.monitor1<<"\t"
               <<monitors.monitor2<<"\t"
               <<monitors.monitor3<<"\t"
               <<monitors.monitor4<<"\t"
               <<monitors.monitor5<<"\t"
               <<monitors.monitor6<<"\t"
               <<monitors.monitor7<<"\t"
               <<monitors.monitor8<<"\t"
               <<monitors.monitor9<<"\t"
               <<monitors.monitor10<<"\t"
               <<monitors.monitor11<<"\t"
               <<monitors.monitor12<<"\t"
               <<monitors.monitor13<<"\t"
               <<monitors.monitor14<<"\t"
               <<monitors.monitor15<<"\t"
               <<monitors.monitor16<<"\t"
               <<monitors.monitor17<<"\t"
               <<monitors.monitor18<<"\t"
               <<monitors.monitor19<<"\t"
               <<monitors.monitor20<<"\t"
               <<monitors.monitor21<<"\t"
               <<monitors.monitor22<<"\t"
               <<monitors.monitor23<<"\t"
               <<monitors.monitor24<<"\t"
               <<monitors.monitor25<<"\t"
               <<monitors.monitor26<<"\t"
               <<monitors.monitor27<<"\t"
               <<monitors.monitor28<<"\n";

    // cycleMonitors<<"1: "<<monitors.monitor1<<"\t"
    //            <<"2: "<<monitors.monitor2<<"\t"
    //            <<"3: "<<monitors.monitor3<<"\t"
    //            <<"4: "<<monitors.monitor4<<"\t"
    //            <<"5: "<<monitors.monitor5<<"\t"
    //            <<"6: "<<monitors.monitor6<<"\t"
    //            <<"7: "<<monitors.monitor7<<"\t"
    //            <<"8: "<<monitors.monitor8<<"\t"
    //            <<"9: "<<monitors.monitor9<<"\t"
    //            <<"10: "<<monitors.monitor10<<"\t"
    //            <<"11: "<<monitors.monitor11<<"\t"
    //            <<"12: "<<monitors.monitor12<<"\t"
    //            <<"13: "<<monitors.monitor13<<"\t"
    //            <<"14: "<<monitors.monitor14<<"\t"
    //            <<"15: "<<monitors.monitor15<<"\t"
    //            <<"16: "<<monitors.monitor16<<"\t"
    //            <<"17: "<<monitors.monitor17<<"\t"
    //            <<"18: "<<monitors.monitor18<<"\t"
    //            <<"19: "<<monitors.monitor19<<"\t"
    //            <<"20: "<<monitors.monitor20<<"\t"
    //            <<"21: "<<monitors.monitor21<<"\t"
    //            <<"22: "<<monitors.monitor22<<"\t"
    //            <<"23: "<<monitors.monitor23<<"\t"
    //            <<"24: "<<monitors.monitor24<<"\t"
    //            <<"25: "<<monitors.monitor25<<"\t"
    //            <<"26: "<<monitors.monitor26<<"\t"
    //            <<"27: "<<monitors.monitor27<<"\t"
    //            <<"28: "<<monitors.monitor28<<"\n";



  cycleMonitors<<"\nMemory State:"<<"\n";
  if(memory_addresses.size()>=1){
  for(int i=0; i<memory_addresses.size()-1; i++){
    cycleMonitors<<DMEM[memory_addresses[i]]<<"\t";
    
  }
    cycleMonitors<<DMEM[memory_addresses[memory_addresses.size()-1]]<<"\n";
  }

  cycleMonitors<<"\nPipeline Stages:\n";
  if(cycle==1){
      cycleMonitors<<IF.fullInstraction.getString()<<"\n";

  }else if(cycle==2){
          cycleMonitors<<IF.fullInstraction.getString()<<"\t"
                  <<ID.fullInstraction.getString()<<"\n";
                  ;

  }else if(cycle==3){
          cycleMonitors<<IF.fullInstraction.getString()<<"\t"
                  <<ID.fullInstraction.getString()<<"\t"
                  <<EX.fullInstraction.getString()<<"\n";
  }else if(cycle==4){
          cycleMonitors<<prevIF.fullInstraction.getString()<<"\t"
                  <<ID.fullInstraction.getString()<<"\t"
                  <<EX.fullInstraction.getString()<<"\t"
                  <<ME.fullInstraction.getString()<<"\n";

  }else {
          cycleMonitors<<IF.fullInstraction.getString()<<"\t"
                  <<ID.fullInstraction.getString()<<"\t"
                  <<EX.fullInstraction.getString()<<"\t"
                  <<ME.fullInstraction.getString()<<"\t"
                  <<WrBa.fullInstraction.getString()<<"\n";

  }
  
}

std::string Mips32::getState(){
  return cycleMonitors.str();
}

std::string Mips32::getFinalState(){
  
  cycleMonitors.str(std::string());
  cycleMonitors<<"\n-----Final State-----\n";
  cycleMonitors<<"Registers:\n";
  cycleMonitors<<std::hex<<PC*4<<"\t";
  
  for(int i=0; i<31; i++){
    cycleMonitors<<std::hex<<registers[i]<<"\t";    
  }
  cycleMonitors<<std::hex<<registers[31]<<"\n";


  cycleMonitors<<"\nMemory State:"<<"\n";
  if(memory_addresses.size()>=1){
  for(int i=0; i<memory_addresses.size()-1; i++){
    cycleMonitors<<DMEM[memory_addresses[i]]<<"\t";
    
  }
    cycleMonitors<<DMEM[memory_addresses[memory_addresses.size()-1]]<<"\n";
  }

  return cycleMonitors.str();
}

int32_t Mips32::mux_3x1(uint8_t select,int32_t in0,int32_t in1,int32_t in2){

    switch (select)
    {
    case 0:
      return in0;
      break;
    case 1:
      return in1;
      break;
    case 2:
      return in2;
      break;
    default:
      std::cerr<<"\nGave Wrong select to mux3x1\n";
      exit(-1);
      break;
    }
}

int32_t Mips32::mux_2x1(uint8_t select,int32_t in0,int32_t in1){
      switch (select)
    {
    case 0:
      return in0;
      break;
    case 1:
      return in1;
      break;
    default:
      std::cerr<<"\nGave Wrong select to mux2x1\n";
      exit(-1);
      break;
    }
}

bool Mips32::dataHazardAndForwadingUnit(uint8_t rs,uint8_t rt,uint8_t rdFromMem,uint8_t rdFromWB,bool writeToRegMem,bool writeToRegWb,uint8_t &muxAselect,uint8_t &muxBselect){

      bool dataHazard=0;
      //Handle mux A

      if(rs==rdFromMem && writeToRegMem==1 && rs!=0){
        muxAselect=2;
        dataHazard=1;
      }else if(rs==rdFromWB && writeToRegWb==1 && rs!=0){
        muxAselect=1;
        dataHazard=1;
      }else{
        muxAselect=0;
      }
      //Handle mux B
      if(rt==rdFromMem && writeToRegMem==1 && rt!=0){
        muxBselect=2;
        dataHazard=1;
      }else if(rt==rdFromWB && writeToRegWb==1 && rt!=0){
        muxBselect=1;
        dataHazard=1;
      }else{
        muxBselect=0;
      }
      return dataHazard;
}


bool Mips32::controlLuHazardDetection(uint8_t rs,uint8_t rt,uint8_t opcode,int cycle){
    if(opcode==0x4||opcode==0x5){
              //Check for branch hazards
        if((prevID.destinationRegister==rs&&rs!=0)||(prevID.destinationRegister==rt&&rt!=0)){// Hazard in execution
          //halt pc //and don't overide
          // IF.Target=PC-1;//halts pc
          IF.Target=prevIF.Target;//keeps beq to id && halts pc
          ss<<"\nBranch Hazard Detected 0\n";
          ss<<"\nID:"<<ID.fullInstraction.getString();
          // std::cout<<"\nBranch Hazard Detected 0\t Cycle: "<<cycle+1;
          // std::cout<<"\nIntruction: "<<PC<<" "<<ID.fullInstraction.getString()<<"\n";
          return 1;

        }else if((prevEX.destinationRegister==rs && rs!=0)||(prevEX.destinationRegister==rt && rt!=0)){//Hazard in mem
          // IF.Target=PC-1;//halts pc
          IF.Target=prevIF.Target;//keeps beq to id && halts pc
          ss<<"\nBranch Hazard Detected 1\n";
          // std::cout<<"Branch Hazard Detected 1\t Cycle: "<<cycle+1;
          // std::cout<<"\nIntruction: "<<PC<<" "<<ID.fullInstraction.getString()<<"\n";
          ss<<"\nID:"<<ID.fullInstraction.getString();
          return 1;
        }
    }

        //LW hazards

    if(prevID.opcode==0x23){
      if((rs==prevID.RT||rt==prevID.RT)&&prevID.RT!=0){
        // ss<<"\nDETECTED LOAD USE HAZARD!!\n";
        std::cout<<"\nDETECTED LOAD USE HAZARD!!\tCycle: "<<cycle+1;
        std::cout<<"\nIntruction: "<<PC<<" "<<ID.fullInstraction.getString()<<"\n";
          // IF.Target=prevIF.Target;//halts pc
          // lus++;
          // PC=PC-1;
          ss<<"\nprevIF PC: "<<prevIF.Target<<"\n";
          IF.Target=prevIF.Target;//halts beq to id
          ss<<"\nLoad Use Hazard Detected\n"<<" lus: "<<lus;
          ss<<"\nID:"<<ID.fullInstraction.getString();         
          return 1;
      }
    }
    return 0;
}
