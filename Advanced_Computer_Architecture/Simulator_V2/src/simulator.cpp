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

#define N_WAY_SS 1

class Instruction_Order
{
public:
	string instruction;
	int instruction_number;
};

class Token_Order
{
public:
	ISA token;
	int instruction_number;
};

class Reorder
{
public:
	Reorder(int res, int num, int dest)
	{
		result = res;
		instruction_number = num;
		rd = dest;
	}
	int result;
	int instruction_number;
	int rd;
};

class Dispatch
{
public:
	Dispatch(vector<int> registers, int immediate, ISA token, int instruction_number)
	{
		m_registers = registers;
		m_immediate = immediate;
		m_token = token;
		m_instruction_number = instruction_number;
	}
	vector<int> m_registers;
	int m_immediate, m_instruction_number;
	ISA m_token;
};

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

	Instruction_Order IFID_command[N_WAY_SS];
	int IDEX_immediate[N_WAY_SS];
	Token_Order IDEX_command[N_WAY_SS];
	vector<int> IDEX_registers[N_WAY_SS];
	string IFID_instruction_keyword[N_WAY_SS];
	string IFID_instruction[N_WAY_SS];
	vector<string> IF_instruction_tokens[N_WAY_SS];
	vector<int> ID_registers[N_WAY_SS];
	int ID_immediate[N_WAY_SS];
	Token_Order ID_command[N_WAY_SS];
	int results[N_WAY_SS];
	int instruction_number = 0;
	int next_to_commit = 0;
	vector<Reorder> reorder_buffer;
	vector<Dispatch> reservation_station;

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
					ID_command[i].token = decode.decode(IFID_command[i].instruction);
					ID_command[i].instruction_number = IFID_command[i].instruction_number;
				}
			}
			if (g_clock > 2)
			{
				int targets[N_WAY_SS] = { 0 };
				int delay = 0;
				for (int i = 0; i < N_WAY_SS; i++)
				{

					//targets[i] = IDEX_registers[i][0];

					bool can_execute_last_instruction = true;
					if (IDEX_command[i].token == EOP)
					{
						for (int a = 0; a < N_WAY_SS; a++)
						{
							if (alu[a].m_lock)
							{
								can_execute_last_instruction = false;
							}
						}
					}

					if (can_execute_last_instruction)
					{
						results[i] =
						    execute(alu[i], IDEX_command[i].token, registers, IDEX_registers[i], IDEX_immediate[i]);
					}
					if (IDEX_command[i].token != JI && IDEX_command[i].token != JR && IDEX_command[i].token != BEQ &&
					    IDEX_command[i].token != BNE && IDEX_command[i].token != LDI)
					{
						if (!alu[i].m_lock)
						{
							Reorder tmp(results[i], IDEX_command[i].instruction_number, IDEX_registers[i][0]);
							reorder_buffer.push_back(tmp);
						}
					}

					if (branch_taken && i != N_WAY_SS - 1)
					{
						for (int j = i + 1; j < N_WAY_SS; j++)
						{
							IDEX_command[j].token = NOP;
						}
					}
				}
			}
			if (g_clock > 3)
			{
				for (int i = 0; i < reorder_buffer.size(); i++)
				{
					if (reorder_buffer[i].instruction_number == next_to_commit)
					{
						registers[reorder_buffer[i].rd] = reorder_buffer[i].result;
						registers_in_use[reorder_buffer[i].rd] = false;
						reorder_buffer.erase(reorder_buffer.begin() + i);

						next_to_commit++;
					}
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
					IFID_command[i].instruction = IF_instruction_tokens[i][0];
					IFID_command[i].instruction_number = instruction_number;
					IFID_instruction[i] = code[PC + i];
					instruction_number++;
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
						IFID_command[i].instruction = IF_instruction_tokens[i][0];
						IFID_command[i].instruction_number = instruction_number;
						IFID_instruction[i] = code[PC + i];
						instruction_number++;
					}
				}
			}
			if (branch_taken)
			{

				branch_taken = !branch_taken;
				if (N_WAY_SS > 1)
				{
					for (int i = 0; i < N_WAY_SS; i++)
					{

						//IFID_command[i] = NOP;
						ID_command[i].token = NOP;
					}
				}
			}

			if (g_clock > 1)
			{

				for (int i = 0; i < N_WAY_SS; i++)
				{
					if (ID_command[i].token != NOP && ID_command[i].token != BNE && ID_command[i].token != BEQ &&
					    ID_command[i].token != ST)
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
					if (g_clock > 2)
					{
						Dispatch tmp(ID_registers[i], ID_immediate[i], ID_command[i].token,
						             ID_command[i].instruction_number);
						reservation_station.push_back(tmp);
					}
					{
						if (alu[i].m_lock == false)
						{
							IDEX_registers[i] = reservation_station[0].m_registers;
							IDEX_immediate[i] = reservation_station[0].m_immediate;
							IDEX_command[i].token = reservation_station[0].m_token;
							IDEX_command[i].instruction_number = reservation_station[0].m_instruction_number;
							reservation_station.erase(reservation_station.begin());
						}
					}

					// for (int a = 0; a < N_WAY_SS; a++)
					// {
					// 	if (alu[a].m_lock == false)
					// 	{
					// 		IDEX_registers[a] = reservation_station[0].m_registers;
					// 		IDEX_immediate[a] = reservation_station[0].m_immediate;
					// 		IDEX_command[a].token = reservation_station[0].m_token;
					// 		IDEX_command[a].instruction_number = reservation_station[0].m_instruction_number;
					// 		reservation_station.erase(reservation_station.begin());
					// 		break;
					// 	}
					// }

					// IDEX_registers[i] = ID_registers[i];
					// IDEX_immediate[i] = ID_immediate[i];
					// IDEX_command[i].token = ID_command[i].token;
					// IDEX_command[i].instruction_number = ID_command[i].instruction_number;
				}
			}
		}

		g_clock++;
	}
	return 0;
}
