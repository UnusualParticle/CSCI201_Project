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
    // Choice prompter for an int range, [low, high]
    int promptchoice(int low, int high)
    {
        int value{ promptint("> ") };
        while (value < low || high < value)
        {
            std::cout << "Not a valid option\n";
            value = promptint("> ");
        }
        return value;
    }
    // Int-based yes/no prompt
    bool promptyn()
    {
        std::cout << "\n\t1. Yes" << "\n\t2. No\n";
        return promptchoice(1, 2) == 1;
    }

    std::istream& getline(std::istream& stream, string& str, char delim)
	{
		return std::getline(stream >> std::ws, str, delim);
	}

    int randint(int low, int high)
    {
        std::uniform_int_distribution distr{ low, high };
        return distr(randengine);
    }

    void Timer::start(int ms)
    {
        setpoint = clock.now() + std::chrono::milliseconds(ms);
    }
    bool Timer::isDone() const
    {
        return clock.now() > setpoint;
    }

    Counter::Counter(int target)
        : m_target(target)
    {}
    bool Counter::done() const { return m_current >= m_target; }
    void Counter::reset() { m_current = 0; }
    Counter& Counter::operator++() { ++m_current; return *this; }
}