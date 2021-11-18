#include "Lexer.h"
using namespace std;
int Lexer::mark_type(char ch) {
	if (ch == '_')				return (byte)0;
	if (ch >= '!' && ch <= '/')	return (byte)1;
	if (ch >= ':' && ch <= '@')	return (byte)1;
	if (ch >= '[' && ch <= '^')	return (byte)1;
	if (ch >= '{' && ch <= '~')	return (byte)1;
	if (ch == '`')				return (byte)1;
	if (ch == '\r' || ch == '\t' || ch == '\n' || ch == ' ')return (byte)2;
	return (byte)0;
}
Lexer::LexerToken Lexer::move_next(const u16string& code, size_t& position, size_t line) {
	if (parsed.size())
	{
		LexerToken token = parsed.front();
		parsed.pop_front();
		return token;
	}
	const static array<u16string, 25> TwoCharacterMark = { u"\\\\",u"\\\'",u"\\\"",u"//",u"/*",u"*/",u"=>",u"++",u"--",u"<< ",u">>",u"+=",u"-=",u"*=",u"/=",u"^=",u"|=",u"&=",u"%=",u"==",u">=",u"<=",u"!=",u"||",u"&&" };
	const static array<u16string, 2> ThreeCharacterMark = { u"<<=",u">>=" };
	const static array<u16string, 8> SeparateOperatpors = { u";",u",",u"{",u"}",u"(",u")",u"[",u"]" };
	int mk_type;
	mk_type = mark_type(code[position]);
	bool digit = isdigit(code[position]);
	size_t start = position;
	while (position < code.length() && (mark_type(code[position]) == mk_type
		|| digit && code[position] == '.'))
		position++;
	LexerToken ret;
	ret.source = source_t{ line, start };
	ret.content = code.substr(start, position - start);
	if (has(keywords, ret.content)) { ret.type = LexerToken::LexerType::Keyword; }
	else if (digit) {
		ret.type = LexerToken::LexerType::Number;
	}
	else if (mk_type == 1)
	{
		for (size_t i = start; i < position;)
		{
			if (i < position - 2 && has(ThreeCharacterMark, code.substr(i, 3))) {
				u16string token = code.substr(i, 3);
				parsed.push_back(LexerToken{ LexerToken::LexerType::Operator,token,array<size_t,2>{line,i} });
				i += 3;
			}
			else if (i < position - 1 && has(TwoCharacterMark, code.substr(i, 2))) {
				u16string token = code.substr(i, 2);
				parsed.push_back(LexerToken{ LexerToken::LexerType::Operator,token,array<size_t,2>{line,i} });
				i += 2;
			}
			else {
				u16string token = code.substr(i, 1);
				parsed.push_back(LexerToken{
					has(SeparateOperatpors,token) ? LexerToken::LexerType::SeparateOperatpor :
					LexerToken::LexerType::Operator,
					token,
					array<size_t,2>{line,i} });
				i += 1;
			}
		}
		if (parsed.size())
		{
			LexerToken token = parsed.front();
			parsed.pop_front();
			return token;
		}
		else throw exception("Lexer::move_next(): parsed.size() is 0 but mk_type is 1.");
	}
	else if (mk_type == 2) {
		ret.type = LexerToken::LexerType::Space;
	}
	else
	{
		ret.type = LexerToken::LexerType::Unknown;
	}
	return ret;
}
deque<Lexer::LexerToken> Lexer::parse(const deque<u16string>& code) {
	deque<LexerToken>ret;
	size_t pos;
	// 解析为Token
	for (size_t i = 0; i < code.size(); i++)
	{
		pos = 0;
		while (pos != code.at(i).length() || parsed.size() > 0)
		{
			ret.push_back(move_next(code.at(i), pos, i + 1));
		}
		ret.push_back(LexerToken{ LexerToken::LexerType::Space,u"\n",source_t{i + 1,code.at(i).length()} });
	}
	//删除注释 (字符串转义和处理、数字处理在Parser)
	size_t comment_now_line = UINT32_MAX;
	bool multi_line_comment = false;
	for (auto it = ret.begin(); it != ret.end();)
	{
		if (it->content == u"//") {
			comment_now_line = it->source[0];
			it = ret.erase(it);
		}
		else if (it->content == u"/*" && comment_now_line != it->source[0]) {
			multi_line_comment = true;
			it = ret.erase(it);
		}
		else if (it->content == u"*/") {
			multi_line_comment = false;
			it = ret.erase(it);
		}
		else if (it->content == u"\'") {
			u16string str;
			it = ret.erase(it);
			while (it->content != u"\'")
			{
				if (it->content == u"\\") {
					it = ret.erase(it);
					switch (it->content[0])
					{
					case u't':
						str.push_back(u'\t');
						break;
					case u'u':
						throw exception("未实现");
						break;
					case u'\\':
						str.push_back(u'\\');
						break;
					case u'a':
						str.push_back(u'\a');
						break;
					case u'\"':
						str.push_back(u'\"');
						break;
					case u'\'':
						str.push_back(u'\'');
						break;
					case u'n':
						str.push_back(u'\n');
						break;
					case u'f':
						str.push_back(u'\f');
						break;
					case u'r':
						str.push_back(u'\r');
						break;
					default:
						break;
					}
				}
				else {
					str += it->content;
				}
				it = ret.erase(it);
			}
			it->content = str;
			it->type = LexerToken::LexerType::Character;
			it++;
		}
		else if (it->content == u"\"") {
			u16string str;
			it = ret.erase(it);
			while (it->content != u"\"")
			{
				if (it->content == u"\\") {
					it = ret.erase(it);
					switch (it->content[0])
					{
					case u't':
						str.push_back(u'\t');
						break;
					case u'u':
						throw exception("未实现");
						break;
					case u'\\':
						str.push_back(u'\\');
						break;
					case u'a':
						str.push_back(u'\a');
						break;
					case u'\"':
						str.push_back(u'\"');
						break;
					case u'\'':
						str.push_back(u'\'');
						break;
					case u'n':
						str.push_back(u'\n');
						break;
					case u'f':
						str.push_back(u'\f');
						break;
					case u'r':
						str.push_back(u'\r');
						break;
					default:
						break;
					}
				}
				else {
					str += it->content;
				}
				it = ret.erase(it);
			}
			it->content = str;
			it->type = LexerToken::LexerType::String;
			it++;
		}
		else if (it->type == LexerToken::LexerType::Space) {
			it = ret.erase(it);
		}
		else {
			if (comment_now_line == it->source[0]) {
				it = ret.erase(it);
			}
			else if (multi_line_comment) {
				it = ret.erase(it);
			}
			else {
				if (it->type == LexerToken::LexerType::Unknown)
					it->type = LexerToken::LexerType::Identifier;
				it++;
			}
		}
	}
	return ret;
}
