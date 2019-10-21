#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class Fetch
{
	Fetch(){};

public:
	static Fetch &getInstance();
	void getInstruction(string line, vector<string> *tokens);
	void getCode(string file_name, vector<string> *instructions);
	void getRegisters(string line, vector<int> *registers);
	int getImmediate(string line);
};
