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

class Register_Usage
{
public:
	bool in_use;
	int blocking_instruction;
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
	Reorder(int res, int num, int dest, ISA token)
	{
		result = res;
		instruction_number = num;
		rd = dest;
		m_token = token;
	}
	int result;
	int instruction_number;
	int rd;
	ISA m_token;
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
	vector<int> m_values;
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
	int issue_entries = 0;
	int dispatch_entries = 0;
	string file_name;
	executed_instructions = 0;
	if (argc > 1)
	{
		file_name = argv[1];
	}
	else
	{
		file_name = "../apps/test.txt";
	}
	PC = 0;
	Fetch &fetch = Fetch::getInstance();
	Decode &decode = Decode::getInstance();
	ALU alu[N_WAY_SS];
	int registers[N_REGISTERS] = { 0 };
	Register_Usage registers_in_use[N_REGISTERS];
	int register_rename[N_REGISTERS];
	for (int i = 0; i < N_REGISTERS; i++)
	{
		register_rename[i] = i;
		registers_in_use[i].in_use = false;
		registers_in_use[i].blocking_instruction = -1;
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

	vector<Reorder *> reorder_buffer;
	vector<Dispatch *> reservation_station;
	vector<Issue *> issue_station;

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
				if (reorder_buffer[i]->instruction_number == next_to_commit)
				{
					if (reorder_buffer[i]->m_token != BEQ)
					{
						registers[reorder_buffer[i]->rd] = reorder_buffer[i]->result;
						//cout << "freeing: " << reorder_buffer[i]->rd << endl;
						registers_in_use[reorder_buffer[i]->rd].in_use = false;
						//register_rename[reorder_buffer[i]->rd] = reorder_buffer[i]->rd;
						reorder_buffer.erase(reorder_buffer.begin() + i);
						next_to_commit++;
						break;
					}
					else
					{
						reorder_buffer.erase(reorder_buffer.begin() + i);
						next_to_commit++;
						break;
					}
				}
			}
		} // End of Commit Stage

		if (g_clock > 3) // Execute Stage
		{
			for (int i = 0; i < N_WAY_SS; i++)
			{
				results[i] = execute(alu[i], IDEX_command[i].token, registers, IDEX_registers[i], IDEX_immediate[i]);
				if (IDEX_command[i].token != JI && IDEX_command[i].token != JR && // IDEX_command[i].token != BEQ &&
				    IDEX_command[i].token != BNE && IDEX_command[i].token != LDI)
				{
					if (!alu[i].m_lock)
					{
						if (IDEX_command[i].token != NOP)
						{
							Reorder *tmp = new Reorder(results[i], IDEX_command[i].instruction_number,
							                           IDEX_registers[i][0], IDEX_command[i].token);
							reorder_buffer.push_back(tmp);
						}
					}
				}
				else
				{
					if (branch_taken)
					{
                                          //cout << "BRANCH TAKEN!\n";
						for (int i = 0; i < reservation_station.size(); i++)
						{
							reservation_station[i]->m_token = NOP;
							if (!reservation_station[i]->m_registers.empty())
							{
								registers_in_use[reservation_station[i]->m_registers[0]].in_use = false;
							}
						}
						for (int i = 0; i < issue_station.size(); i++)
						{
							issue_station[i]->m_token = NOP;
							if (!issue_station[i]->m_registers.empty())
							{
								registers_in_use[issue_station[i]->m_registers[0]].in_use = false;
							}
						}
						for (int i = 0; i < N_WAY_SS; i++)
						{
							ID_command[i].token = NOP;
							if (!ID_registers[i].empty())
							{
								registers_in_use[ID_registers[i][0]].in_use = false;
							}
							IFID_command[i].instruction = "NOP";
						}
					}
					else
					{
						next_to_commit++;
						//cout << "branch not taken, next to commit = " << next_to_commit << endl;
					}
				}
			}
		} // End of Execute Stage

		if (g_clock > 2) // Dispatch Stage
		{
			vector<int> index_to_remove;
			vector<int> remove_for_eop;
			for (int i = 0; i < issue_station.size();
			     i++) // Make sure that EOP can execute even if there are NOP's from blank execution
			{
				if (issue_station[i]->m_token == EOP)
				{
					for (int j = 0; j < issue_station.size(); j++)
					{
						if (issue_station[j]->m_token == EOP || issue_station[j]->m_token == NOP)
						{
							if (i != j)
							{
								if (i == j)
								{
									exit(-1);
								}
								remove_for_eop.push_back(j);
							}
						}
					}
				}
			}

			for (int i = 0; i < remove_for_eop.size(); i++)
			{
				issue_station.erase(issue_station.begin() + remove_for_eop[i] - i); // remove excess instructions
			}

			for (int i = 0; i < issue_station.size(); i++)
			{
				if (issue_station[i]->m_token == EOP)
				{
					if (issue_station.size() != 1)
					{

						continue;
					}
				}
				if (issue_station[i]->m_dependency == false)
				{
					Dispatch *tmp = new Dispatch(issue_station[i]->m_registers, issue_station[i]->m_immediate,
					                             issue_station[i]->m_token, issue_station[i]->m_instruction_number);
					reservation_station.push_back(tmp);
					dispatch_entries++;
					//issue_station.erase(issue_station.begin() + i);
					index_to_remove.push_back(i);
				}
			}

			for (int i = 0; i < index_to_remove.size(); i++)
			{
				issue_station.erase(issue_station.begin() + index_to_remove[i] - i);
			}

			for (int i = 0; i < N_WAY_SS; i++)
			{
				if (!alu[i].m_lock)
				{
					if (reservation_station.size() > 0)
					{
						IDEX_registers[i] = reservation_station[0]->m_registers;
						IDEX_immediate[i] = reservation_station[0]->m_immediate;
						IDEX_command[i].token = reservation_station[0]->m_token;
						IDEX_command[i].instruction_number = reservation_station[0]->m_instruction_number;
						// for (auto j : IDEX_registers[i])
						// {
						// 	cout << "register " << j << " has value: " << registers[j] << endl;
						// }
						// cout << "for instruction: " << IDEX_command[i].instruction_number
						//      << " token: " << IDEX_command[i].token << endl;
						reservation_station.erase(reservation_station.begin());
					}
					else
					{
						IDEX_command[i].token = NOP;
					}
				}
			}
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
							if (registers_in_use[ID_registers[i][j]].in_use && ID_registers[i][j] != ID_registers[i][0])
							{
								dependency_not_met = true;
							}
						}
						Issue *tmp = new Issue(ID_registers[i], ID_immediate[i], ID_command[i].token,
						                       ID_command[i].instruction_number, dependency_not_met);
						issue_station.push_back(tmp);
						issue_entries++;
					}
					else if (ID_command[i].token == EOP)
					{
						Issue *tmp = new Issue(ID_registers[i], ID_immediate[i], ID_command[i].token,
						                       ID_command[i].instruction_number, false);
						issue_station.push_back(tmp);
						issue_entries++;
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
								if (registers_in_use[ID_registers[i][j]].in_use &&
								    ID_registers[i][j] != ID_registers[i][0])
								{
									dependency_not_met = true;
								}
							}
							Issue *tmp = new Issue(ID_registers[i], ID_immediate[i], ID_command[i].token,
							                       ID_command[i].instruction_number, dependency_not_met);
							issue_station.push_back(tmp);
							issue_entries++;
						}
						else if (ID_command[i].token == EOP)
						{
							Issue *tmp = new Issue(ID_registers[i], ID_immediate[i], ID_command[i].token,
							                       ID_command[i].instruction_number, true);
							issue_station.push_back(tmp);
							issue_entries++;
						}
					}
				}
				else
				{
					//cout << "skipping issue\n";
				}
			}
			for (int i = 0; i < issue_station.size(); i++)
			{
				bool dependency_not_met = false;
				for (int j = 1; j < issue_station[i]->m_registers.size(); j++)
				{

					if (issue_station[i]->m_token != NOP && issue_station[i]->m_token != EOP &&
					    issue_station[i]->m_token != BEQ)
					{
						if (registers_in_use[issue_station[i]->m_registers[j]].in_use &&
						    issue_station[i]->m_registers[j] != issue_station[i]->m_registers[0])
						{
							dependency_not_met = true;
						}
					}
				}
				if (issue_station[i]->m_token == BEQ)
				{
					if (registers_in_use[issue_station[i]->m_registers[0]].in_use ||
					    registers_in_use[issue_station[i]->m_registers[1]].in_use)
					{
						if (registers_in_use[issue_station[i]->m_registers[0]].blocking_instruction <
						        issue_station[i]->m_instruction_number &&
						    registers_in_use[issue_station[i]->m_registers[1]].blocking_instruction <
						        issue_station[i]->m_instruction_number)
						{
							dependency_not_met = true;
						}
					}
				}
				issue_station[i]->m_dependency = dependency_not_met;
			}

		} // End of Issue Stage

		if (g_clock > 0) // Decode Stage
		{
			if (!fetch_stop)
			{
				for (int i = 0; i < N_WAY_SS; i++) // Normal Execution
				{

					ID_registers[i] = decode.getRegisters(IFID_instruction[i]); // Get the registers to be used
					ID_command[i].token = decode.decode(IFID_command[i].instruction);
					ID_command[i].instruction_number = IFID_command[i].instruction_number;

					if (!ID_registers[i].empty())
					{
						if (ID_command[i].token == BEQ)
						{
							for (int j = 0; j < ID_registers[i].size(); j++)
							{
								ID_registers[i][j] = register_rename[ID_registers[i][j]];
							}
						}
					}

					if (ID_command[i].token != NOP && ID_command[i].token != BNE && ID_command[i].token != BEQ &&
					    ID_command[i].token != ST)
					{
						if (!ID_registers[i].empty())
						{
							int target = ID_registers[i][0];
							if (registers_in_use[target].in_use) // See if register is in use
							{
								for (int j = N_REGISTERS / 2; j < N_REGISTERS; j++)
								{
									if (!registers_in_use[j].in_use)
									{
										registers_in_use[j].in_use = true;
										registers_in_use[j].blocking_instruction = ID_command[i].instruction_number;
										register_rename[target] = j;
										ID_registers[i][0] = j;
										break;
									}
								}
							}
							else
							{
								registers_in_use[target].in_use = true;
								registers_in_use[target].blocking_instruction = ID_command[i].instruction_number;
								register_rename[target] = target;
							}

							ID_registers[i][0] = register_rename[ID_registers[i][0]];
						}
					}
					// for (int j = 0; j < ID_registers[i].size(); j++) // Rename the registers
					// {
					// 	ID_registers[i][j] = register_rename[ID_registers[i][j]];
					// }

					ID_immediate[i] = decode.getImmediate(IFID_instruction[i]);
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
				else
				{
					//cout << "skipping decode\n";
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
				if (!branch_taken)
				{
					PC += remaining_commands;
				}
				else
				{
					last = false;
					fetch_stop = false;
					decode_stop = false;
					issue_stop = false;
				}
			}
			else
			{
				//cout << "skipping fetch\n";
			}
		}

		// End of Fetch Stage

		g_clock++; // Increment Clock by 1 Cycle
	} // Bottom of Infinite Loop
	return 0;
}
