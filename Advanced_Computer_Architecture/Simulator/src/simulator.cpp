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

	string IFID_command[2];
	int IDEX_immediate[2];
	ISA IDEX_command[2];
	vector<int> IDEX_registers[2];
	string IFID_instruction_keyword[2], IFID_instruction[2];
	vector<string> IF_instruction_tokens[2];
	vector<int> ID_registers[2];
	int ID_immediate[2];
	ISA ID_command[2];

	for (;;)
	{
		if ((g_clock % 2) == 0)
		{
			IF_instruction_tokens[0].clear();
			IF_instruction_tokens[1].clear();

			if (PC < code.size())
			{
				IF_instruction_tokens[0] = fetch.getInstruction(code[PC]);
				IF_instruction_tokens[1] = fetch.getInstruction(code[PC + 1]);
			}

			if (g_clock > 0)
			{
				ID_registers[0] = decode.getRegisters(IFID_instruction[0]);
				ID_immediate[0] = decode.getImmediate(IFID_instruction[0]);
				ID_command[0] = decode.decode(IFID_command[0]);

				ID_registers[1] = decode.getRegisters(IFID_instruction[1]);
				ID_immediate[1] = decode.getImmediate(IFID_instruction[1]);
				ID_command[1] = decode.decode(IFID_command[1]);
			}
			if (g_clock > 2)
			{
				execute(IDEX_command[0], registers, IDEX_registers[0], IDEX_immediate[0]);

				execute(IDEX_command[1], registers, IDEX_registers[1], IDEX_immediate[1]);
			}
		}
		else
		{
			if (!IF_instruction_tokens[0].empty() && !IF_instruction_tokens[1].empty())
			{
				IFID_command[0] = IF_instruction_tokens[0][0];
				IFID_instruction[0] = code[PC];

				IFID_command[1] = IF_instruction_tokens[1][0];
				IFID_instruction[1] = code[PC + 1];

				if (!branch_taken)
				{
					PC+=2;
				}
				else
				{
					branch_taken = !branch_taken;
				}
			}
			if (g_clock > 1)
			{
				IDEX_registers[0] = ID_registers[0];
				IDEX_immediate[0] = ID_immediate[0];
				IDEX_command[0] = ID_command[0];

                                IDEX_registers[1] = ID_registers[1];
				IDEX_immediate[1] = ID_immediate[1];
				IDEX_command[1] = ID_command[1];
			}
		}

		g_clock++;
	}
	return 0;
}
