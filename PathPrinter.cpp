/*
 * PathPrinter.cpp
 *
 *  Created on: Jan 3, 2017
 *      Author: bbielaws
 */

#include "PathPrinter.hpp"

#include <iostream>
#include <algorithm>
#include <numeric>

using namespace AJSP;
using namespace std;



void PathPrinter::arrayStart()
{
	//cout << "AS" << stack.size() << endl;
	pathConstructor.push(parser->getLastKey());
}

void PathPrinter::arrayEnd()
{
	//cout << "AE" << stack.size() << endl;
	pathConstructor.pop();
}

void PathPrinter::objectStart()
{
	//cout << "OS" << stack.size() << endl;
	pathConstructor.push(parser->getLastKey());
}

void PathPrinter::objectEnd()
{
	//cout << "OE" << stack.size() << endl;
	pathConstructor.pop();
}

void PathPrinter::key(const std::string& key)
{

}

void PathPrinter::value(const std::string& value, Parser::Entity)
{
	cout << pathConstructor.getPath() << "/" << parser->getLastKey() << ": " << value << endl;
}


const std::string& PathPrinter::getCurrentPath()
{
	return pathConstructor.getPath();
}


void PathPrinter::done()
{

}
