// #include <stdint.h>
#ifndef  register_file_h
#define register_file_h


class register_file{
public:
  int32_t registers[32];
  // std::string register_keys[32];

public:

  inline static const std::string register_keys[32]={"zero","at","v0","v1","a0","a1","a2","a3","t0","t1","t2","t3","t4","t5","t6","t7","s0","s1","s2","s3","s4","s5","s6","s7","t8","t9","k0","k1","gp","sp","fp","ra"};


register_file();
void set_register_value(std::string register_key,int32_t value);
int32_t get_register_value(std::string register_key);
static int8_t  get_index(std::string register_key);
int32_t operator[](std::string register_key);
int32_t operator[](int register_key);
void set_register_value(int8_t key,int32_t value);
// void operator[]=(int register_key);
static bool is_valid_register(std::string test_register);
};









#endif
