#include "execute.hpp"
#include <iostream>
extern int executed_instructions;
extern int g_clock;
extern bool branch_taken;

int nop_count = 0;
int missed_branch = 0;

int execute(ALU &alu, ISA instructions, int registers[64], vector<int> register_file, int immediate)
{
	// static int nop_count = 0;
	// static int missed_branch = 0;
	//static MEM &mem = MEM::getInstance();
	int result;
	switch (instructions)
	{
	case ADD:
		if (alu.m_lock == false)
		{
			alu.m_lock = true;
			alu.m_delay = 1;
		}
		alu.m_delay--;
		if (alu.m_delay == 0)
		{
			alu.m_lock = false;
			result = alu.add(&registers[register_file[1]], &registers[register_file[2]]);
			cout << " [ ADD ] " << alu.add(&registers[register_file[1]], &registers[register_file[2]]) << endl;
		}

		break;
	case SUB:
		if (alu.m_lock == false)
		{
			alu.m_lock = true;
			alu.m_delay = 1;
		}
		alu.m_delay--;
		if (alu.m_delay == 0)
		{
			alu.m_lock = false;
			result = alu.sub(&registers[register_file[1]], &registers[register_file[2]]);
			cout << " [ SUB ] " << alu.sub(&registers[register_file[1]], &registers[register_file[2]]) << endl;
		}

		break;
	case MUL:
		result = alu.mul(&registers[register_file[1]], &registers[register_file[2]]);
		cout << " [ MUL ] " << alu.mul(&registers[register_file[1]], &registers[register_file[2]]) << endl;
		break;
	case DIV:
		result = alu.div(&registers[register_file[1]], &registers[register_file[2]]);
		cout << " [ DIV ] " << alu.div(&registers[register_file[1]], &registers[register_file[2]]) << endl;
		break;
	case MOD:
		result = alu.mod(&registers[register_file[1]], &registers[register_file[2]]);
		cout << " [ MOD ] " << alu.mod(&registers[register_file[1]], &registers[register_file[2]]) << endl;
		break;
	case SLL:
		result = alu.sll(&registers[register_file[1]], immediate);
		cout << " [ SLL ] " << alu.sll(&registers[register_file[1]], immediate) << endl;
		break;
	case SRL:
		result = alu.srl(&registers[register_file[1]], immediate);
		cout << " [ SRL ] " << alu.srl(&registers[register_file[1]], immediate) << endl;
		break;
	case AND:
		result = alu.and_op(&registers[register_file[1]], &registers[register_file[2]]);
		cout << " [ AND ] " << alu.and_op(&registers[register_file[1]], &registers[register_file[2]]) << endl;
		break;
	case ORR:
		result = alu.orr(&registers[register_file[1]], &registers[register_file[2]]);
		cout << " [ ORR ] " << alu.orr(&registers[register_file[1]], &registers[register_file[2]]) << endl;
		break;
	case NOR:
		result = alu.nor(&registers[register_file[1]], &registers[register_file[2]]);
		cout << " [ NOR ] " << alu.nor(&registers[register_file[1]], &registers[register_file[2]]) << endl;
		break;
	case SLT:
		result = alu.slt(&registers[register_file[1]], &registers[register_file[2]]);
		cout << " [ SLT ] " << alu.slt(&registers[register_file[1]], &registers[register_file[2]]) << endl;
		break;
	case ADDI:
		if (alu.m_lock == false)
		{
			alu.m_lock = true;
			alu.m_delay = 2;
		}
		alu.m_delay--;
		if (alu.m_delay == 0)
		{
			alu.m_lock = false;
			result = alu.addi(&registers[register_file[1]], immediate);
			cout << " [ ADDI ] " << alu.addi(&registers[register_file[1]], immediate) << endl;
		}
		break;
	case SUBI:
		result = alu.subi(&registers[register_file[1]], immediate);
		cout << " [ SUBI ] " << alu.subi(&registers[register_file[1]], immediate) << endl;
		break;
	case MULI:
		result = alu.muli(&registers[register_file[1]], immediate);
		cout << " [ MULI ] " << alu.muli(&registers[register_file[1]], immediate) << endl;
		break;
	case DIVI:
		result = alu.divi(&registers[register_file[1]], immediate);
		cout << " [ DIVI ] " << alu.divi(&registers[register_file[1]], immediate) << endl;
		break;
	case ANDI:
		result = alu.andi(&registers[register_file[1]], immediate);
		cout << " [ ANDI ] " << alu.andi(&registers[register_file[1]], immediate) << endl;
		break;
	case ORI:
		result = alu.ori(&registers[register_file[1]], immediate);
		cout << " [ ORI ] " << alu.ori(&registers[register_file[1]], immediate) << endl;
		break;
	case MODI:
		result = alu.modi(&registers[register_file[1]], immediate);
		cout << " [ MODI ] " << alu.modi(&registers[register_file[1]], immediate) << endl;
		break;
	case SLTI:
		result = alu.slti(&registers[register_file[1]], immediate);
		cout << " [ SLTI ] " << alu.slti(&registers[register_file[1]], immediate) << endl;
		break;
	case BEQ:
		result = alu.beq(&registers[register_file[0]], &registers[register_file[1]], immediate);
		cout << " [ BEQ ] ";
		if (branch_taken)
		{
			missed_branch++;
			cout << " Taken: ";
		}
		else
		{
			cout << " Not Taken ";
		}
		cout << result << endl;
		// cout << " [ BEQ ] " << alu.beq(&registers[register_file[0]], &registers[register_file[1]], immediate) << endl;
		break;
	case BNE:
		result = alu.bne(&registers[register_file[0]], &registers[register_file[1]], immediate);
		cout << " [ BNE ] ";
		if (branch_taken)
		{
			missed_branch++;
			cout << " Taken: ";
		}
		else
		{
			cout << " Not Taken ";
		}
		cout << result << endl;
		//cout << " [ BNE ] " << alu.bne(&registers[register_file[0]], &registers[register_file[1]], immediate) << endl;
		break;
	// case LD:
	// 	cout << " [ LD ] " << mem.ld(&registers[register_file[0]], immediate) << endl;
	// 	break;
	// case ST:
	// 	mem.st(&registers[register_file[0]], immediate);
	// 	cout << " [ ST ] " << endl;
	// 	break;
	case SEQ:
		result = alu.seq(&registers[register_file[1]], immediate);
		cout << " [ SEQ ] " << alu.seq(&registers[register_file[1]], immediate) << endl;
		break;
	case EOP:
		cout << " [ EOP ] Program terminated successfully " << endl;
		cout << " { " << (executed_instructions + nop_count) << " } Instructions Executed " << endl;
		cout << " { " << (executed_instructions) << " } Useful Instructions Executed " << endl;
		cout << " { " << ceil(g_clock) << " } Clock Cycles Taken " << endl;
		cout << " { " << (executed_instructions + nop_count) / (ceil(g_clock)) << " } Instructions per Cycle" << endl;
		cout << " { " << nop_count << " } NOP Instructions" << endl;
		cout << " { " << missed_branch << " } Miss-Predicted Branches" << endl;
		exit(EXIT_SUCCESS);

	case NOP:
		cout << " [ NOP ]\n";
		nop_count++;
		g_clock++;
		break;
	}
	if (!alu.m_lock)
	{
		executed_instructions++;
	}
	return result;
}

void end()
{
	cout << " [ EOP ] Program terminated successfully " << endl;
	cout << " { " << (executed_instructions + nop_count) << " } Instructions Executed " << endl;
	cout << " { " << (executed_instructions) << " } Useful Instructions Executed " << endl;
	cout << " { " << ceil(g_clock) << " } Clock Cycles Taken " << endl;
	cout << " { " << (executed_instructions + nop_count) / (ceil(g_clock)) << " } Instructions per Cycle" << endl;
	cout << " { " << nop_count << " } NOP Instructions" << endl;
	cout << " { " << missed_branch << " } Miss-Predicted Branches" << endl;
	exit(EXIT_SUCCESS);
}
