#include <vector>
#ifndef label_struct_h
#define label_struct_h


struct label
{
  std::string name;
  int32_t pc;

  label(std::string name, int32_t pc);
  static int32_t get_branch_address(std::vector<label> labels,std::string label_);

};


#endif
