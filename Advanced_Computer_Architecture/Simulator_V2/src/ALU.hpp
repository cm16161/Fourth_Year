#ifndef ALU_HPP
#define ALU_HPP
#include "ISA.hpp"
#include "branch.hpp"

class ALU
{
	//ALU(){};

public:
	static ALU &getInstance();
	ALU(){};
	bool m_lock = false;
  int m_delay = 0;
	int add(int *rs, int *rt);
	int sub(int *rs, int *rt);
	int mul(int *rs, int *rt);
	int div(int *rs, int *rt);
	int mod(int *rs, int *rt);
	int sll(int *rt, int shamt);
	int srl(int *rt, int shamt);
	int and_op(int *rs, int *rt);
	int orr(int *rs, int *rt);
	int nor(int *rs, int *rt);
	int slt(int *rs, int *rt);

	int addi(int *rs, int immediate);
	int subi(int *rs, int immediate);
	int muli(int *rs, int immediate);
	int andi(int *rs, int immediate);
	int ori(int *rs, int immediate);
	int divi(int *rs, int immediate);
	int modi(int *rs, int immediate);
	int slti(int *rs, int immediate);

	int seq(int *rs, int immediate);

	int ji(int address);
	int jr(int *rs);
	int beq(int *rs, int *rt, int address);
	int bne(int *rs, int *rt, int address);

	int ldi(int *rs, int immediate);
};

#endif
