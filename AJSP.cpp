/*
 * AJSP.cpp
 *
 *  Created on: Jan 2, 2017
 *      Author: bbielaws
 */

#include "AJSP.hpp"

#include <ctype.h>

//#define USE_ARDUINO

#ifdef USE_ARDUINO
#include <Arduino.h>
#else
#include <unistd.h>
#endif

using namespace AJSP;
using namespace std;

static std::string localToString(uint32_t v)
{
	char buffer[12];
	snprintf(buffer, 12,  "%d", v);
	return std::string(buffer);
}

AJSP::Parser::Parser()
{
	stack.emplace(Entity::VALUE, State::NONE);
	localBuffer.reserve(32);
}

void AJSP::Parser::setListener(Listener* l)
{
	listener = l;
}

void AJSP::Parser::reset()
{
	localBuffer.clear();
	lastKey = "root";
	offset = 0;

	stack.emplace(Entity::VALUE, State::NONE);
}

bool AJSP::Parser::skipWhitespace(char c) const
{
	auto& currentElement = stack.top();
	return isspace(c) && !(currentElement.entity == Entity::STRING && currentElement.state == State::STRING_BODY);
}


bool AJSP::Parser::parse(char c)
{
	if (!c)
		return true;

	offset++;

	if (skipWhitespace(c))
		return false;

	bool consumed = false;
	int tries = 1;

	do
	{
		auto& currentElement = stack.top();

		switch (currentElement.entity)
		{
			case Entity::OBJECT:
				consumed = parseObject(c);
				break;

			case Entity::ARRAY:
				consumed = parseArray(c);
				break;

			case Entity::VALUE:
				consumed = parseValue(c);
				break;

			case Entity::STRING:
			case Entity::KEY:
				consumed = parseString(c);
				break;

			case Entity::RAW:
				consumed = parseRaw(c);
				break;
		}
		if (consumed) tries--;
	} while (tries && stack.size() > 0);

	if (stack.empty() && listener)
		listener->done();

	return stack.empty();
}


//changes the VALUE entity from the top of the stack to the proper entity
bool AJSP::Parser::parseValue(char c)
{
	auto& currentElement = stack.top();

	if (currentElement.entity != Entity::VALUE)
		return false;

	if (c == '{')	//object - consumes the element
	{
		//NOTE: exit point
		if (listener) listener->objectStart();
		currentElement = StackElement(Entity::OBJECT, State::KV_OR_END);
		return true;
	}

	if (c == '[')	//array - consumes the char
	{
		//NOTE: exit point
		if (listener) listener->arrayStart();

		//set the new lastIndex to 0, the previous one should be saved in ARRAY's state
		//to be restored when we pop
		lastKey = "0";
		currentElement = StackElement(Entity::ARRAY, (State)0);
		stack.emplace(Entity::VALUE, State::NONE);
		return true;
	}

	if ((c == 'u') or (c == '\"') or (c == '\''))	//string
	{
		currentElement = StackElement(Entity::STRING, State::STRING_START);
		parseString(c);
		return true;
	}

	//let's see if we can handle this one with RAW entity (bool, null, numbers)
	currentElement = StackElement(Entity::RAW, State::NONE);
	localBuffer.clear();
	return parseRaw(c);
}

bool AJSP::Parser::parseString(char c)
{
	auto& currentElement = stack.top();

	if (!(currentElement.entity == Entity::STRING or currentElement.entity == Entity::KEY))
		return false;	//we're not parsing strings...?

	switch (currentElement.state)
	{
		case State::STRING_START:
			//we should skip 'u' that is at the beginning - u for unicode
			if (c == 'u')
				return true;
			if ((c == '\"') or (c == '\''))
			{
				currentElement.state = State::STRING_BODY;	//we're in the string
				localBuffer.clear();
				//localBuffer += c;
				return true;
			}

			reportErrorToParent(ErrorCode::STRING_START_EXPECTED);
			return false;

		case State::STRING_BODY:
			if ((c == '\"') or (c == '\''))		//end of string
			{
				//localBuffer += c;

				//NOTE: exit point
				bool isKey = currentElement.entity == Entity::KEY;

				if (isKey)
				{
					if (listener) listener->key(localBuffer);
					lastKey = localBuffer;
				}
				else
				{
					if (listener) listener->value(localBuffer);
				}

				stack.pop();
				return true;
			}

			if (c == '\\')
			{
				currentElement.state = State::STRING_ESCAPE;
				return true;
			}

			localBuffer += c;
			return true;

		case State::STRING_ESCAPE:
			switch (c)
			{
				case 'n': localBuffer += '\n'; break;
				case 'r': localBuffer += '\r'; break;
				case 't': localBuffer += '\t'; break;
				case '\\': localBuffer += '\\'; break;

				default:
					localBuffer += c;		//just put the raw value
			}

			currentElement.state = State::STRING_BODY;
			return true;

		default:;

	}

	reportErrorToParent(ErrorCode::INVALID_INTERNAL_STATE);
	return false;
}

bool AJSP::Parser::parseArray(char c)
{
	auto& currentElement = stack.top();
	if (currentElement.entity != Entity::ARRAY)
		return false;

	if (c == ',')
	{
		uint32_t newLastIndex = (uint32_t)currentElement.state;
		newLastIndex++;
		currentElement.state = (State)newLastIndex;
		lastKey = localToString(newLastIndex);
		stack.emplace(Entity::VALUE, State::NONE);
		return true;
	}
	if (c == ']')
	{
		//NOTE: exit point
		if (listener)
			listener->arrayEnd();
		stack.pop();
		return true;
	}

	//this should not happen
	//(all other values are handled by the item inside the array)
	reportErrorToParent(ErrorCode::INVALID_INTERNAL_STATE);
	return false;
}


bool		AJSP::Parser::parseObject(char c)
{
	auto& currentElement = stack.top();
	if (currentElement.entity != Entity::OBJECT)
		return false;

	switch (currentElement.state)
	{
		case State::KV_OR_END:
			if (c == '}')
			{
				//NOTE: exit point
				if (listener)
					listener->objectEnd();

				stack.pop();
				return true;
			}
			if (c == ',')
			{
				currentElement.state = State::KV_K;
				stack.emplace(Entity::KEY, State::STRING_START);
				return true;
			}

			//all the rest try to parse as a string start
			currentElement.state = State::KV_K;
			stack.emplace(Entity::KEY, State::STRING_START);
			return parseString(c);

		case State::KV_K:
			if (c == ':')
			{
				currentElement.state = State::KV_OR_END;
				stack.emplace(Entity::VALUE, State::NONE);
				return true;
			}

			//no break;
		case State::INVALID:		//no break;
		default:;					//no break;
	}

	reportErrorToParent(ErrorCode::INVALID_CHARACTER);
	return false;
}

bool		AJSP::Parser::parseRaw(char c)
{
	auto& currentElement = stack.top();
	if (currentElement.entity != Entity::RAW)
		return false;

	/*
	 * FIXME:
	 * currently the code will accept any input that consists of these characters
	 * it could be fixed to be able to invalid sequences:
	 * -multiple dots
	 * -multiple exponents
	 * -+- signs not at the begining or not after e/E
	 * -dot at the end
	 * -... and probably many more
	 */

	if (isalnum(c) || c == '-' || c == '+' || c == '.')
	{
		localBuffer += c;
		return true;
	}

	//if we already had something in the buffer then it's the end of the token
	if (localBuffer.length() && listener)
	{
		//NOTE: exit point
		listener->value(localBuffer);
	}
	stack.pop();
	return false;
}


void AJSP::Parser::reportErrorToParent(ErrorCode ec)
{
	errorCode = ec;
	if (stack.size() == 0)
		return;

	stack.pop();
	stack.top().state = State::INVALID;
}

//not used yet
void AJSP::Parser::unwindStack()
{
	while (!stack.empty()) stack.pop();
	stack.emplace(Entity::VALUE, State::NONE);
	localBuffer.clear();
}
