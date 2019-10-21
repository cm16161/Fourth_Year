#include "ALU.hpp"
#include "ISA.hpp"
#include "decode.hpp"
#include "memory.hpp"
#include <vector>
using namespace std;

void execute(ISA instructions, int registers[64], vector<int> register_file, int immediate);
