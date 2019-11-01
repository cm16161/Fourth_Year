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
int g_clock;
extern bool branch_taken;

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
	g_clock = 0;
	string current_line, current_inst;
	string IFID_command;
	int IDEX_immediate;
	ISA IDEX_command;
	vector<int> IDEX_registers;
	string IFID_instruction_keyword, IFID_instruction;
	vector<string> IF_instruction_tokens;
	vector<int> ID_registers;
	int ID_immediate;
	ISA ID_command;
	for (;;)
	{
		if ((g_clock % 2) == 0)
		{
			IF_instruction_tokens.clear();
			if (PC < code.size())
			{
				IF_instruction_tokens = fetch.getInstruction(code[PC]);
			}

			if (g_clock > 0)
			{
				ID_registers = decode.getRegisters(IFID_instruction);
				ID_immediate = decode.getImmediate(IFID_instruction);
				ID_command = decode.decode(IFID_command);
			}
			if (g_clock > 2)
			{
				execute(IDEX_command, registers, IDEX_registers, IDEX_immediate);
			}
		}
		else
		{
			if (!IF_instruction_tokens.empty())
			{
				IFID_command = IF_instruction_tokens[0];
				IFID_instruction = code[PC];
				if (!branch_taken)
				{
					PC++;
				}
				else
				{
					branch_taken = !branch_taken;
				}
			}
			if (g_clock > 1)
			{
				IDEX_registers = ID_registers;
				IDEX_immediate = ID_immediate;
				IDEX_command = ID_command;
			}
		}

		g_clock++;
	}
	return 0;
}
