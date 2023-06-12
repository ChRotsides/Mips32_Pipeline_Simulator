#include <string>
#include <stdint.h>
#include <vector>
#include "./label_struct.h"

label::label(std::string name, int32_t pc):name(name),pc(pc){}

int32_t label::get_branch_address(std::vector<label> labels,std::string label_){

    for(int i=0; i<labels.size(); i++){
      if(labels[i].name==label_){
        return labels[i].pc;
      }
    }
    return -1;
}
