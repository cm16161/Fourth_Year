#ifndef ISA_HPP
#define ISA_HPP
#include <cstring>
#include <iostream>
#include <string>
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
public:
	int m_address;
	int m_opcode;
	J(int opcode, int address)
	{
		m_opcode = opcode;
		m_address = address;
	};
	J(){};
};

/*
///////////////////////////////////////////////////////////////////////////

////// Below are definitions for all implemented R-type Instructions //////

///////////////////////////////////////////////////////////////////////////
*/

class ADD : public R
{

public:
	ADD()
	{
		m_opcode = 0;
		m_shamt = 0;
		m_funct = 32;
	}
	ADD(int *rs, int *rt, int *rd)
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

class ADDI : public I
{
public:
	ADDI()
	{
		m_opcode = 8;
	}
	ADDI(int *rs, int *rd, int immediate)
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

/*
///////////////////////////////////////////////////////////////////////////

////// Below are definitions for all implemented J-type Instructions //////

///////////////////////////////////////////////////////////////////////////
*/

class JI : public J
{
public:
	JI()
	{
		m_opcode = 2;
	}
	JI(int address)
	{
		m_opcode = 2;
		m_address = address;
	}
	int run()
	{
		PC = m_address;
		return PC;
	}
};

class JR : public R
{
public:
	JR()
	{
		m_opcode = 8;
	}
	JR(int *rs)
	{
		m_opcode = 8;
		m_rs = rs;
	}
	int run()
	{
		PC = *m_rs;
		return PC;
	}
};

class BEQ : public I
{
public:
	BEQ()
	{
		m_opcode = 4;
	}
	BEQ(int *rs, int *rd, int immediate)
	{
		m_opcode = 4;
		m_rs = rs;
		m_rd = rd;
		m_immediate = immediate;
	}
	int run()
	{
		if (*m_rs == *m_rd)
		{
			PC = m_immediate;
		}
		return PC;
	}
};

class BNE : public I
{
public:
	BNE()
	{
		m_opcode = 5;
	}
	BNE(int *rs, int *rd, int immediate)
	{
		m_opcode = 5;
		m_rs = rs;
		m_rd = rd;
		m_immediate = immediate;
	}
	int run()
	{
		if (*m_rs != *m_rd)
		{
			PC = m_immediate;
		}
		return PC;
	}
};

class LDI : public I
{
public:
	LDI(int *rs, int immediate)
	{
		m_rs = rs;
		m_immediate = immediate;
	}
	int run()
	{
		*m_rs = m_immediate;
		return *m_rs;
	}
};

#endif
