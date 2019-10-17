#include "decode.hpp"
#include "ISA.hpp"
#include "ALU.hpp"
#include <vector>
using namespace std;

void execute(ALU alu, ISA instructions, int registers[64], vector<int> register_file, int immediate);
