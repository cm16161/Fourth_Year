#ifndef ISA_HPP
#define ISA_HPP
#include <cstring>
#include <string>
#include <iostream>
extern int PC;

class Instruction
{
public:
	int m_opcode;
	virtual int run() = 0;
};

class R : public Instruction
{
public:
	int *m_rs, *m_rt, *m_rd, m_shamt, m_funct;
	R(int opcode, int *rs, int *rt, int *rd, int shamt, int funct)
	{
		m_opcode = opcode;
		m_rs = rs;
		m_rt = rt;
		m_shamt = shamt;
		m_funct = funct;
	};
	R(){};
};

class I : public Instruction
{
public:
	int *m_rs, *m_rd, m_immediate;
	I(int opcode, int *rs, int *rd, int immediate)
	{
		m_opcode = opcode;
		m_rs = rs;
		m_rd = rd;
		m_immediate = immediate;
	};
	I(){};
};

class J : Instruction
{
	int m_address;
	J(int opcode, int address)
	{
		m_opcode = opcode;
		m_address = address;
	};
};

/*
///////////////////////////////////////////////////////////////////////////

////// Below are definitions for all implemented R-type Instructions //////

///////////////////////////////////////////////////////////////////////////
*/

class alu_ADD : public R
{

public:
	alu_ADD()
	{
		m_opcode = 0;
		m_shamt = 0;
		m_funct = 32;
	}
	alu_ADD(int *rs, int *rt, int *rd)
	{
		m_opcode = 0;
		m_shamt = 0;
		m_funct = 32;
		m_rs = rs;
		m_rt = rt;
		m_rd = rd;
	}

	int run()
	{
		*m_rd = *m_rs + *m_rt;
		return *m_rd;
	}
};

class SUB : public R
{
public:
	SUB()
	{
		m_opcode = 0;
		m_shamt = 0;
		m_funct = 34;
	}

	SUB(int *rs, int *rt, int *rd)
	{
		m_opcode = 0;
		m_shamt = 0;
		m_funct = 34;
		m_rs = rs;
		m_rt = rt;
		m_rd = rd;
	}

	int run()
	{
		*m_rd = *m_rs - *m_rt;
		return *m_rd;
	}
};

class MUL : public R
{
public:
	MUL()
	{
		m_opcode = 0;
		m_shamt = 0;
		m_funct = 24;
	}

	MUL(int *rs, int *rt, int *rd)
	{
		m_opcode = 0;
		m_shamt = 0;
		m_funct = 24;
		m_rs = rs;
		m_rt = rt;
		m_rd = rd;
	}

	int run()
	{
		*m_rd = *m_rs * *m_rt;
		return *m_rd;
	}
};

class DIV : public R
{
public:
	DIV()
	{
		m_opcode = 0;
		m_shamt = 0;
		m_funct = 26;
	}

	DIV(int *rs, int *rt, int *rd)
	{
		m_opcode = 0;
		m_shamt = 0;
		m_funct = 26;
		m_rs = rs;
		m_rt = rt;
		m_rd = rd;
	}

	int run()
	{
		*m_rd = *m_rs / *m_rt;
		return *m_rd;
	}
};

class MOD : public R
{
public:
	MOD()
	{
		m_opcode = 0;
		m_shamt = 0;
		m_funct = 27;
	}

	MOD(int *rs, int *rt, int *rd)
	{
		m_opcode = 0;
		m_shamt = 0;
		m_funct = 25;
		m_rs = rs;
		m_rt = rt;
		m_rd = rd;
	}

	int run()
	{
		*m_rd = *m_rs % *m_rt;
		return *m_rd;
	}
};

class SLL : public R
{
public:
	SLL()
	{
		m_opcode = 0;
		m_funct = 0;
		m_rs = 0;
	}

	SLL(int *rt, int shamt, int *rd)
	{
		m_opcode = 0;
		m_shamt = shamt;
		m_funct = 0;
		m_rs = 0;
		m_rt = rt;
		m_rd = rd;
	}

	int run()
	{
		*m_rd = *m_rt << m_shamt;
		return *m_rd;
	}
};

class SRL : public R
{
public:
	SRL()
	{
		m_opcode = 0;
		m_rs = 0;
		m_funct = 2;
	}

	SRL(int *rt, int shamt, int *rd)
	{
		m_opcode = 0;
		m_shamt = shamt;
		m_funct = 2;
		m_rs = 0;
		m_rt = rt;
		m_rd = rd;
	}

	int run()
	{
		*m_rd = *m_rt >> m_shamt;
		return *m_rd;
	}
};

class AND : public R
{
public:
	AND()
	{
		m_opcode = 0;
		m_shamt = 0;
		m_funct = 36;
	}

	AND(int *rs, int *rt, int *rd)
	{
		m_opcode = 0;
		m_shamt = 0;
		m_funct = 36;
		m_rs = rs;
		m_rt = rt;
		m_rd = rd;
	}

	int run()
	{
		*m_rd = *m_rs & *m_rt;
		return *m_rd;
	}
};

class ORR : public R
{
public:
	ORR()
	{
		m_opcode = 0;
		m_shamt = 0;
		m_funct = 37;
	}

	ORR(int *rs, int *rt, int *rd)
	{
		m_opcode = 0;
		m_shamt = 0;
		m_funct = 37;
		m_rs = rs;
		m_rt = rt;
		m_rd = rd;
	}

	int run()
	{
		*m_rd = *m_rs | *m_rt;
		return *m_rd;
	}
};

class NOR : public R
{
public:
	NOR()
	{
		m_opcode = 0;
		m_shamt = 0;
		m_funct = 39;
	}

	NOR(int *rs, int *rt, int *rd)
	{
		m_opcode = 0;
		m_shamt = 0;
		m_funct = 39;
		m_rs = rs;
		m_rt = rt;
		m_rd = rd;
	}

	int run()
	{
		*m_rd = !(*m_rs | *m_rt);
		return *m_rd;
	}
};

class JR : public R
{
public:
	JR()
	{
		m_opcode = 0;
		m_shamt = 0;
		m_funct = 8;
		m_rt = 0;
		m_rd = 0;
	}
	JR(int *rs)
	{
		m_opcode = 0;
		m_shamt = 0;
		m_funct = 8;
		m_rt = 0;
		m_rd = 0;
		m_rs = rs;
	}
	int run()
	{
		PC = *m_rs;
		return 0;
	}
};

class SLT : public R
{
public:
	SLT()
	{
		m_opcode = 0;
		m_shamt = 0;
		m_funct = 42;
	}
	SLT(int *rs, int *rt, int *rd)
	{
		m_rs = rs;
		m_rt = rt;
		m_rd = rd;
	}
	int run()
	{
		*m_rd = *m_rs < *m_rt ? 1 : 0;
		return *m_rd;
	}
};

/*
///////////////////////////////////////////////////////////////////////////

////// Below are definitions for all implemented I-type Instructions //////

///////////////////////////////////////////////////////////////////////////
*/

class alu_ADDI : public I
{
public:
	alu_ADDI()
	{
		m_opcode = 8;
	}
	alu_ADDI(int *rs, int *rd, int immediate)
	{
		m_opcode = 8;
		m_rs = rs;
		m_rd = rd;
		m_immediate = immediate;
	}
	int run()
	{
		*m_rd = *m_rs + m_immediate;
		return *m_rd;
	}
};

class SUBI : public I
{
public:
	SUBI()
	{
		m_opcode = 8;
	}
	SUBI(int *rs, int *rd, int immediate)
	{
		m_opcode = 8;
		m_rs = rs;
		m_rd = rd;
		m_immediate = immediate;
	}
	int run()
	{
		*m_rd = *m_rs - m_immediate;
		return *m_rd;
	}
};

class MULI : public I
{
public:
	MULI()
	{
		m_opcode = 8;
	}
	MULI(int *rs, int *rd, int immediate)
	{
		m_opcode = 8;
		m_rs = rs;
		m_rd = rd;
		m_immediate = immediate;
	}
	int run()
	{
		*m_rd = *m_rs * m_immediate;
		return *m_rd;
	}
};

class ANDI : public I
{
public:
	ANDI()
	{
		m_opcode = 12;
	}
	ANDI(int *rs, int *rd, int immediate)
	{
		m_opcode = 12;
		m_rs = rs;
		m_rd = rd;
		m_immediate = immediate;
	}
	int run()
	{
		*m_rd = *m_rs & m_immediate;
		return *m_rd;
	}
};

class ORI : public I
{
public:
	ORI()
	{
		m_opcode = 13;
	}
	ORI(int *rs, int *rd, int immediate)
	{
		m_opcode = 13;
		m_rs = rs;
		m_rd = rd;
		m_immediate = immediate;
	}
	int run()
	{
		*m_rd = *m_rs | m_immediate;
		return *m_rd;
	}
};

class DIVI : public I
{
public:
	DIVI()
	{
		m_opcode = 13;
	}
	DIVI(int *rs, int *rd, int immediate)
	{
		m_opcode = 13;
		m_rs = rs;
		m_rd = rd;
		m_immediate = immediate;
	}
	int run()
	{
		*m_rd = *m_rs / m_immediate;
		return *m_rd;
	}
};

class MODI : public I
{
public:
	MODI()
	{
		m_opcode = 13;
	}
	MODI(int *rs, int *rd, int immediate)
	{
		m_opcode = 13;
		m_rs = rs;
		m_rd = rd;
		m_immediate = immediate;
	}
	int run()
	{
		*m_rd = *m_rs % m_immediate;
		return *m_rd;
	}
};

class SLTI : public I
{
public:
	SLTI()
	{
		m_opcode = 10;
	}
	SLTI(int *rs, int *rd, int immediate)
	{
		m_opcode = 13;
		m_rs = rs;
		m_rd = rd;
		m_immediate = immediate;
	}
	int run()
	{
		*m_rd = *m_rs < m_immediate ? 1 : 0;
		return *m_rd;
	}
};

#endif
