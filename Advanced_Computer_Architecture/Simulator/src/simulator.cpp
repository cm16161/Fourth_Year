#include "ALU.hpp"
#include "ISA.hpp"
#include "decode.hpp"
#include "execute.hpp"
#include "fetch.hpp"
#include "memory.hpp"
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#define N_REGISTERS 64
int PC;
int executed_instructions;

//TODO CHECK EXECUTE.CPP

using namespace std;

int main(int argc, char *argv[])
{
	string file_name;
	executed_instructions = 0;
	if (argc > 1)
	{
		file_name = argv[1];
	}
	else
	{
		file_name = "../apps/all_ops.txt";
	}
	PC = 0;
	Fetch &fetch = Fetch::getInstance();
	Decode &decode = Decode::getInstance();
	int registers[N_REGISTERS] = { 0 };
	vector<string> tokens, code;
	vector<int> registers_to_use;
	int immediate;
	fetch.getCode(file_name, &code);
	int clock = 0;
	string current_line, current_inst;
	int immediate_to_use;
	ISA inst_to_use, inst;
	vector<int> pipeline_registers_to_use;
	for (;;)
	{
		fetch.getInstruction(code[PC], &tokens);
		decode.getRegisters(code[PC], &registers_to_use);
		immediate = decode.getImmediate(code[PC]);
		PC++;
		ISA inst = decode.decode(tokens[0]);
		execute(inst, registers, registers_to_use, immediate);
	}
	return 0;
}
