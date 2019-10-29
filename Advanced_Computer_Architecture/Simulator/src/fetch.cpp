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
