/*
 * AJSP.hpp
 *
 *  Created on: Jan 2, 2017
 *      Author: bbielaws
 */

#ifndef AJSP_HPP_
#define AJSP_HPP_

#include <string>
#include <stack>
#include <utility>
#include <vector>

namespace AJSP
{
	class Parser;

	class Listener
	{
		public:
			Listener(Parser* p): parser(p) {}
			virtual ~Listener() {}

			virtual void arrayStart() = 0;
			virtual void arrayEnd() = 0;

			virtual void objectStart() = 0;
			virtual void objectEnd() = 0;

			virtual void key(const std::string& key) = 0;
			virtual void value(const std::string& value) = 0;

			virtual void done() = 0;
		protected:
			Parser* parser;
	};

	class Parser
	{
	public:
		enum class State: uint16_t
		{
			NONE = 0,

			KV_OR_END,
			KV_K,
			KV_S,
			KV_V,

			STRING_START,
			STRING_BODY,
			STRING_ESCAPE,

			INVALID = 0xFFFF
		};

		enum class Entity: uint16_t
		{
			OBJECT,
			ARRAY,
			KEY,
			VALUE,
			STRING,
			RAW
		};

		struct StackElement
		{
			StackElement(Entity e, State s): entity(e), state(s) {}
			Entity entity;
			State state;
		};


			Parser();
			~Parser() {}

			void reset();
			void parse(char c);
			void setListener(Listener* l);
			uint32_t getCurrentOffset() const {return offset;}
			const std::string& getLastKey() const {return lastKey;}

		private:
			bool 		skipWhitespace(char c) const;

			bool 		parseValue(char c);
			bool		parseString(char c);
			bool		parseKeyValue(char c);
			bool		parseObject(char c);
			bool		parseArray(char c);
			bool		parseRaw(char c);

			void		unwindStack();
			void 		printStack();
			void		reportErrorToParent();

			std::string printStackElement(StackElement& e);

			Listener* 	listener;

			std::string localBuffer;
			std::string lastKey;
			State		state;
			uint32_t	offset;

			std::stack<StackElement, std::vector<StackElement>> stack;
	};
}


#endif /* AJSP_HPP_ */
