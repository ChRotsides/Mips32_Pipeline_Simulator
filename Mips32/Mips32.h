#include <sstream>
#ifndef Mips32_h
#define Mips32_h



class Mips32{

public:


  enum instructions_{add,addi,addiu,addu,andi,
    beq,bne,j,lw,nor,ori,slt,
    slti,sltiu,sltu,sll,srl,sw,sub,subu,and_m,or_m,nothing};
  instructions_ str2enum(std::string instruction){
    if(instruction=="add") return add;
    if(instruction=="addi") return addi;
    if(instruction=="addiu") return addiu;
    if(instruction=="addu") return addu;
    if(instruction=="and") return and_m;
    if(instruction=="andi") return andi;
    if(instruction=="beq") return beq;
    if(instruction=="bne") return bne;
    if(instruction=="j") return j;
    if(instruction=="lw") return lw;
    if(instruction=="nor") return nor;
    if(instruction=="or") return or_m;
    if(instruction=="ori") return ori;
    if(instruction=="slt") return slt;
    if(instruction=="slti") return slti;
    if(instruction=="sltiu") return sltiu;
    if(instruction=="sltu") return sltu;
    if(instruction=="sll") return sll;
    if(instruction=="srl") return srl;
    if(instruction=="sw") return sw;
    if(instruction=="sub") return sub;
    if(instruction=="subu") return subu;
    return nothing;
  }
  Mips32();
  Mips32(int32_t *instructionsBin,std::vector<instruction> instructionsV,int size);
  ~Mips32();

    register_file registers;
    int32_t *DMEM=new int32_t[0x100000000];
    uint32_t *IMEM=new uint32_t[0x100000000];
    std::vector<int> memory_addresses;
    std::vector<instruction> instructions;
    int32_t PC=0;

    struct IFreg_t{
      instruction fullInstraction;
      uint32_t FPC;
      uint32_t Target=0;
      uint32_t instr;
    }IF;
    IFreg_t prevIF;
    struct IDreg_t{
      instruction fullInstraction;
      int32_t BranchAddress;
      uint8_t RS;
      uint8_t RD;
      uint8_t RT;
      uint8_t opcode;
      uint8_t shamnt;
      uint8_t func;
      bool branching;
      int32_t Register1Data;
      int32_t Register2Data;
      int32_t imm;
      uint8_t destinationRegister;

    }ID;
    IDreg_t prevID;

    struct EXreg_t{

      instruction fullInstraction;
      int32_t AluResult;
      int32_t WritetoMemData;
      bool memRead;
      bool memWrite;
      uint8_t destinationRegister;
      bool writeToReg;
      int32_t BranchAddress;
      bool branching;

    }EX;
    EXreg_t prevEX;

    struct MEMreg_t{

      instruction fullInstraction;
      uint32_t memAddress;
      int32_t WritetoMemData;
      int32_t dataRead;
      int32_t AluResult;
      uint8_t destinationRegister;
      
      bool memToReg;
      bool writeToReg;
      int32_t BranchAddress;
      bool branching;

    }ME;
    MEMreg_t prevME;


    struct WBreg_t{
      instruction fullInstraction;
      int32_t WriteToRegData;
      bool writeToReg;
      uint8_t destinationRegister;
      int32_t BranchAddress;
      bool branching;
      int32_t muxOutData;
    }WrBa;

    WBreg_t prevWrBa;
    std::stringstream ss;
    std::stringstream cycleMonitors;
    std::string getStringForPrint();

    struct monitor_t{
      std::string monitor1="-";
      std::string monitor2="-";
      std::string monitor3="-";
      std::string monitor4="-";
      std::string monitor5="-";
      std::string monitor6="-";
      std::string monitor7="-";
      std::string monitor8="-";
      std::string monitor9="-";
      std::string monitor10="-";
      std::string monitor11="-";
      std::string monitor12="-";
      std::string monitor13="-";
      std::string monitor14="-";
      std::string monitor15="-";
      std::string monitor16="-";
      std::string monitor17="-";
      std::string monitor18="-";
      std::string monitor19="-";
      std::string monitor20="-";
      std::string monitor21="-";
      std::string monitor22="-";
      std::string monitor23="-";
      std::string monitor24="-";
      std::string monitor25="-";
      std::string monitor26="-";
      bool monitor27=0;
      bool monitor28=0;
    }monitors;

    void loadIMEM(uint32_t *instructionsBin,std::vector<instruction> instructionsV,int size);
    int32_t ALU(int32_t inA,int32_t inB,uint8_t opcode,uint8_t func,uint8_t shamnt);
    int32_t mux_3x1(uint8_t select,int32_t in0,int32_t in1,int32_t in2);
    int32_t mux_2x1(uint8_t select,int32_t in0,int32_t in1);
    bool dataHazardAndForwadingUnit(uint8_t rs,uint8_t rt,uint8_t rdFromMem,uint8_t rdFromWB,bool writeToRegMem,bool writeToRegWb,uint8_t &muxAselect,uint8_t &muxBselect);
    bool controlLuHazardDetection(uint8_t rs,uint8_t rt,uint8_t opcode,int cycle);
    void IFetch();
    void IDecode(int cycle);
    void Execute();
    void MEM();
    void WB();
    bool tick(int cycle);
    void saveState(int cycle);
    std::string getState();
    std::string getFinalState();
    int lus=0;
    int endFlag=0;
    int endFlagTrig=0;


  // std::string get_state(int pc);

  };


#endif
