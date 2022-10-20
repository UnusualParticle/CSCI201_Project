#pragma once

#include <algorithm>
#include <array>
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
	
	template<size_t n> class NameArray;
	template<typename id_t> class NameMap;

	template<size_t n>
	// NameArray stores strings, intended for enums
	class NameArray
	{
	private:
		std::array<string, n> arr{};
		using const_iterator = std::array<string, n>::const_iterator;
	public:
		NameArray() = default;
		~NameArray() = default;

		// Get the name associated with an ID
		const string& getName(int id) const
		{
			return arr.at(id);
		}

		// For use in a range-based for loop
		const const_iterator begin() const { return arr.begin(); }
		const const_iterator end() const { return arr.end(); }
		
		// Just some friend template function fun, don't mind me
		template<typename id_t>
		template<size_t size>
		friend void NameMap<id_t>::exportToArray(NameArray<size>& names) const;
	};

	template<size_t size>
	bool verifyNameArray(const NameArray<size>& arr)
	{
		auto ptr{ std::find(arr.begin(), arr.end(), "") };
		return ptr == arr.end();
	}

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
		template<size_t size>
		void exportToArray(NameArray<size>& names) const
		{
			for(const auto& p : mp)
				names.arr.at(p.second) = p.first;
		}
	};

	template<typename data_t>
	// DataList stores a data type
	class DataVector
	{
	private:
		std::vector<data_t> v;
		using const_iterator = std::vector<data_t>::const_iterator;
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
				if (file.eof())
					break;
				if (file.fail())
					throw std::invalid_argument{ "Input file not formatted correctly" };
				v.push_back(data);
			}
		}
		const data_t& getdata(int id)
		{
			return v[id];
		}
		const_iterator getdatabyname(const string& name)
		{
			auto ptr{ std::find_if(v.begin(), v.end(), [name](const data_t& data) {return data.name == name; }) };
			if (ptr == v.end())
				throw std::invalid_argument{ "Name <" + name + "> is not in data" };
			return ptr;
		}

		// For use in a range-based for loop
		const const_iterator begin() const { return v.begin(); }
		const const_iterator end() const { return v.end(); }
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