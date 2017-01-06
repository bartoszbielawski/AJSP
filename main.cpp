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

using namespace std;
using namespace AJSP;

int main(int argc, char* argv[])
{
	if (argc < 2)
		return 1;

	try
	{
		Parser p;
		PathPrinter pp(&p);

		p.setListener(&pp);

		std::ifstream ifs (argv[1], std::ifstream::in);

		if (!ifs.good())
		{
			cerr << "#Error, failed to open file..." << endl;
			return 2;
		}

		using Result = Parser::Result;

		while (ifs.good())
		{
			char c = ifs.get();
			auto r = p.parse(c);
			if (r != Result::OK)
			{
				if (r == Result::DONE)
					break;

				cerr << "#Error: " << p.getResultDescription(r) << " at offset " << p.getCurrentOffset() << endl;
				break;
			}
		};


		ifs.close();
	}
	catch(const std::exception& e)
	{
		cout << e.what() << endl;
	}

	return 0;
}


