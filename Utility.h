#pragma once

#include <algorithm>
#include <exception>
#include <fstream>
#include <map>
#include <string>
#include <vector>
using string = std::string;

/*
* This file contains basic utility functions and classes that may be used throughout the project
*/

namespace util
{
	inline const auto STREAMMAX{ std::numeric_limits<std::streamsize>::max() };

	template<size_t n>
	// NameArray stores strings, intended for enums
	class NameArray
	{
	private:
		std::array<string, n> arr;
		using const_iterator = std::array<string, n>::const_iterator;
	public:
		NameArray() = default;
		~NameArray() = default;

		// Get the name associated with an ID
		const string& getName(int id) const
		{
			return arr.at(id);
		}

		const const_iterator begin() const { return arr.begin(); }
		const const_iterator end() const { return arr.end(); }
	};

	template<typename id_t>
	// NameMap sores strings and ids, intended for enums
	class NameMap
	{
	private:
		using pair_t = std::pair<string, id_t>;
		std::map<string, id_t> mp;

		void add(const string& name, id_t id)
		{
			auto ptr{ mp.find(name)};
			if (ptr != mp.end())
				throw std::range_error{ "Name already exists" };

			mp.insert({ name, id });
		}
	public:
		NameMap() = default;
		NameMap(const string& title)
		{
			std::ifstream file{ "namelists.txt" };
			string temp;

			// Look for the desired list
			while (temp != title)
			{
				if (!file.good())
					throw std::invalid_argument{ "File formatting wrong" };
				file.ignore(STREAMMAX, '[');
				file.ignore(STREAMMAX, '#');
				getline(file >> std::ws, temp);
			}

			// List found, Get data
			int i{};
			while (file.peek() != ']')
			{
				getline(file >> std::ws, temp);
				add(temp, (id_t)i);
				++i;
			}
		}
		~NameMap() = default;

		// Get the ID associated with a name
		id_t getID(const string & name) const
		{
			auto ptr{ mp.find(name) };
			if (ptr == mp.end())
				throw std::range_error{ "Name does not exist" };
			return ptr->second;
		}

		// Get the name associated with an ID
		const string& getName(int id) const
		{
			auto ptr{ std::find_if(mp.begin(), mp.end(), [id](const pair_t& p) {return p.second == id; }) };
			if (ptr == mp.end())
				throw std::range_error{ "ID does not exist" };
			return ptr->first;
		}

		// Export a NameArray for use in the program
		template<int size>
		void exportToArray(NameArray<size> names) const
		{
			for(const auto& p : mp)
				names[p.second] = p.first;
			return names;
		}
	};

	template<typename data_t>
	// DataList stores a data type
	class DataVector
	{
	private:
		std::vector<data_t> v;
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
				data_t data{};
				file >> data;
				if (file.fail() && !file.eof())
					throw std::invalid_argument{ "Input file not formatted correctly" };
				v.push_back(data);
			}
		}
		const data_t& getdata(int id)
		{
			return v[id];
		}
		const data_t* getdatabyname(const string& name)
		{
			return std::find_if(v.begin(), v.end(), [name](const data_t& data) {return data.name == name; });
		}
	};

	// My basic string prompter
	string promptstr(const string& prompt);
	// My basic int prompter
	int promptint(const string& prompt);
	// My basic char prompter
	char promptchar(const string& prompt);

	// Getline without leading whitepace
	std::istream& getline(std::istream& stream, string& str, char delim = '\n');
}