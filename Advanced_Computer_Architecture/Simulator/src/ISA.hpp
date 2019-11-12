#ifndef ISA_HPP
#define ISA_HPP
#include <cstring>
#include <iostream>
#include <string>
extern int PC;

class Instruction
{
public:
	virtual int run() = 0;
	bool m_complete = false;
};

class R : public Instruction
{
public:
	int *m_rs, *m_rt, *m_rd, m_shamt, m_duration;
	R(int *rs, int *rt, int *rd, int shamt, int funct)
	{

		m_rs = rs;
		m_rt = rt;
		m_shamt = shamt;
	};
	R(){};
};

class I : public Instruction
{
public:
	int *m_rs, *m_rd, m_immediate;
	I(int *rs, int *rd, int immediate)
	{

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

	J(int address)
	{

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
		m_shamt = 0;
	}
	ADD(int *rs, int *rt, int *rd)
	{
		m_shamt = 0;
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
		m_shamt = 0;
	}

	SUB(int *rs, int *rt, int *rd)
	{
		m_shamt = 0;
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
		m_shamt = 0;
	}

	MUL(int *rs, int *rt, int *rd)
	{
		m_shamt = 0;
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
		m_shamt = 0;
	}

	DIV(int *rs, int *rt, int *rd)
	{
		m_shamt = 0;
		m_rs = rs;
		m_rt = rt;
		m_rd = rd;
		m_duration = 4;
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
		m_shamt = 0;
	}

	MOD(int *rs, int *rt, int *rd)
	{
		m_shamt = 0;
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
		m_rs = 0;
	}

	SLL(int *rt, int shamt, int *rd)
	{
		m_shamt = shamt;
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
		m_rs = 0;
	}

	SRL(int *rt, int shamt, int *rd)
	{
		m_shamt = shamt;
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
		m_shamt = 0;
	}

	AND(int *rs, int *rt, int *rd)
	{
		m_shamt = 0;
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
		m_shamt = 0;
	}

	ORR(int *rs, int *rt, int *rd)
	{
		m_shamt = 0;
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
		m_shamt = 0;
	}

	NOR(int *rs, int *rt, int *rd)
	{
		m_shamt = 0;
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
		m_shamt = 0;
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
	}
	ADDI(int *rs, int *rd, int immediate)
	{
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
	}
	SUBI(int *rs, int *rd, int immediate)
	{
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
	}
	MULI(int *rs, int *rd, int immediate)
	{
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
	}
	ANDI(int *rs, int *rd, int immediate)
	{
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
	}
	ORI(int *rs, int *rd, int immediate)
	{
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
	}
	DIVI(int *rs, int *rd, int immediate)
	{
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
	}
	MODI(int *rs, int *rd, int immediate)
	{
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
	}
	SLTI(int *rs, int *rd, int immediate)
	{
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
	}
	JI(int address)
	{
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
	}
	JR(int *rs)
	{
		m_rs = rs;
	}
	int run()
	{
		PC = *m_rs;
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
