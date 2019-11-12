#include "ALU.hpp"
#include "ISA.hpp"
#include "decode.hpp"
#include "memory.hpp"
#include <vector>
#include <cmath>
using namespace std;

int execute(ALU alu, ISA instructions, int registers[64], vector<int> register_file, int immediate);
