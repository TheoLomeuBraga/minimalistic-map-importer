#pragma once
#include <fstream>
#include <vector>
#include <iostream>

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

bool read_map_file(char *path, std::vector<char> *buffer = NULL)
{
	std::ifstream file(path);
	if (!file.is_open())
	{
		return false;
	}

	if (buffer != NULL)
	{

		buffer->clear();
		std::string line;


		while (std::getline(file, line))
		{

			if (line.empty() || line[0] == '#' || line[0] == '/')
			{
				continue;
			}

			buffer->insert(buffer->end(), line.begin(), line.end());
			buffer->push_back('\n');
		}
	}

	file.close();
	return true;
}