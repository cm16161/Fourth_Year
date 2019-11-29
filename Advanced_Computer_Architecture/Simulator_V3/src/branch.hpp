#include "ISA.hpp"

class BEQ : public I
{
public:
	BEQ(int *rs, int *rd, int immediate);
	int run();
};

class BNE : public I
{
public:
	BNE(int *rs, int *rd, int immediate);
	int run();
};
