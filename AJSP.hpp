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

#ifndef ARDUINO
#include <map>
#endif

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

			enum class Result: uint8_t
			{
					OK,
					DONE,
					INVALID_CHARACTER = 0x10,		//generic
					IC_STRING_START_EXPECTED,
					IC_ARRAY_COMMA_OR_END_EXPECTED,
					IC_ARRAY_VALUE_OR_END_EXPECTED,
					IC_ARRAY_VALUE_EXPECTED,
					IC_OBJECT_COLON_EXPECTED,
					IC_OBJECT_VALUE_EXPECTED,
					IC_OBJECT_KEY_OR_END_EXPECTED,
					IC_OBJECT_SEPARATOR_OR_END_EXPECTED,
					INVALID_INTERNAL_STATE = 0x80
			};

			static const char* getResultDescription(Result r);

			Result parse(char c);		//returns true when it's done
			void setListener(Listener* l);
			uint32_t getCurrentOffset() const {return offset;}
			const std::string& getLastKey() const {return lastKey;}

			bool done() {return stack.empty();}



			Result getLastResult() const {return result;}

		private:
	 		enum class State
			{
				NONE = 0,		//for anything that doesn't need state

				OBJECT_KEY_OR_END,		//
				OBJECT_COLON,
				OBJECT_VALUE,
				OBJECT_SEPARATOR_OR_END,

				ARRAY_VALUE_OR_END,
				ARRAY_SEPARATOR_OR_END,
				ARRAY_VALUE,

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

			bool 		checkRawChar(char c);

			void		reportErrorToParent(Result ec);

			Listener* 	listener = nullptr;

			std::string localBuffer;

			constexpr static const char* rootElementName = "root";

			std::string lastKey = rootElementName;
			uint32_t	offset = 0;
			Result   	result = Result::OK;

			std::stack<StackElement, std::vector<StackElement>> stack;

#ifndef ARDUINO
			void 	  printState(const std::string& msg) const;
			void 		printStack() const;
			static std::map<Entity, std::string> entityNames;

			static const char* getStateDescription(State& s);
#endif
	};
}


#endif /* AJSP_HPP_ */
