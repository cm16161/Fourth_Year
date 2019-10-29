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
	vector<int> register_file;
	int immediate;
	fetch.getCode(file_name, &code);
	int clock = 0;
	for (;;)
	{
		fetch.getInstruction(code[PC], &tokens);
		decode.getRegisters(code[PC], &register_file);
		immediate = decode.getImmediate(code[PC]);
		ISA inst = decode.decode(tokens[0]);
		PC++;
		execute(inst, registers, register_file, immediate);
		clock++;
	}
	return 0;
}
