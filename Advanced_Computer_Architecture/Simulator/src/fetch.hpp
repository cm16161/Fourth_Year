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
	vector<string> getInstruction(string line);
	void getCode(string file_name, vector<string> *instructions);
};
