#include "branch.hpp"

bool branch_taken = false;

BEQ::BEQ(int *rs, int *rd, int immediate)
{
	m_rs = rs;
	m_rd = rd;
	m_immediate = immediate;
}
int BEQ::run()
{
	if (*m_rs == *m_rd)
	{
		PC = m_immediate - 1;
		branch_taken = true;
	}
	return PC;
}

BNE::BNE(int *rs, int *rd, int immediate)
{
	m_rs = rs;
	m_rd = rd;
	m_immediate = immediate;
}
int BNE::run()
{
	if (*m_rs != *m_rd)
	{
		PC = m_immediate - 1;
		branch_taken = true;
	}
	return PC;
}
