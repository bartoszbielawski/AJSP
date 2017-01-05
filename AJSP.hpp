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
		Parser();
		~Parser() {}

		void reset();
		bool parse(char c);		//returns true when it's done
		void setListener(Listener* l);
		uint32_t getCurrentOffset() const {return offset;}
		const std::string& getLastKey() const {return lastKey;}

		bool done() {return stack.empty();}

		enum class ErrorCode
		{
			OK,
			STRING_START_EXPECTED,
			INVALID_INTERNAL_STATE,
			INVALID_CHARACTER
		};

		ErrorCode getErrorCode() const {return errorCode;}

	private:
		enum class State: uint16_t
		{
			NONE = 0,		//for anything that doesn't need state

			KV_OR_END,		//for objects with key-value paris
			KV_K,
			KV_S,
			KV_V,

			STRING_START,	//for strings and keys
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

		bool 		skipWhitespace(char c) const;

		bool 		parseValue(char c);
		bool		parseString(char c);
		bool		parseObject(char c);
		bool		parseArray(char c);
		bool		parseRaw(char c);

		void		unwindStack();
		void		reportErrorToParent(ErrorCode ec);

		Listener* 	listener = nullptr;

		std::string localBuffer;
		std::string lastKey = "root";
		uint32_t	offset = 0;
		ErrorCode   errorCode = ErrorCode::OK;

		std::stack<StackElement, std::vector<StackElement>> stack;
};
}


#endif /* AJSP_HPP_ */
