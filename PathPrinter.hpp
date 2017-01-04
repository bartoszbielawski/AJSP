/*
 * PathPrinter.hpp
 *
 *  Created on: Jan 3, 2017
 *      Author: bbielaws
 */

#ifndef PATHPRINTER_HPP_
#define PATHPRINTER_HPP_

#include "AJSP.hpp"

class PathPrinter: public AJSP::Listener
{
	public:
		PathPrinter(AJSP::Parser* p): AJSP::Listener(p) {}
		virtual ~PathPrinter() {}

		virtual void arrayStart();
		virtual void arrayEnd();

		virtual void objectStart();
		virtual void objectEnd();

		virtual void key(const std::string& key);
		virtual void value(const std::string& value);

		virtual void done();


	private:
		const std::string& getCurrentPath();

		std::string bufferedCurrentPath;

		std::vector<std::string> stack;
};

#endif /* PATHPRINTER_HPP_ */
