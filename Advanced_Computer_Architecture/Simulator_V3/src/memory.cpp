#include "memory.hpp"

MEM::MEM()
{
	for (int i = 0; i < MEM_SIZE; i++)
	{
		memory[i] = i;
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
	std::cout << " [ LD ] " << *rs << std::endl;
	return *rs;
}
void MEM::st(int *rs, int address)
{
	STORE(*rs, address);
	std::cout << " [ ST ] " << std::endl;
}

MEM &MEM::getInstance()
{
	static MEM mem;
	return mem;
}
