#pragma once
#include "Basic.h"
#include <deque>
#include <exception>
using std::exception;
using std::u16string;
using std::deque;
using std::array;
class Lexer {
public:
	struct LexerToken
	{
		enum class LexerType {
			Unknown,
			Keyword,
			SeparateOperatpor,
			Operator,
			Number,
			Space,
			String,
			Character,
			Identifier,
		};
		LexerType type = LexerType::Unknown;
		u16string content;
		source_t source = source_t{ 0, 0 };
	};
private:
	deque<LexerToken>parsed;
	using byte = unsigned char;
	int mark_type(char ch);
	LexerToken move_next(const u16string& code, size_t& position, size_t line);
public:
	deque<LexerToken> parse(const deque<u16string>& code);
};
