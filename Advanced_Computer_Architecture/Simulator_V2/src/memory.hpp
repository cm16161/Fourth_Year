#ifndef MEMORY_HPP
#define MEMORY_HPP
#define MEM_SIZE 128
#include <iostream>

class MEM
{
	int memory[MEM_SIZE];
	MEM();

public:
	static MEM &getInstance();
	int LOAD(int index);
	void STORE(int value, int index);
	int ld(int *rs, int address);
	void st(int *rs, int address);
	void sti(int value, int address);
};

#endif
