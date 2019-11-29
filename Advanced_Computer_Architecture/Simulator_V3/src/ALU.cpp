#include "ALU.hpp"

ALU &ALU::getInstance()
{
	static ALU alu;
	return alu;
}

int ALU::add(int *rs, int *rt)
{

	// ADD addition(rs, rt, rd);
	// return addition.run();
	return *rs + *rt;
}

int ALU::sub(int *rs, int *rt)
{
	// SUB subtraction(rs, rt, rd);
	// return subtraction.run();
	return *rs - *rt;
}

int ALU::mul(int *rs, int *rt)
{
	// MUL multiplication(rs, rt, rd);
	// return multiplication.run();
	return *rs * *rt;
}

int ALU::div(int *rs, int *rt)
{
	// DIV division(rs, rt, rd);
	// m_lock = division.m_duration;
	// return division.run();
	return *rs / *rt;
}

int ALU::mod(int *rs, int *rt)
{
	// MOD modular(rs, rt, rd);
	// return modular.run();
	return *rs % *rt;
}

int ALU::sll(int *rt, int shamt)
{
	// SLL shift_left_logical(rt, shamt, rd);
	// return shift_left_logical.run();
	return *rt >> shamt;
}

int ALU::srl(int *rt, int shamt)
{
	// SRL shift_right_logical(rt, shamt, rd);
	// return shift_right_logical.run();
	return *rt << shamt;
}

int ALU::and_op(int *rs, int *rt)
{
	// AND and_gate(rs, rt, rd);
	// return and_gate.run();
	return *rs & *rt;
}

int ALU::orr(int *rs, int *rt)
{
	// ORR orr(rs, rt, rd);
	// return orr.run();
	return *rs | *rt;
}

int ALU::nor(int *rs, int *rt)
{
	// NOR nor(rs, rt, rd);
	// return nor.run();
	return !(*rs | *rt);
}

int ALU::slt(int *rs, int *rt)
{
	// SLT slt(rs, rt, rd);
	// return slt.run();
	return (*rs < *rt ? 1 : 0);
}

int ALU::addi(int *rs, int immediate)
{
	// ADDI addi(rs, rd, immediate);
	// return addi.run();
	return *rs + immediate;
}

int ALU::subi(int *rs, int immediate)
{
	// SUBI subi(rs, rd, immediate);
	// return subi.run();
	return *rs - immediate;
}

int ALU::muli(int *rs, int immediate)
{
	// MULI muli(rs, rd, immediate);
	// return muli.run();
	return *rs * immediate;
}

int ALU::andi(int *rs, int immediate)
{
	// ANDI andi(rs, rd, immediate);
	// return andi.run();
	return *rs & immediate;
}

int ALU::ori(int *rs, int immediate)
{
	// ORI ori(rs, rd, immediate);
	// return ori.run();
	return *rs | immediate;
}

int ALU::divi(int *rs, int immediate)
{
	// DIVI divi(rs, rd, immediate);
	// return divi.run();
	return *rs / immediate;
}

int ALU::modi(int *rs, int immediate)
{
	// MODI modi(rs, rd, immediate);
	// return modi.run();
	return *rs % immediate;
}

int ALU::slti(int *rs, int immediate)
{
	// SLTI slti(rs, rd, immediate);
	// return slti.run();
	return (*rs < immediate ? 1 : 0);
}

int ALU::seq(int *rs, int immediate)
{
	// SEQ seq(rs, rd, immediate);
	// return seq.run();
	return (*rs == immediate ? 1 : 0);
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
