#pragma once

#include <algorithm>
#include <exception>
#include <fstream>
#include <string>
#include <vector>
using string = std::string;

/*
* This file contains basic utility functions and classes that may be used throughout the project
*/

namespace util
{
	// NameVector stores strings, intended for enums
	class NameVector
	{
	private:
		std::vector<string> v;

		void add(const string& name);
	public:
		NameVector() = default;
		NameVector(const string& title);
		~NameVector() = default;

		// Get the name associated with an ID
		const string& getName(int id) const;

		// Get the ID associated with a name
		int getID(const string& name) const;
	};

	template<typename data_t>
	// DataList stores a data type
	class DataVector
	{
	private:
		std::vector<string> v;
	public:
		DataVector() = default;
		DataVector(const string& filename)
		{
			loadFromFile(filename);
		}
		~DataVector() = default;

		// Clears the vector and adds data
		void loadFromFile(const string& filename)
		{
			std::ifstream file{ filename };
			while (file)
			{
				data_t data;
				file >> data;
				if (file.fail())
					throw std::invalid_argument{ "Input file not formatted correctly" };
				add(data);
			}

		}
		data_t& getdata(int id);
		data_t& getdatabyname(const string& name);
	};

	// My basic string prompter
	string promptstr(const string& prompt);
	// My basic int prompter
	int promptint(const string& prompt);
	// My basic char prompter
	char promptchar(const string& prompt);

	// Getline without leading whitepace
	void getline(std::istream& stream, string& str, char delim = '\n');
}