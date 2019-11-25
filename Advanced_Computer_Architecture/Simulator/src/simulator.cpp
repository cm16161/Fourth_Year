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
	ALU alu[N_WAY_SS];
	int registers[N_REGISTERS] = { 0 };
	bool registers_in_use[N_REGISTERS] = { false };
	int register_rename[N_REGISTERS];
	for (int i = 0; i < N_REGISTERS; i++)
	{
		register_rename[i] = i;
	}
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
	int durations[N_WAY_SS];

	for (;;)
	{
		for (int i = 0; i < N_WAY_SS; i++)
		{
			durations[i] = 0;
		}
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
				int targets[N_WAY_SS] = { 0 };
				int delay = 0;
				for (int i = 0; i < N_WAY_SS; i++)
				{
					targets[i] = IDEX_registers[i][0];
					durations[i] = execute(alu[i], IDEX_command[i], registers, IDEX_registers[i], IDEX_immediate[i]);
					for (int j = 1; j < IDEX_registers[i].size(); j++)
					{
						for (int k = i-1; k >=0; k--)
						{
							if (IDEX_registers[i][j] == targets[k])
							{
								delay += durations[i];
                                                                break;
							}
						}
					}
					if (IDEX_command[i] != NOP && IDEX_command[i] != BNE && IDEX_command[i] != BEQ &&
					    IDEX_command[i] != ST)
					{
						registers_in_use[IDEX_registers[i][0]] = false;
					}

					if (branch_taken && i != N_WAY_SS - 1)
					{
						for (int j = i + 1; j < N_WAY_SS; j++)
						{
							IDEX_command[j] = NOP;
						}
					}
				}
				int max = -1;
				for (int i = 0; i < N_WAY_SS; i++)
				{
					if (durations[i] >= max)
					{
						max = durations[i];
					}
				}
				if (delay < max)
				{
					g_clock += 2 * max;
				}
				else
				{
					g_clock += 2 * delay;
				}
			}
		}
		else
		{
			bool last = false;
			int remaining_commands = 0;
			for (int i = 0; i < N_WAY_SS; i++)
			{
				if (IF_instruction_tokens[i].empty())
				{
					last = true;
				}
				else
				{
					remaining_commands++;
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
			}
			else
			{
				for (int i = 0; i < remaining_commands; i++)
				{
					{
						IFID_command[i] = IF_instruction_tokens[i][0];
						IFID_instruction[i] = code[PC + i];
					}
				}
			}
			if (branch_taken)
			{

				branch_taken = !branch_taken;
				for (int i = 0; i < N_WAY_SS; i++)
				{

					IFID_command[i] = NOP;
					ID_command[i] = NOP;
				}
			}

			if (g_clock > 1)
			{

				for (int i = 0; i < N_WAY_SS; i++)
				{
					if (ID_command[i] != NOP && ID_command[i] != BNE && ID_command[i] != BEQ && ID_command[i] != ST)
					{
						if (!ID_registers[i].empty())
						{
							int target = ID_registers[i][0];
							if (registers_in_use[target])
							{
								for (int j = 0; j < N_REGISTERS; j++)
								{
									if (!registers_in_use[j])
									{
										registers_in_use[j] = true;
										register_rename[target] = j;
										ID_registers[i][0] = j;
									}
								}
							}
							else
							{
								registers_in_use[target] = true;
							}
						}
					}
					for (int j = 0; j < ID_registers[i].size(); j++)
					{
						ID_registers[i][j] = register_rename[ID_registers[i][j]];
					}
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
