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

#define N_WAY_SS 4

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

	string IFID_command[N_WAY_SS];
	int IDEX_immediate[N_WAY_SS];
	ISA IDEX_command[N_WAY_SS];
	vector<int> IDEX_registers[N_WAY_SS];
	string IFID_instruction_keyword[N_WAY_SS], IFID_instruction[N_WAY_SS];
	vector<string> IF_instruction_tokens[N_WAY_SS];
	vector<int> ID_registers[N_WAY_SS];
	int ID_immediate[N_WAY_SS];
	ISA ID_command[N_WAY_SS];

	for (;;)
	{
		if ((g_clock % 2) == 0)
		{
			for (int i = 0; i < N_WAY_SS; i++)
			{
				IF_instruction_tokens[i].clear();
			}
			if (PC < code.size())
			{
				for (int i = 0; i < N_WAY_SS; i++)
				{
					IF_instruction_tokens[i] = fetch.getInstruction(code[PC + i]);
				}
			}

			if (g_clock > 0)
			{
				for (int i = 0; i < N_WAY_SS; i++)
				{
					ID_registers[i] = decode.getRegisters(IFID_instruction[i]);
					ID_immediate[i] = decode.getImmediate(IFID_instruction[i]);
					ID_command[i] = decode.decode(IFID_command[i]);
				}
			}
			if (g_clock > 2)
			{
				for (int i = 0; i < N_WAY_SS; i++)
				{
					execute(IDEX_command[i], registers, IDEX_registers[i], IDEX_immediate[i]);
				}
			}
		}
		else
		{
			bool last = false;
			for (int i = 0; i < N_WAY_SS; i++)
			{
				if (IF_instruction_tokens[i].empty())
				{
					last = true;
				}
			}
			if (!last)
			{
				for (int i = 0; i < N_WAY_SS; i++)
				{
					IFID_command[i] = IF_instruction_tokens[i][0];
					IFID_instruction[i] = code[PC + i];
				}
				if (!branch_taken)
				{
					PC += N_WAY_SS;
				}
				else
				{
					branch_taken = !branch_taken;
					if (N_WAY_SS > 1)
					{
						for (int i = 0; i < N_WAY_SS; i++)
						{
							IFID_command[i] = NOP;
						}
					}
				}
			}

			if (g_clock > 1)
			{
				for (int i = 0; i < N_WAY_SS; i++)
				{
					IDEX_registers[i] = ID_registers[i];
					IDEX_immediate[i] = ID_immediate[i];
					IDEX_command[i] = ID_command[i];
				}
			}
		}

		g_clock++;
	}
	return 0;
}
