/*
 * main.cpp
 *
 *  Created on: Jan 2, 2017
 *      Author: bbielaws
 */

#include <fstream>
#include <iostream>

#include "AJSP.hpp"
#include "Printer.hpp"
#include "PathPrinter.hpp"

#include <utility>

using namespace std;
using namespace AJSP;


struct Result
{
		Result(AJSP::Parser::Result result, uint32_t offset): result(result), offset(offset) {}
		AJSP::Parser::Result result;
		uint32_t offset;

		bool operator==(const Result& other) const
		{
			if (result == Parser::Result::OK and other.result == Parser::Result::OK)
				return true;

			if (result != other.result)
				return false;

			if (offset != other.offset)
				return false;

			return true;
		}

		bool operator!=(const Result& other) const {return !(*this == other);}

		std::string toString() const
		{
			return string(Parser::getResultDescription(result)) + " @ " + to_string(offset);
		}
};


Result parseFile(const std::string& filename)
{
		AJSP::Parser p;
		std::ifstream ifs (filename, std::ifstream::in);

		if (!ifs.good())
		{
			return Result(Parser::Result::INVALID_INTERNAL_STATE, 0xFFFFFFFF);
		}

		while (ifs.good())
		{
			auto r = p.parse(ifs.get());
			if (r == Parser::Result::DONE)
				break;

			if (r != Parser::Result::OK)
			{
				return Result(r, p.getCurrentOffset());
			}
		};

		ifs.close();

		return Result(Parser::Result::OK, 0);
	}

vector<pair<string, Result>> tests =
{
		{"jsonExamples/weatherExample.json", Result(Parser::Result::OK, 0)}
};

int main(int argc, char* argv[])
{

	for (const auto& p: tests)
	{
			auto r = parseFile(p.first);

			cout << "Testing file: " << p.first << endl;
			if (r != p.second)
			{
					cout << "Expected: " << p.second.toString() << endl;
					cout << "Result:   " << r.toString() << endl;
			}
			else
					cout << "OK" << endl;
	}

	return 0;
}
