#include "ALU.hpp"
#include "ISA.hpp"
#include <iostream>
int *PC;

using namespace std;

int main()
{
	int a, b, c;
	a = 1;
	b = 2;
	c = 0;
	ALU alu;
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
	return 0;
}
