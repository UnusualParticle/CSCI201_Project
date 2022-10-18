#include "Utility.h"

namespace util
{
	void NameVector::add(const string& name)
	{
		auto pt{ std::find(v.begin(), v.end(), name) };
		if (pt != v.end())
			throw std::range_error{ "Name already exists" };

		v.push_back(name);
	}

	NameVector::NameVector(const string& filename)
	{
		loadFromFile(filename);
	}

	// Clears the vector and adds data line by line to the vector
	void NameVector::loadFromFile(const string& filename)
	{
		v.clear();
		std::ifstream file{ filename };

		while (file)
		{
			string name;
			std::getline(file, name);
			if (name.size() != 0)
				add(name);
		}
	}

	// Get the name associated with an ID
	const string& NameVector::getName(int id) const
	{
		return v.at(id);
	}

	// Get the ID associated with a name
	int NameVector::getID(const string& name) const
	{
		auto ptr{ std::find(v.begin(), v.end(), name) };
		if (ptr == v.end())
			throw std::invalid_argument{ "Name does not exist" };
		return 
	}

	// My basic string prompter
	string promptstr(const string& prompt)
	{
        // Get the value
        string str{};
        std::cout << prompt;
        std::getline(std::cin, str);

        // If the value is empty, try again
        while (str.size() == 0)
        {
            std::cout << "String cannot be empty\n" << prompt;
            std::getline(std::cin, str);
        }

        // Return the value
        return str;
	}

	// My basic int prompter
	int promptint(const string& prompt)
	{
        // Get the value
        int value{};
        std::cout << prompt;
        std::cin >> value;

        // If the input failed, clear the stream and try again
        while (!std::cin.good())
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Must enter a number\n" << prompt;
            std::cin >> value;
        }

        // Ignore whatever else was in the stream
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        // Return the value
        return value;
	}

	// My basic char prompter
	char promptchar(const string& prompt)
	{
        // Get the value
        char value;
        std::cout << prompt;
        std::cin >> value;

        // Ignore whatever is left in the stream
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        // Return the value
        return value;
	}
}