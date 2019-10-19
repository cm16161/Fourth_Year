#include "memory.hpp"

MEM::MEM()
{
	{
		for (int i = 0; i < MEM_SIZE; i++)
		{
			memory[i] = i;
		}
	}
}

int MEM::LOAD(int index)
{
	return memory[index];
}
void MEM::STORE(int value, int index)
{
	memory[index] = value;
}
int MEM::ld(int *rs, int address)
{
	*rs = LOAD(address);
	return *rs;
}
void MEM::st(int *rs, int address)
{
	STORE(*rs, address);
}
