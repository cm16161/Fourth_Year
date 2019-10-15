#include "ALU.hpp"
#include "ISA.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#define N_REGISTERS 64
int PC;

using namespace std;

void getInstruction(string line, vector<string> *tokens)
{
	PC++;
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
	getCode(&code);
	cout << code[PC] << endl;
	getInstruction(code[PC], &tokens);
	for (auto i : tokens)
	{
		cout << i << endl;
	}

	cout << "A = " << a << " B = " << b << " C = " << c << endl;
	cout << " [ ADD ] " << ADD(&a, &b, &c) << endl;
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

	cout << " [ ADDI ] " << ADDI(&a, &c, 5) << endl;
	cout << " [ SUBI ] " << alu.subi(&a, &c, 5) << endl;
	cout << " [ MULI ] " << alu.muli(&a, &c, 5) << endl;
	cout << " [ ANDI ] " << alu.andi(&a, &c, 5) << endl;
	cout << " [ ORI ] " << alu.ori(&a, &c, 5) << endl;
	cout << " [ DIVI ] " << alu.divi(&a, &c, 5) << endl;
	cout << " [ MODI ] " << alu.modi(&a, &c, 5) << endl;
	cout << " [ SLTI ] " << alu.slti(&a, &c, 5) << endl;
	return 0;
}
