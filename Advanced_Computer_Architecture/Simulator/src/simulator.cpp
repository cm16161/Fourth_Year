#include "ISA.hpp"
#include "ALU.hpp"
#include <iostream>
int* PC;

using namespace std;

int main()
{
	int a, b, c;
	a = 1;
	b = 2;
	c = 0;
	ALU alu;
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
	return 0;
}
