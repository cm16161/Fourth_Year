#ifndef DECODE_HPP
#define DECODE_HPP
#include <string>

enum ISA
{
	ADD,
	SUB,
	MUL,
	DIV,
	MOD,
	SLL,
	SRL,
	AND,
	ORR,
	NOR,
	SLT,
	ADDI,
	SUBI,
	MULI,
	DIVI,
	ANDI,
	ORI,
	MODI,
	SLTI,
        JI,
        JR,
        BEQ,
        BNE,
	NOP,
	EOP,

};

ISA cmp(std::string inst);

#endif
