#ifndef ALU_HPP
#define ALU_HPP

class ALU
{
public:
	int add(int *rs, int *rt, int *rd);
	int sub(int *rs, int *rt, int *rd);
	int mul(int *rs, int *rt, int *rd);
	int div(int *rs, int *rt, int *rd);
	int mod(int *rs, int *rt, int *rd);
	int sll(int *rt, int shamt, int *rd);
	int srl(int *rt, int shamt, int *rd);
	int and_op(int *rs, int *rt, int *rd);
	int orr(int *rs, int *rt, int *rd);
	int nor(int *rs, int *rt, int *rd);
	int jr(int *rs);
	int slt(int *rs, int *rt, int *rd);

	int addi(int *rs, int *rd, int immediate);
	int subi(int *rs, int *rd, int immediate);
	int muli(int *rs, int *rd, int immediate);
	int andi(int *rs, int *rd, int immediate);
	int ori(int *rs, int *rd, int immediate);
	int divi(int *rs, int *rd, int immediate);
	int modi(int *rs, int *rd, int immediate);
	int slti(int *rs, int *rd, int immediate);
};

#endif
