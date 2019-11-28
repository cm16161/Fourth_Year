#ifndef DECODE_HPP
#define DECODE_HPP
#include <cstring>
#include <string>
#include <vector>
using namespace std;

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
	LD,
	LDI,
	ST,
        SEQ,

};

class Decode
{
	Decode(){};

public:
	static Decode &getInstance();
	ISA decode(std::string inst);
	vector<int> getRegisters(string line);
	int getImmediate(string line);
};

#endif
