#include "ALU.hpp"
#include "ISA.hpp"
#include "decode.hpp"
#include "execute.hpp"
#include "fetch.hpp"
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

//TODO CHECK EXECUTE.CPP

using namespace std;

int main()
{
	PC = 0;
	ALU alu;
	Fetch fetch;
	int registers[N_REGISTERS] = { 0 };
	vector<string> tokens, code;
	vector<int> register_file;
	int immediate;
	fetch.getCode(&code);
	for (;;)
	{
		fetch.getInstruction(code[PC], &tokens);
		fetch.getRegisters(code[PC], &register_file);
		immediate = fetch.getImmediate(code[PC]);
		PC++;
		execute(alu, cmp(tokens[0]), registers, register_file,
                immediate);
	}
	return 0;
}
