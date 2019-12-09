#include "ALU.hpp"
#include "ISA.hpp"
#include "decode.hpp"
#include "memory.hpp"
#include <cmath>
#include <vector>
#include <iomanip>
using namespace std;

int execute(ALU &alu, ISA instructions, int registers[N_REGISTERS], vector<int> register_file, int immediate);
void end();
