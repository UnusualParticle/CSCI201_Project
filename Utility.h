#ifndef UTILITY_H
#define UTILITY_H

#include <algorithm>
#include <array>
#include <chrono>
#include <exception>
#include <fstream>
#include <map>
#include <random>
#include <string>
#include <vector>
using string = std::string;

/*
* This file contains basic utility functions and classes that may be used throughout the project
*/

namespace util
{
	inline const auto STREAMMAX{ std::numeric_limits<std::streamsize>::max() };
	
	// Basic stream class for getting data from a CSV file
	class CSV
	{
	private:
		// Stream limit
		static const auto LIMIT{ std::numeric_limits<std::streamsize>::max() };
		// Throw an error if a newline character is encountered
		void check_newline();
		// Check for comments and skip them
		void skip_comment();
		// Skip to the next cell
		void skip_cell();
		// Private file stream
		std::ifstream file{};
	public:
		CSV(const string& filename);

		void open(const string& filename);
		bool good() const;
		bool eof() const;
		void endline();

		friend CSV& operator>>(CSV& csv, int& n);
		friend CSV& operator>>(CSV& csv, double& n);
		friend CSV& operator>>(CSV& csv, string& str);
		friend CSV& operator>>(CSV& csv, bool& b);
	};

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
			std::ifstream file{ "Data/namelists.txt" };
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
				throw std::range_error{ "Name <" + name + "> does not exist" };
			return ptr->second;
		}

		// Get the name associated with an ID
		const string& getName(int id) const
		{
			auto ptr{ std::find_if(mp.begin(), mp.end(), [id](const pair_t& p) {return p.second == id; }) };
			if (ptr == mp.end())
				throw std::range_error{ "ID <"+std::to_string(id)+"> does not exist" };
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
	public:
		using const_iterator = std::vector<data_t>::const_iterator;
		using iterator = std::vector<data_t>::iterator;
		using reverse_iterator = std::vector<data_t>::reverse_iterator;
	private:
		std::vector<data_t> v;
	public:
		DataVector() = default;
		DataVector(const string& filename, std::vector<string>& errlist)
		{
			loadFromFile(filename, errlist);
		}
		~DataVector() = default;

		// Clears the vector and adds data
		void loadFromFile(const string& filename, std::vector<string>& errlist)
		{
			util::CSV csv{ filename };
			while (!csv.eof())
			{
				try {
					data_t data{};
					csv >> data;
					v.push_back(data);
					if (csv.eof())
						break;
				}
				catch (std::exception& e)
				{
					errlist.push_back(e.what());
					csv.endline();
				}
			}
		}
		const data_t& getdata(int id)
		{
			return v[id];
		}
		const_iterator getdatabyname(const string& name)
		{
			auto ptr{ std::find_if(v.begin(), v.end(), [name](const data_t& data) {return data.getName() == name; })};
			if (ptr == v.end())
				throw std::range_error{ "Name <" + name + "> is not in data" };
			return ptr;
		}

		// For use in a range-based for loop
		const_iterator begin() const { return v.begin(); }
		const_iterator end() const { return v.end(); }
		iterator begin() { return v.begin(); }
		iterator end() { return v.end(); }
		reverse_iterator rbegin() { return v.rbegin(); }
		reverse_iterator rend() { return v.rend(); }

		size_t size() { return v.size(); }
	};

	// My basic string prompter
	string promptstr(const string& prompt);
	// My basic int prompter
	int promptint(const string& prompt);
	// My basic char prompter
	char promptchar(const string& prompt);
	// Choice prompter for an int range, [low, high]
	int promptchoice(int low, int high);
	// Int-based yes/no prompt (returns yes:true or no:false)
	bool promptyn();


	// Getline without leading whitepace
	std::istream& getline(std::istream& stream, string& str, char delim = '\n');

	// Generate a random int, inclusive
	inline std::mt19937 randengine{ static_cast<unsigned int>(std::chrono::steady_clock::now().time_since_epoch().count()) };
	int randint(int low, int high);

	class Timer
	{
	private:
		static std::chrono::steady_clock clock;
		std::chrono::time_point<std::chrono::steady_clock> setpoint;
	public:
		void start(int milliseconds);
		bool isDone() const;
	};
	class Counter
	{
	private:
		int m_target;
		int m_current{};
	public:
		Counter(int target);
		bool done() const;
		void reset();
		Counter& operator++();
	};
}

#endif