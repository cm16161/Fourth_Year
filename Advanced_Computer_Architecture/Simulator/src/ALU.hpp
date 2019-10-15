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
};
