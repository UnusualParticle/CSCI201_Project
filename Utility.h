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
	// NameMap stores strings and a second data type, intended for enums
	class NameVector
	{
	private:
		std::vector<string> v;

		void add(const string& name);
	public:
		NameVector() = default;
		NameVector(const string& filename);
		~NameVector() = default;

		// Clears the vector and adds data line by line to the vector
		void loadFromFile(const string& filename);

			// Get the name associated with an ID
			const string& getName(int id) const;

			// Get the ID associated with a name
			int getID(const string& name) const;
	};

	// My basic string prompter
	string promptstr(const string& prompt);
	// My basic int prompter
	int promptint(const string& prompt);
	// My basic char prompter
	char promptchar(const string& prompt);
}