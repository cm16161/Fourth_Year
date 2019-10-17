#include "execute.hpp"
#include <iostream>

extern int PC;

//TODO Move Registers to separate folder!
void execute(ALU alu, ISA instructions, int registers[64], vector<int> register_file, int immediate)
{
	switch (instructions)
	{
	case ADD:
		cout << " [ ADD ] "
		     << alu.add(&registers[register_file[1]], &registers[register_file[2]], &registers[register_file[0]])
		     << endl;
		break;
	case SUB:
		cout << " [ SUB ] "
		     << alu.sub(&registers[register_file[1]], &registers[register_file[2]], &registers[register_file[0]])
		     << endl;
		break;
	case MUL:
		cout << " [ MUL ] "
		     << alu.mul(&registers[register_file[1]], &registers[register_file[2]], &registers[register_file[0]])
		     << endl;
		break;
	case DIV:
		cout << " [ DIV ] "
		     << alu.div(&registers[register_file[1]], &registers[register_file[2]], &registers[register_file[0]])
		     << endl;
		break;
	case MOD:
		cout << " [ MOD ] "
		     << alu.mod(&registers[register_file[1]], &registers[register_file[2]], &registers[register_file[0]])
		     << endl;
		break;
	case SLL:
		cout << " [ SLL ] " << alu.sll(&registers[register_file[1]], immediate, &registers[register_file[0]]) << endl;
		break;
	case SRL:
		cout << " [ SRL ] " << alu.srl(&registers[register_file[1]], immediate, &registers[register_file[0]]) << endl;
		break;
	case AND:
		cout << " [ AND ] "
		     << alu.and_op(&registers[register_file[1]], &registers[register_file[2]], &registers[register_file[0]])
		     << endl;
		break;
	case ORR:
		cout << " [ ORR ] "
		     << alu.orr(&registers[register_file[1]], &registers[register_file[2]], &registers[register_file[0]])
		     << endl;
		break;
	case NOR:
		cout << " [ NOR ] "
		     << alu.nor(&registers[register_file[1]], &registers[register_file[2]], &registers[register_file[0]])
		     << endl;
		break;
	case SLT:
		cout << " [ SLT ] "
		     << alu.slt(&registers[register_file[1]], &registers[register_file[2]], &registers[register_file[0]])
		     << endl;
		break;
	case ADDI:
		cout << " [ ADDI ] " << alu.addi(&registers[register_file[1]], &registers[register_file[0]], immediate) << endl;
		break;
	case SUBI:
		cout << " [ SUBI ] " << alu.subi(&registers[register_file[1]], &registers[register_file[0]], immediate) << endl;
		break;
	case MULI:
		cout << " [ MULI ] " << alu.muli(&registers[register_file[1]], &registers[register_file[0]], immediate) << endl;
		break;
	case DIVI:
		cout << " [ DIVI ] " << alu.divi(&registers[register_file[1]], &registers[register_file[0]], immediate) << endl;
		break;
	case ANDI:
		cout << " [ ANDI ] " << alu.andi(&registers[register_file[1]], &registers[register_file[0]], immediate) << endl;
		break;
	case ORI:
		cout << " [ ORI ] " << alu.ori(&registers[register_file[1]], &registers[register_file[0]], immediate) << endl;
		break;
	case MODI:
		cout << " [ MODI ] " << alu.modi(&registers[register_file[1]], &registers[register_file[0]], immediate) << endl;
		break;
	case SLTI:
		cout << " [ SLTI ] " << alu.slti(&registers[register_file[1]], &registers[register_file[0]], immediate) << endl;
		break;
	case EOP:
		cout << " [ EOP ] Program terminated successfully " << endl;
		exit(EXIT_SUCCESS);
	case NOP:
		break;
	}
}
