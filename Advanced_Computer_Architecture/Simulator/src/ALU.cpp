#include "ALU.hpp"
#include "ISA.hpp"

int ALU::add(int *rs, int *rt, int *rd)
{

	ADD addition(rs, rt, rd);
	return addition.run();
}

int ALU::sub(int *rs, int *rt, int *rd)
{
	SUB subtraction(rs, rt, rd);
	return subtraction.run();
}

int ALU::mul(int *rs, int *rt, int *rd)
{
	MUL multiplication(rs, rt, rd);

	return multiplication.run();
}

int ALU::div(int *rs, int *rt, int *rd)
{
	DIV division(rs, rt, rd);

	return division.run();
}

int ALU::mod(int *rs, int *rt, int *rd)
{
	MOD modular(rs, rt, rd);

	return modular.run();
}

int ALU::sll(int *rt, int shamt, int *rd)
{
	SLL shift_left_logical(rt, shamt, rd);
	return shift_left_logical.run();
}

int ALU::srl(int *rt, int shamt, int *rd)
{
	SRL shift_right_logical(rt, shamt, rd);
	return shift_right_logical.run();
}

int ALU::and_op(int *rs, int *rt, int *rd)
{
	AND and_gate(rs, rt, rd);
	return and_gate.run();
}

int ALU::orr(int *rs, int *rt, int *rd)
{
	ORR orr(rs, rt, rd);
	return orr.run();
}

int ALU::nor(int *rs, int *rt, int *rd)
{
	NOR nor(rs, rt, rd);
	return nor.run();
}

int ALU::jr(int *rs)
{
	JR jr(rs);
	return jr.run();
}

int ALU::slt(int *rs, int *rt, int *rd)
{
	SLT slt(rs, rt, rd);
	return slt.run();
}
