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

class Issue
{
public:
	Issue(vector<int> registers, int immediate, ISA token, int inst_num, bool dependency)
	{
		m_registers = registers;
		m_immediate = immediate;
		m_token = token;
		m_instruction_number = inst_num;
		m_dependency = dependency;
	}
	vector<int> m_registers;
	int m_immediate;
	ISA m_token;
	int m_instruction_number;
	bool m_dependency;
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
	vector<Issue> issue_station;

	bool last = false;
	bool fetch_stop = false;
	bool decode_stop = false;
	bool issue_stop = false;
	int final_last_instructions = 0;
	for (;;)
	{
		if (g_clock > 4) // Commit Stage
		{
			for (int i = 0; i < reorder_buffer.size(); i++)
			{
				if (reorder_buffer[i].instruction_number == next_to_commit)
				{
					registers[reorder_buffer[i].rd] = reorder_buffer[i].result;
					registers_in_use[reorder_buffer[i].rd] = false;
					;
					reorder_buffer.erase(reorder_buffer.begin() + i);

					next_to_commit++;
				}
			}
		} // End of Commit Stage

		if (g_clock > 3) // Execute Stage
		{
			for (int i = 0; i < N_WAY_SS; i++)
			{
				results[i] = execute(alu[i], IDEX_command[i].token, registers, IDEX_registers[i], IDEX_immediate[i]);
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
		} // End of Execute Stage

		if (g_clock > 2) // Dispatch Stage
		{
			for (int i = 0; i < issue_station.size(); i++)
			{
				if (issue_station[i].m_token == EOP)
				{
					if (issue_station.size() != 1)
					{
						continue;
					}
				}
				if (issue_station[i].m_dependency == false)
				{
					Dispatch tmp(issue_station[i].m_registers, issue_station[i].m_immediate, issue_station[i].m_token,
					             issue_station[i].m_instruction_number);
					reservation_station.push_back(tmp);
					issue_station.erase(issue_station.begin() + i);
				}
			}
			for (int i = 0; i < N_WAY_SS; i++)
			{
				if (!alu[i].m_lock)
				{
					if (reservation_station.size() > 0)
					{
						IDEX_registers[i] = reservation_station[0].m_registers;
						IDEX_immediate[i] = reservation_station[0].m_immediate;
						IDEX_command[i].token = reservation_station[0].m_token;
						IDEX_command[i].instruction_number = reservation_station[0].m_instruction_number;
						reservation_station.erase(reservation_station.begin());
					}
				}
			}
			// for (int i = 0; i < N_WAY_SS; i++)
			// {
			// 	Dispatch tmp(ID_registers[i], ID_immediate[i], ID_command[i].token, ID_command[i].instruction_number);
			// 	reservation_station.push_back(tmp); //Allow instruction to be dispatched to execute
			// 	if (!alu[i].m_lock)
			// 	{
			// 		if (reservation_station.size() > 0)
			// 		{
			// 			IDEX_registers[i] = reservation_station[0].m_registers;
			// 			IDEX_immediate[i] = reservation_station[0].m_immediate;
			// 			IDEX_command[i].token = reservation_station[0].m_token;
			// 			IDEX_command[i].instruction_number = reservation_station[0].m_instruction_number;
			// 			reservation_station.erase(reservation_station.begin());
			// 		}
			// 	}
			// }
		} // End of Dispatch Stage

		if (g_clock > 1) // Issue Stage - Wait for Registers
		{
			if (!decode_stop) // Normal Execution
			{
				for (int i = 0; i < N_WAY_SS; i++)
				{
					if (!ID_registers[i].empty())
					{
						bool dependency_not_met = false;
						for (int j = 1; j < ID_registers[i].size(); j++)
						{
							if (registers_in_use[ID_registers[i][j]] && ID_registers[i][j] != ID_registers[i][0])
							{
								dependency_not_met = true;
							}
						}
						Issue tmp(ID_registers[i], ID_immediate[i], ID_command[i].token,
						          ID_command[i].instruction_number, dependency_not_met);
						issue_station.push_back(tmp);
					}
					else if (ID_command[i].token == EOP)
					{
						Issue tmp(ID_registers[i], ID_immediate[i], ID_command[i].token,
						          ID_command[i].instruction_number, false);
						issue_station.push_back(tmp);
					}
				}
			}
			else
			{
				if (!issue_stop)
				{
					issue_stop = true;
					for (int i = 0; i < final_last_instructions; i++)
					{
						if (!ID_registers[i].empty())
						{
							bool dependency_not_met = false;
							for (int j = 1; j < ID_registers[i].size(); j++)
							{
								if (registers_in_use[ID_registers[i][j]] && ID_registers[i][j] != ID_registers[i][0])
								{
									dependency_not_met = true;
								}
							}
							Issue tmp(ID_registers[i], ID_immediate[i], ID_command[i].token,
							          ID_command[i].instruction_number, dependency_not_met);
							issue_station.push_back(tmp);
						}
						else if (ID_command[i].token == EOP)
						{
							Issue tmp(ID_registers[i], ID_immediate[i], ID_command[i].token,
							          ID_command[i].instruction_number, true);
							issue_station.push_back(tmp);
						}
					}
				}
			}
			for (int i = 0; i < issue_station.size(); i++)
			{
				bool dependency_not_met = false;
				for (int j = 1; j < issue_station[i].m_registers.size(); j++)
				{
					if (registers_in_use[issue_station[i].m_registers[j]] &&
					    issue_station[i].m_registers[j] != issue_station[i].m_registers[0])
					{
						dependency_not_met = true;
					}
				}
				issue_station[i].m_dependency = dependency_not_met;
			}
			// for (auto i : issue_station)
			// {
			// 	bool dependency_not_met = false;
			// 	for (int j = 1; j < i.m_registers.size(); j++)
			// 	{
			// 		if (registers_in_use[i.m_registers[j]] && i.m_registers[j] != i.m_registers[0])
			// 		{
			// 			cout << i.m_token << " bloked on " << i.m_registers[j] << endl;
			// 			dependency_not_met = true;
			// 		}
			// 	}
			// 	i.m_dependency = dependency_not_met;
			// 	cout << "token is: " << i.m_token;
			// 	cout << "setting dependency to: " << dependency_not_met << endl;
			// 	cout << "new dependency is: " << i.m_dependency << endl;
			// }
			// for (auto i : issue_station)
			// {
			// 	cout << "end of issue " << i.m_dependency << endl;
			// }
		} // End of Issue Stage

		if (g_clock > 0) // Decode Stage
		{
			if (!fetch_stop)
			{
				for (int i = 0; i < N_WAY_SS; i++) // Normal Execution
				{

					ID_registers[i] = decode.getRegisters(IFID_instruction[i]);
					if (ID_command[i].token != NOP && ID_command[i].token != BNE && ID_command[i].token != BEQ &&
					    ID_command[i].token != ST)
					{
						if (!ID_registers[i].empty())
						{
							int target = ID_registers[i][0];
							if (registers_in_use[target]) // See if register is in use
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
					for (int j = 0; j < ID_registers[i].size(); j++) // Rename the registers
					{
						ID_registers[i][j] = register_rename[ID_registers[i][j]];
					}

					ID_immediate[i] = decode.getImmediate(IFID_instruction[i]);
					ID_command[i].token = decode.decode(IFID_command[i].instruction);
					ID_command[i].instruction_number = IFID_command[i].instruction_number;
				}
			}
			else
			{
				if (!decode_stop)
				{
					decode_stop = true;
					for (int i = 0; i < final_last_instructions; i++) // Final Set of Instructions
					{
						ID_registers[i] = decode.getRegisters(IFID_instruction[i]);
						ID_immediate[i] = decode.getImmediate(IFID_instruction[i]);
						ID_command[i].token = decode.decode(IFID_command[i].instruction);
						ID_command[i].instruction_number = IFID_command[i].instruction_number;
					}
				}
			}
		} // End of Decode Stage

		// Fetch Stage
		for (int i = 0; i < N_WAY_SS; i++) // Clear all current tokens in buffers
		{
			IF_instruction_tokens[i].clear();
		}
		if (PC < code.size()) // Ensure there is still instructions to get
		{
			for (int i = 0; i < N_WAY_SS; i++) // Get the instructions
			{
				IF_instruction_tokens[i] = fetch.getInstruction(code[PC + i]);
			}
		}

		int remaining_commands = 0;
		for (int i = 0; i < N_WAY_SS; i++)
		{
			if (IF_instruction_tokens[i].empty()) // See if any of the instructions are empty
			{
				last = true;
			}
			else
			{
				remaining_commands++;
			}
		}
		if (!last) // Normal execution
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
		else // Remaining N Instructions
		{
			if (!fetch_stop)
			{
				fetch_stop = true;
				for (int i = 0; i < remaining_commands; i++)
				{
					IFID_command[i].instruction = IF_instruction_tokens[i][0];
					IFID_command[i].instruction_number = instruction_number;
					IFID_instruction[i] = code[PC + i];
					instruction_number++;
					final_last_instructions = remaining_commands;
				}
				PC += remaining_commands;
			}
		}

		// End of Fetch Stage

		g_clock++; // Increment Clock by 1 Cycle
	} // Bottom of Infinite Loop
	return 0;
}
