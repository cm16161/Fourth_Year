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

#define N_WAY_SS 2

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
	Issue(vector<int> registers, int immediate, ISA token, int inst_num, bool dependency, vector<int> branch_waits)
	{
		m_registers = registers;
		m_immediate = immediate;
		m_token = token;
		m_instruction_number = inst_num;
		m_dependency = dependency;
		m_branch_waits = branch_waits;
	}
	vector<int> m_registers;
	int m_immediate;
	ISA m_token;
	int m_instruction_number;
	bool m_dependency;
	vector<int> m_branch_waits;
};

class Reorder
{
public:
	Reorder(int res, int num, int dest, ISA token, vector<int> branch_waits, bool mem_flag)
	{
		if (token != LD && token != ST && token != LDR)
		{
			result = res;
			instruction_number = num;
			rd = dest;
			m_token = token;
			m_branch_waits = branch_waits;
			m_mem_flag = mem_flag;
		}
		else if (token == LDR)
		{
			m_rs = res;
			instruction_number = num;
			rd = dest;
			m_token = token;
			m_branch_waits = branch_waits;
			m_mem_flag = mem_flag;
		}
		else
		{
			m_rs = dest;
			m_immidiate = res;
			instruction_number = num;
			m_token = token;
			m_branch_waits = branch_waits;
			m_mem_flag = mem_flag;
		}
	}
	// Reorder(int immidiate, int num, int rs, ISA token, vector<int> branch_waits, bool mem_flag)
	// {
	// 	m_rs = rs;
	// 	m_immidiate = immidiate;
	// 	instruction_number = num;
	// 	m_token = token;
	// 	m_branch_waits = branch_waits;
	// 	m_mem_flag = mem_flag;
	// }
	int result;
	int instruction_number;
	int rd;
	ISA m_token;
	vector<int> m_branch_waits;
	int m_rs = 0;
	int m_immidiate = 0;
	bool m_mem_flag;
};

class Dispatch
{
public:
	Dispatch(vector<int> registers, int immediate, ISA token, int instruction_number, vector<int> branch_waits)
	{
		m_registers = registers;
		m_immediate = immediate;
		m_token = token;
		m_instruction_number = instruction_number;
		m_branch_waits = branch_waits;
	}
	vector<int> m_registers;
	vector<int> m_values;
	int m_immediate, m_instruction_number;
	ISA m_token;
	vector<int> m_branch_waits;
};

//#define N_REGISTERS 256
int PC;
int executed_instructions;
int g_clock;
extern bool branch_taken;

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
	static MEM &mem = MEM::getInstance();
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

	int IDEX_immediate[N_WAY_SS];
	Token_Order IDEX_command[N_WAY_SS];
	vector<int> IDEX_registers[N_WAY_SS];
	vector<int> IDEX_branch_waits[N_WAY_SS];

	Instruction_Order IFID_command[N_WAY_SS];
	string IFID_instruction_keyword[N_WAY_SS];
	string IFID_instruction[N_WAY_SS];
	vector<string> IF_instruction_tokens[N_WAY_SS];

	vector<int> ID_registers[N_WAY_SS];
	int ID_immediate[N_WAY_SS];
	Token_Order ID_command[N_WAY_SS];
	vector<int> ID_branch_waits[N_WAY_SS];

	int results[N_WAY_SS];

	int instruction_number = 0;
	int next_to_commit = 0;

	vector<Reorder *> reorder_buffer;
	vector<Dispatch *> reservation_station;
	vector<Issue *> issue_station;

	bool depends_on_branch = false;
	vector<int> dependent_branch;

	bool last = false;
	bool fetch_stop = false;
	bool decode_stop = false;
	bool issue_stop = false;
	int final_last_instructions = 0;
	for (;;)
	{
		if (g_clock > 4) // Commit Stage
		{
			//cout << " Next to commit " << next_to_commit << endl;
			vector<int> entries_to_free;
			for (int i = 0; i < reorder_buffer.size(); i++)
			{
				// cout << reorder_buffer[i]->instruction_number << endl;
				// cout << next_to_commit << endl;
				if (reorder_buffer[i]->instruction_number == next_to_commit)
				{

					if (reorder_buffer[i]->m_token != BEQ && reorder_buffer[i]->m_token != BNE &&
					    reorder_buffer[i]->m_token != ST && reorder_buffer[i]->m_token != LD &&
					    reorder_buffer[i]->m_token != LDR && reorder_buffer[i]->m_token != EOP)
					{
						entries_to_free.push_back(i);
						registers[reorder_buffer[i]->rd] = reorder_buffer[i]->result;
						//cout << "freeing " << reorder_buffer[i]->rd << endl;
						registers_in_use[reorder_buffer[i]->rd].in_use = false;
						//reorder_buffer.erase(reorder_buffer.begin() + i);
						next_to_commit++;
						//break;
					}
					else if (reorder_buffer[i]->m_token == LD)
					{
						if (!mem.m_lock)
						{
							mem.m_lock = true;
							mem.m_delay = 5;
						}
						mem.m_delay--;
						if (mem.m_delay <= 0)
						{
							entries_to_free.push_back(i);
							mem.m_lock = false;
							mem.ld(&registers[reorder_buffer[i]->m_rs], reorder_buffer[i]->m_immidiate);
							registers_in_use[reorder_buffer[i]->m_rs].in_use = false;
							//reorder_buffer.erase(reorder_buffer.begin() + i);
							next_to_commit++;
						}

						//break;
					}
					else if (reorder_buffer[i]->m_token == LDR)
					{
						if (!mem.m_lock)
						{
							mem.m_lock = true;
							mem.m_delay = 5;
						}
						mem.m_delay--;
						if (mem.m_delay <= 0)
						{
							entries_to_free.push_back(i);
							mem.m_lock = false;
							mem.ldr(&registers[reorder_buffer[i]->rd], &registers[reorder_buffer[i]->m_rs]);
							registers_in_use[reorder_buffer[i]->rd].in_use = false;
							//reorder_buffer.erase(reorder_buffer.begin() + i);
							next_to_commit++;
						}

						//break;
					}
					else if (reorder_buffer[i]->m_token == ST)
					{
						//mem.m_delay = 0;
						if (!mem.m_lock)
						{
							mem.m_lock = true;
							mem.m_delay = 5;
						}
						mem.m_delay--;
						if (mem.m_delay <= 0)
						{
							entries_to_free.push_back(i);
							mem.m_lock = false;
							mem.st(&registers[reorder_buffer[i]->m_rs], reorder_buffer[i]->m_immidiate);
							registers_in_use[reorder_buffer[i]->m_rs].in_use = false;
							//reorder_buffer.erase(reorder_buffer.begin() + i);
							next_to_commit++;
						}
						//break;
					}
					else if (reorder_buffer[i]->m_token == EOP)
					{
						end();
						//break;
					}
					else
					{
						//reorder_buffer.erase(reorder_buffer.begin() + i);
						next_to_commit++;
						//break;
					}
				}
			}
			for (int i = 0; i < entries_to_free.size(); i++)
			{
				reorder_buffer.erase(reorder_buffer.begin() + entries_to_free[i] - i);
			}
		} // End of Commit Stage

		if (g_clock > 3) // Execute Stage
		{

			for (int i = 0; i < N_WAY_SS; i++)
			{
				if (IDEX_command[i].token != LD && IDEX_command[i].token != ST && IDEX_command[i].token != LDR &&
				    IDEX_command[i].token != EOP)
				{
					results[i] =
					    execute(alu[i], IDEX_command[i].token, registers, IDEX_registers[i], IDEX_immediate[i]);
				}
				else if (IDEX_command[i].token == LDR)
				{
					Reorder *tmp = new Reorder(IDEX_registers[i][1], IDEX_command[i].instruction_number,
					                           IDEX_registers[i][0], IDEX_command[i].token, IDEX_branch_waits[i], true);
					reorder_buffer.push_back(tmp);
				}
				else
				{
					Reorder *tmp = new Reorder(IDEX_immediate[i], IDEX_command[i].instruction_number,
					                           IDEX_registers[i][0], IDEX_command[i].token, IDEX_branch_waits[i], true);
					reorder_buffer.push_back(tmp);
				}
				if (!alu[i].m_lock)
				{
					if (IDEX_command[i].token != NOP && IDEX_command[i].token != LD && IDEX_command[i].token != ST &&
					    IDEX_command[i].token != BEQ)
					{
						Reorder *tmp = new Reorder(results[i], IDEX_command[i].instruction_number, IDEX_registers[i][0],
						                           IDEX_command[i].token, IDEX_branch_waits[i], false);
						reorder_buffer.push_back(tmp);
					}
				}
				{
					if (branch_taken)
					{
						branch_taken = false;
						for (int j = 0; j < reservation_station.size(); j++)
						{
							reservation_station[j]->m_token = NOP;
							if (!reservation_station[j]->m_registers.empty())
							{
								registers_in_use[reservation_station[j]->m_registers[0]].in_use = false;
							}
						}
						for (int j = 0; j < issue_station.size(); j++)
						{
							issue_station[j]->m_token = NOP;
							if (!issue_station[j]->m_registers.empty())
							{
								registers_in_use[issue_station[j]->m_registers[0]].in_use = false;
							}
						}
						for (int j = 0; j < N_WAY_SS; j++)
						{
							ID_command[j].token = NOP;
							if (!ID_registers[j].empty())
							{
								registers_in_use[ID_registers[j][0]].in_use = false;
							}
							IFID_command[j].instruction = "NOP";
						}
						// for (int j = 0; j < N_WAY_SS; j++)
						// {
						// 	IDEX_command[j].token = NOP;
						// }

						vector<int> index_to_remove;
						for (int j = 0; j < reorder_buffer.size(); j++)
						{
							for (auto k : reorder_buffer[j]->m_branch_waits)
							{
								if (k == IDEX_command[i].instruction_number)
								{
									index_to_remove.push_back(j);
									continue;
								}
							}
						}
						for (int j = 0; j < index_to_remove.size(); j++)
						{
							reorder_buffer.erase(reorder_buffer.begin() + index_to_remove[j] - j);
						}
						dependent_branch.clear();

						for (int i = 0; i < N_WAY_SS; i++)
						{ // Release All ALUs on branch mispredict
							alu[i].m_lock = false;
						}
						next_to_commit = instruction_number;
						fetch_stop = false;
						decode_stop = false;
						issue_stop = false;
						last = false;
					}
					else
					{
						if (!dependent_branch.empty())
						{
							if (IDEX_command[i].instruction_number == dependent_branch[0])
							{
								next_to_commit++;
								dependent_branch.erase(dependent_branch.begin());
								for (auto p : reorder_buffer)
								{
									vector<int> tmp;
									for (int q = 0; q < p->m_branch_waits.size(); q++)
									{
										if (p->m_branch_waits[q] == IDEX_command[i].instruction_number)
										{
											tmp.push_back(q);
										}
									}
									for (int q = 0; q < tmp.size(); q++)
									{
										p->m_branch_waits.erase(p->m_branch_waits.begin() + tmp[q] - q);
									}
								}
								for (auto p : issue_station)
								{
									vector<int> tmp;
									for (int q = 0; q < p->m_branch_waits.size(); q++)
									{
										if (p->m_branch_waits[q] == IDEX_command[i].instruction_number)
										{
											tmp.push_back(q);
										}
									}
									for (int q = 0; q < tmp.size(); q++)
									{
										p->m_branch_waits.erase(p->m_branch_waits.begin() + tmp[q] - q);
									}
								}
								for (auto p : reservation_station)
								{
									vector<int> tmp;
									for (int q = 0; q < p->m_branch_waits.size(); q++)
									{
										if (p->m_branch_waits[q] == IDEX_command[i].instruction_number)
										{
											tmp.push_back(q);
										}
									}
									for (int q = 0; q < tmp.size(); q++)
									{
										p->m_branch_waits.erase(p->m_branch_waits.begin() + tmp[q] - q);
									}
								}
							}
						}
					}
				}
			}
		} // End of Execute Stage

		if (g_clock > 2) // Dispatch Stage
		{
			vector<int> index_to_remove;
			vector<int> remove_for_eop;

			for (int i = 0; i < issue_station.size(); i++)
			{
				if (issue_station[i]->m_dependency == false)
				{
					Dispatch *tmp = new Dispatch(issue_station[i]->m_registers, issue_station[i]->m_immediate,
					                             issue_station[i]->m_token, issue_station[i]->m_instruction_number,
					                             issue_station[i]->m_branch_waits);
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
						IDEX_branch_waits[i] = reservation_station[0]->m_branch_waits;
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
						Issue *tmp =
						    new Issue(ID_registers[i], ID_immediate[i], ID_command[i].token,
						              ID_command[i].instruction_number, dependency_not_met, ID_branch_waits[i]);
						issue_station.push_back(tmp);
						issue_entries++;
					}
					else if (ID_command[i].token == EOP)
					{
						Issue *tmp = new Issue(ID_registers[i], ID_immediate[i], ID_command[i].token,
						                       ID_command[i].instruction_number, false, ID_branch_waits[i]);
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
							Issue *tmp =
							    new Issue(ID_registers[i], ID_immediate[i], ID_command[i].token,
							              ID_command[i].instruction_number, dependency_not_met, ID_branch_waits[i]);
							issue_station.push_back(tmp);
							issue_entries++;
						}
						else if (ID_command[i].token == EOP)
						{
							Issue *tmp = new Issue(ID_registers[i], ID_immediate[i], ID_command[i].token,
							                       ID_command[i].instruction_number, true, ID_branch_waits[i]);
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

			for (int i = 0; i < issue_station.size(); i++) // Update Dependencies for all entries in the Issue Station
			{
				bool dependency_not_met = false;
				for (int j = 1; j < issue_station[i]->m_registers.size(); j++)
				{

					if (issue_station[i]->m_token != NOP && issue_station[i]->m_token != EOP &&
					    issue_station[i]->m_token != BEQ && issue_station[i]->m_token != BNE)
					{
						if (registers_in_use[issue_station[i]->m_registers[j]].in_use &&
						    issue_station[i]->m_registers[j] != issue_station[i]->m_registers[0])
						{
							// cout << issue_station[i]->m_instruction_number << " has token " << issue_station[i]->m_token
							//      << "  is blocked on " << issue_station[i]->m_registers[j] << endl;
							dependency_not_met = true;
						}
					}
				}
				if (issue_station[i]->m_token == BEQ || issue_station[i]->m_token == BNE)
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
				if (issue_station[i]->m_token == ST)
				{
					if (registers_in_use[issue_station[i]->m_registers[0]].in_use)
					{
						dependency_not_met = true;
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
					if (ID_command[i].token == BEQ || ID_command[i].token == BNE)
					{
						depends_on_branch = true;
						dependent_branch.push_back(ID_command[i].instruction_number);
					}

					if (!ID_registers[i].empty())
					{
						if (ID_command[i].token == BEQ || ID_command[i].token == ST || ID_command[i].token == BNE)
						{
							for (int j = 0; j < ID_registers[i].size(); j++)
							{
								ID_registers[i][j] = register_rename[ID_registers[i][j]];
							}
						}
						else
						{
							for (int j = 1; j < ID_registers[i].size(); j++)
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
						ID_branch_waits[i] = dependent_branch;
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
				if (PC + i < code.size())
				{
					IF_instruction_tokens[i] = fetch.getInstruction(code[PC + i]);
				}
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
