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
	stack.emplace_back(parser->getLastKey());
}

void PathPrinter::arrayEnd()
{
	//cout << "AE" << stack.size() << endl;
	stack.pop_back();
}

void PathPrinter::objectStart()
{
	//cout << "OS" << stack.size() << endl;
	stack.emplace_back(parser->getLastKey());
}

void PathPrinter::objectEnd()
{
	//cout << "OE" << stack.size() << endl;
	stack.pop_back();
}

void PathPrinter::key(const std::string& key)
{

}

void PathPrinter::value(const std::string& value)
{
	//cout << "V" << stack.size() << endl;
	//append the last key here, don't push it on the stack and pop it...
	cout << getCurrentPath() << "/" << parser->getLastKey() << ": " << value << endl;
}


const std::string& PathPrinter::getCurrentPath()
{
	bufferedCurrentPath.clear();

	for (auto& e: stack)
	{
		bufferedCurrentPath.append(e);
		bufferedCurrentPath.append("/");
	}

	if (bufferedCurrentPath.size())
		bufferedCurrentPath.pop_back();

	return bufferedCurrentPath;
}


void PathPrinter::done()
{

}
