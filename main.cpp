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

int main(int argc, char* argv[])
{
	if (argc < 2)
		return 1;

	try
	{
		AJSP::Parser p;
		PathPrinter pp(&p);

		p.setListener(&pp);

		std::ifstream ifs (argv[1], std::ifstream::in);

		if (!ifs.good())
		{
			cerr << "#Error, failed to open file..." << endl;
			return 2;
		}

		while (ifs.good())
		{
			char c = ifs.get();
			p.parse(c);
		};

		ifs.close();
	}
	catch(const std::exception& e)
	{
		cout << e.what() << endl;
	}

	return 0;
}


