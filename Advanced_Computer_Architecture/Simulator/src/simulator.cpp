#include "ALU.hpp"
#include "ISA.hpp"
#include "decode.hpp"
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>
#include "execute.hpp"

#define N_REGISTERS 64
int PC;

//TODO CHECK EXECUTE.CPP

using namespace std;

void getInstruction(string line, vector<string> *tokens)
{
	tokens->clear();
	istringstream iss(line);
	copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter(*tokens));
}

void getCode(vector<string> *instructions)
{
	ifstream file("alu_processes.txt");
	if (file.is_open())
	{
		string line;
		while (getline(file, line))
		{
			instructions->push_back(line);
		}
		file.close();
	}
}

void getRegisters(string line, vector<int> *registers)
{
	registers->clear();
	char *my_line = const_cast<char *>(line.c_str());
	char *token = strtok(my_line, "$");
	while (token != nullptr)
	{
		token = strtok(NULL, "$");
		if (token != nullptr)
		{
			registers->push_back(atoi(token));
		}
	}
}

int getImmediate(string line)
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

int main()
{
	PC = 0;
	int a, b, c;
	a = 1;
	b = 2;
	c = 0;
	ALU alu;
	int registers[N_REGISTERS] = { 0 };
	vector<string> tokens, code;
	vector<int> register_file;
	int immediate;
	getCode(&code);
	for (;;)
	{
		getInstruction(code[PC], &tokens);
		getRegisters(code[PC], &register_file);
		immediate = getImmediate(code[PC]);
		PC++;
		execute(alu, cmp(tokens[0]), registers, register_file, immediate);
	}
	cout << "A = " << a << " B = " << b << " C = " << c << endl;
	cout << " [ ADD ] " << alu.add(&a, &b, &c) << endl;
	cout << " [ SUB ] " << alu.sub(&b, &a, &c) << endl;
	cout << " [ MUL ] " << alu.mul(&a, &b, &c) << endl;
	cout << " [ DIV ] " << alu.div(&a, &b, &c) << endl;
	cout << " [ MOD ] " << alu.mod(&a, &b, &c) << endl;
	cout << " [ SLL ] " << alu.sll(&a, 5, &c) << endl;
	cout << " [ SLR ] " << alu.srl(&c, 5, &c) << endl;
	cout << " [ AND ] " << alu.and_op(&a, &b, &c) << endl;
	cout << " [ ORR ] " << alu.orr(&a, &b, &c) << endl;
	cout << " [ NOR ] " << alu.nor(&a, &b, &c) << endl;
	cout << " [ SLT ] " << alu.slt(&a, &b, &c) << endl;

	cout << " [ ADDI ] " << alu.addi(&a, &c, 5) << endl;
	cout << " [ SUBI ] " << alu.subi(&a, &c, 5) << endl;
	cout << " [ MULI ] " << alu.muli(&a, &c, 5) << endl;
	cout << " [ ANDI ] " << alu.andi(&a, &c, 5) << endl;
	cout << " [ ORI ] " << alu.ori(&a, &c, 5) << endl;
	cout << " [ DIVI ] " << alu.divi(&a, &c, 5) << endl;
	cout << " [ MODI ] " << alu.modi(&a, &c, 5) << endl;
	cout << " [ SLTI ] " << alu.slti(&a, &c, 5) << endl;
	return 0;
}
