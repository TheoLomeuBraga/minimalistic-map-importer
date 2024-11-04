#pragma once
#include <fstream>
#include <vector>

bool read_file(char *path, std::vector<char> *buffer = NULL)
{
	std::ifstream file(path);
	if (!file.is_open())
	{
		return false;
	}

	if (buffer != NULL)
	{

		file.seekg(0, std::ios::end);
		std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);

		(*buffer).resize(size);
		if (!file.read((*buffer).data(), size))
		{
			return false;
		}
	}

	file.close();
	return true;
}