#include "ALU.hpp"
#include "ISA.hpp"
#include "decode.hpp"
#include "memory.hpp"
#include <vector>
using namespace std;

void execute(ALU alu, MEM &mem, ISA instructions, int registers[64], vector<int> register_file, int immediate);
