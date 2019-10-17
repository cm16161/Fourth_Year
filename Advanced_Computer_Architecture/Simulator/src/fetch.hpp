#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <sstream>
#include <cstring>

using namespace std;

class Fetch
{
public:
	void getInstruction(string line, vector<string> *tokens);
	void getCode(vector<string> *instructions);
	void getRegisters(string line, vector<int> *registers);
	int getImmediate(string line);
};
