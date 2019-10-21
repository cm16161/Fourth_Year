#include "ALU.hpp"
#include "ISA.hpp"

ALU &ALU::getInstance()
{
	static ALU alu;
	return alu;
}

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

int ALU::slt(int *rs, int *rt, int *rd)
{
	SLT slt(rs, rt, rd);
	return slt.run();
}

int ALU::addi(int *rs, int *rd, int immediate)
{
	ADDI addi(rs, rd, immediate);
	return addi.run();
}

int ALU::subi(int *rs, int *rd, int immediate)
{
	SUBI subi(rs, rd, immediate);
	return subi.run();
}

int ALU::muli(int *rs, int *rd, int immediate)
{
	MULI muli(rs, rd, immediate);
	return muli.run();
}

int ALU::andi(int *rs, int *rd, int immediate)
{
	ANDI andi(rs, rd, immediate);
	return andi.run();
}

int ALU::ori(int *rs, int *rd, int immediate)
{
	ORI ori(rs, rd, immediate);
	return ori.run();
}

int ALU::divi(int *rs, int *rd, int immediate)
{
	DIVI divi(rs, rd, immediate);
	return divi.run();
}

int ALU::modi(int *rs, int *rd, int immediate)
{
	MODI modi(rs, rd, immediate);
	return modi.run();
}

int ALU::slti(int *rs, int *rd, int immediate)
{
	SLTI slti(rs, rd, immediate);
	return slti.run();
}

int ALU::ji(int immediate)
{
	JI ji(immediate);
	return ji.run();
}

int ALU::jr(int *rs)
{
	JR jr(rs);
	return jr.run();
}
int ALU::beq(int *rs, int *rt, int address)
{
	BEQ beq(rs, rt, address);
	return beq.run();
}
int ALU::bne(int *rs, int *rt, int address)
{
	BNE bne(rs, rt, address);
	return bne.run();
}

int ALU::ldi(int *rs, int immediate)
{
	LDI ldi(rs, immediate);
	return ldi.run();
}
