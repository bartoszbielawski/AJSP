/*
 * PathListener.h
 *
 *  Created on: 04.01.2017
 *      Author: caladan
 */

#ifndef PATHLISTENER_H_
#define PATHLISTENER_H_

#include "AJSP.hpp"
#include "PathConstructor.h"

class PathListener: public AJSP::Listener {
	public:
		using Callback = void (*)(const std::string& path, const std::string& value);

		PathListener(AJSP::Parser* p);
		virtual ~PathListener();

		void clear();

		virtual void arrayStart();
		virtual void arrayEnd();

		virtual void objectStart();
		virtual void objectEnd();

		virtual void key(const std::string& key);
		virtual void value(const std::string& value);

		virtual void done();

		void setCallback(Callback callback) {this->callback = callback;}

		std::vector<const char*> &monitoredPaths() {return _monitoredPaths;}
	private:
		PathConstructor pathConstructor;
		std::vector<const char*> _monitoredPaths;
		Callback callback = nullptr;
};

#endif /* PATHLISTENER_H_ */
