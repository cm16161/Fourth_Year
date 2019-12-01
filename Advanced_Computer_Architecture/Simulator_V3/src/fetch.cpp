#include "fetch.hpp"

Fetch &Fetch::getInstance()
{
	static Fetch fetch;
	return fetch;
}

vector<string> Fetch::getInstruction(string line)
{
	vector<string> my_tokens;
	istringstream iss(line);
	copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter(my_tokens));
	return my_tokens;
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
