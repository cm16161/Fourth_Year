#include "decode.hpp"

ISA Decode::decode(std::string inst)
{
	if (inst.compare("ADD") == 0)
	{
		return ADD;
	}
	else if (inst.compare("SUB") == 0)
	{
		return SUB;
	}
	else if (inst.compare("MUL") == 0)
	{
		return MUL;
	}
	else if (inst.compare("DIV") == 0)
	{
		return DIV;
	}
	else if (inst.compare("MOD") == 0)
	{
		return MOD;
	}
	else if (inst.compare("SLL") == 0)
	{
		return SLL;
	}
	else if (inst.compare("SRL") == 0)
	{
		return SRL;
	}
	else if (inst.compare("AND") == 0)
	{
		return AND;
	}
	else if (inst.compare("ORR") == 0)
	{
		return ORR;
	}
	else if (inst.compare("NOR") == 0)
	{
		return NOR;
	}
	else if (inst.compare("SLT") == 0)
	{
		return SLT;
	}
	else if (inst.compare("ADDI") == 0)
	{
		return ADDI;
	}
	else if (inst.compare("SUBI") == 0)
	{
		return SUBI;
	}
	else if (inst.compare("MULI") == 0)
	{
		return MULI;
	}
	else if (inst.compare("DIVI") == 0)
	{
		return DIVI;
	}
	else if (inst.compare("MODI") == 0)
	{
		return MODI;
	}
	else if (inst.compare("ANDI") == 0)
	{
		return ANDI;
	}
	else if (inst.compare("ORI") == 0)
	{
		return ORI;
	}
	else if (inst.compare("SLTI") == 0)
	{
		return SLTI;
	}
	else if (inst.compare("JI") == 0)
	{
		return JI;
	}
	else if (inst.compare("JR") == 0)
	{
		return JR;
	}
	else if (inst.compare("BEQ") == 0)
	{
		return BEQ;
	}
	else if (inst.compare("BNE") == 0)
	{
		return BNE;
	}
	else if (inst.compare("LD") == 0)
	{
		return LD;
	}
	else if (inst.compare("LDI") == 0)
	{
		return LDI;
	}
	else if (inst.compare("ST") == 0)
	{
		return ST;
	}
	else if (inst.compare("EOP") == 0)
	{
		return EOP;
	}
	else
		return NOP;
}

Decode &Decode::getInstance()
{
	static Decode decode;
	return decode;
}

vector<int> Decode::getRegisters(string line)
{
        vector<int> registers_to_use;
	char *my_line = const_cast<char *>(line.c_str());
	char *token = strtok(my_line, "$");
	while (token != nullptr)
	{
		token = strtok(NULL, "$");
		if (token != nullptr)
		{
			registers_to_use.push_back(atoi(token));
		}
	}
        return registers_to_use;
}

int Decode::getImmediate(string line)
{
	char *my_line = const_cast<char *>(line.c_str());
	char *token = strtok(my_line, "#");
	while (token != nullptr)
	{
		token = strtok(nullptr, "#");

		if (token != nullptr)
		{
			return atoi(token);
		}
	}
	return 0;
}
