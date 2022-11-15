#include <chrono>
#include <iostream>
#include <random>
#include "Utility.h"

namespace util
{
    // Basic stream class for getting data from a CSV file
    CSV::CSV(const string& filename)
        : file(filename)
    {}
    void CSV::check_newline()
    {
        if (file.peek() == '\n')
            throw std::runtime_error{ "CSV caught a newline. Use endline() at the end of a data line." };
    }
    void CSV::skip_comment()
    {
        check_newline();
        while (file && file.peek() == '#')
            file.ignore(LIMIT, '\n');
    }
    void CSV::skip_cell()
    {
        file.ignore(LIMIT, ',');
    }
    void CSV::open(const string& filename)
    {
        if (file.is_open())
            file.close();
        file.open(filename);
    }
    bool CSV::good() const
    {
        return file.good();
    }
    bool CSV::eof() const
    {
        return file.eof();
    }
    void CSV::endline()
    {
        file.clear();
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        skip_comment();
    }
    CSV& operator>>(CSV& csv, int& n)
    {
        csv.skip_comment();

        csv.file >> n;
        csv.skip_cell();

        return csv;
    }
    CSV& operator>>(CSV& csv, double& n)
    {
        csv.skip_comment();

        csv.file >> n;
        csv.skip_cell();

        return csv;
    }
    CSV& operator>>(CSV& csv, string& str)
    {
        csv.skip_comment();

        std::getline(csv.file >> std::ws, str, ',');

        return csv;
    }
    CSV& operator>>(CSV& csv, bool& b)
    {
        csv.skip_comment();

        string str{};
        csv >> str;
        if (str == "TRUE")
            b = true;
        else if (str == "FALSE")
            b = false;
        else
            csv.file.setstate(std::ios::failbit);

        return csv;
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