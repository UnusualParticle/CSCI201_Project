#include <chrono>
#include <iostream>
#include <random>
#include "Utility.h"

namespace util
{
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

    std::istream& getline(std::istream& stream, string& str, char delim)
	{
		return std::getline(stream >> std::ws, str, delim);
	}

    int randint(int low, int high)
    {
        static std::mt19937 mt{ static_cast<unsigned int>(std::chrono::steady_clock::now().time_since_epoch().count()) };
        std::uniform_int_distribution distr{ low, high };
        return distr(mt);
    }
}