#include "fetch.hpp"

Fetch &Fetch::getInstance()
{
	static Fetch fetch;
	return fetch;
}

void Fetch::getInstruction(string line, vector<string> *tokens)
{
	tokens->clear();
	istringstream iss(line);
	copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter(*tokens));
}

void Fetch::getCode(string file_name, vector<string> *instructions)
{
	//ifstream file("../apps/alu_processes.txt");
	// ifstream file("../apps/ld_str.txt");
	ifstream file(file_name.c_str());
	if (file.is_open())
	{
		string line;
		while (getline(file, line))
		{
			instructions->push_back(line);
		}
		file.close();
	}
}

void Fetch::getRegisters(string line, vector<int> *registers)
{
	registers->clear();
	char *my_line = const_cast<char *>(line.c_str());
	char *token = strtok(my_line, "$");
	while (token != nullptr)
	{
		token = strtok(NULL, "$");
		if (token != nullptr)
		{
			registers->push_back(atoi(token));
		}
	}
}

int Fetch::getImmediate(string line)
{
	char *my_line = const_cast<char *>(line.c_str());
	char *token = strtok(my_line, "#");
	while (token != nullptr)
	{
		token = strtok(nullptr, "#");

		if (token != nullptr)
		{
			return atoi(token);
		}
	}
	return 0;
}
