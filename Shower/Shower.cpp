#pragma region includes and usings
#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <vector>
#include <map>
#include <memory>
#include <deque>
#include <stack>
#include <exception>
using std::string;
using std::u16string;
using std::cout;
using std::map;
using std::pair;
using std::array;
using std::vector;
using std::shared_ptr;
using std::weak_ptr;
using std::unique_ptr;
using std::exception;
using std::fstream;
using std::ifstream;
using std::ofstream;
using std::deque;
using std::stack;
using std::getline;
using std::make_pair;
using std::make_shared;
#pragma endregion
/*

Arithmetic Operators
Relational Operators
Logical Operators
Bitwise Operators
Assignment Operators
Misc Operators

Category	Operator	Associativity
Postfix 	() [] -> . ++ - -  	Left to right
Unary 	+ - ! ~ ++ - - (type)* & sizeof 	Right to left
Multiplicative  	* / % 	Left to right
Additive  	+ - 	Left to right
Shift  	<< >> 	Left to right
Relational  	< <= > >= 	Left to right
Equality  	== != 	Left to right
Bitwise AND 	& 	Left to right
Bitwise XOR 	^ 	Left to right
Bitwise OR 	| 	Left to right
Logical AND 	&& 	Left to right
Logical OR 	|| 	Left to right
Conditional 	?: 	Right to left
Assignment 	= += -= *= /= %=>>= <<= &= ^= |= 	Right to left
Comma 	, 	Left to right

Comment

*/
const static array<string, 39> keywords{"class", "interface", "function", "var", "enum", "using", "namespace", "integer", "float", "string", "void", "char", "boolean", "true", "false", "virtual", "abstract", "override", "friend", "readonly", "try", "catch", "throw", "do", "while", "foreach", "in", "for", "if", "else", "break", "coutinue", "goto", "switch", "case", "_assembly", "_generic", "_meta_programming"};

using source_t = array<size_t, 2>;
template<class T,class P>
bool has(T list, P value) {
	for (size_t i = 0; i < list.size(); i++)
	{
		if (list.at(i) == value)
		{
			return true;
		}
	}
	return false;
}
class MyUtils {
public:
	static void warn(string message, size_t code, source_t s) {
		cout << "Warn W" << code << ": " << message << '\n';
		cout << "At: <Source>(" << s[0] + 1 << ":?)" << '\n';
		cout << "更多信息: https://github.com/zly2006/HappyCoding/wiki/Warn-" << code;
	}
	static void error(string message, size_t code, source_t s) {
		cout << "Error E" << code << ": " << message << '\n';
		cout << "At: <Source>(" << s[0] + 1 << ":?)" << '\n';
		cout << "更多信息: https://github.com/zly2006/HappyCoding/wiki/Error-" << code;
		exit(code);
	}
	static char16_t parse_hex_u16char(u16string str, source_t s) {
		if (str.length() != 4) {
			error("无效的Hex在字符串中", 2032, s);
		}
		char16_t ret = 0;
		for (int i = 0; i < 4; i++) {
			ret *= 16;
			if (str[i] <= '9' && str[i] >= '0') {
				ret += str[i] - '0';
			}
			else if (str[i] <= 'f' && str[i] >= 'a') {
				ret += str[i] - 'a' + 10;
			}
			else if (str[i] <= 'F' && str[i] >= 'A') {
				ret += str[i] - 'A' + 10;
			}
			else {
				error("无效的Hex在字符串中", 2032, s);
			}
		}
		return (char)ret;
	}
	static char parse_hex_char(string str, source_t s) {
		if (str.length() != 2) {
			error("无效的Hex在字符串中", 2032, s);
		}
		unsigned char ret = 0;
		for (int i = 0; i < 2; i++) {
			ret *= 16;
			if (str[i] <= '9' && str[i] >= '0') {
				ret += str[i] - '0';
			}
			else if (str[i] <= 'f' && str[i] >= 'a') {
				ret += str[i] - 'a' + 10;
			}
			else if (str[i] <= 'F' && str[i] >= 'A') {
				ret += str[i] - 'A' + 10;
			}
			else {
				error("无效的Hex在字符串中", 2032, s);
			}
		}
		return (char)ret;
	}
	static string parse_string_literal(string str, source_t s) {
		if (str.empty() || *str.begin() != '\"' || *str.rbegin() != '\"') {
			error("内部错误。", 1000, s);
		}
		str = str.substr(1, str.length() - 2);
		for (size_t i = 0; i < str.length(); i++) {
			if (str[i] == '\\') {
				if (i == str.length()) {
					error("无效\\。", 2031, s);
				}
				switch (str[i + 1])
				{
				case 'n':
					str.replace(i, 2, "\n");
					break;
				case 'a':
					str.replace(i, 2, "\a");
					break;
				case 'v':
					str.replace(i, 2, "\v");
					break;
				case 't':
					str.replace(i, 2, "\t");
					break;
				case 'f':
					str.replace(i, 2, "\f");
					break;
				case '0':
					str.replace(i, 2, "\0");
					break;
				case 'r':
					str.replace(i, 2, "\r");
					break;
				case 'b':
					str.replace(i, 2, "\b");
					break;
				case '\'':
					str.replace(i, 2, "\'");
					break;
				case '\"':
					str.replace(i, 2, "\"");
					break;
				case '\\':
					str.replace(i, 2, "\\");
					break;
				case 'x':
					if (i > str.length() - 4) {
						error("无效\\。", 2031, s);
					}
					str.replace(i, 4, "" + parse_hex_char(str.substr(i + 2, 2), source_t{ s[0],s[1] + i }));
					break;
				default:
					error("无效\\。", 2031, s);
				}
			}
		}
		return str;
	}
};
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
			String,
			NameTag
		};
		LexerType type = LexerType::Unknown;
		string content;
		source_t source = source_t{0, 0};
	};
private:
	deque<LexerToken>parsed;
	using byte = unsigned char;
	int mark_type(char ch) {
		if (ch == '_')				return (byte)0;
		if (ch >= '!' && ch <= '/')	return (byte)1;
		if (ch >= ':' && ch <= '@')	return (byte)1;
		if (ch >= '[' && ch <= '^')	return (byte)1;
		if (ch >= '{' && ch <= '~')	return (byte)1;
		if (ch == '`')				return (byte)1;
		if (ch == '\r' || ch == '\t' || ch == '\n' || ch == ' ')return (byte)2;
		return (byte)0;
	}
	LexerToken parse_string(const string& code, size_t& position, size_t line, size_t string_start = UINT32_MAX) {
		if (string_start == UINT32_MAX)
			string_start = position;
		if (code[string_start] != '\"') {
			throw std::invalid_argument("Lexer::parse_string(...): code[string_start] != '\\\"'.");
		}
		for (size_t i = string_start + 1; i < code.length(); i++) {
			if (i < code.length() - 1 && code.substr(i, 2) == "\\\"") {
				i++;
			}
			else if (code.at(i) == '\"') {
				// string end
				position = i + 1;
				return LexerToken{ LexerToken::LexerType::String,code.substr(string_start,position - string_start),array<size_t,2>{line,string_start} };
			}
		}
		MyUtils::error("Missing \" at the end of string.", 2037, source_t{ line,position });
	}
	LexerToken move_next(const string& code, size_t& position, size_t line) {
		if (parsed.size())
		{
			LexerToken token = parsed.front();
			parsed.pop_front();
			return token;
		}
		const static array<string, 25> TwoCharacterMark = { "\\\\","\\\'","\\\"","//","/*","*/","=>","++","--","<< "," >> "," += "," -= "," *= "," /= "," ^= "," |= "," &= "," %= "," == "," >= "," <= "," != "," || "," && " };
		const static array<string, 2> ThreeCharacterMark = { "<<=",">>=" };
		const static array<string, 8> SeparateOperatpors = { ";",",","{","}","(",")","[","]" };
		position--;// for position++ next
		int mk_type;
		do {
			position++;
			mk_type = mark_type(code[position]);
		} while (mk_type == 2 && position < code.length());
		bool digit = isdigit(code[position]);
		size_t start = position;
		while (position < code.length() && mark_type(code[position]) == mk_type
			|| (digit && code[position] == '.'))
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
					string token = code.substr(i, 3);
					parsed.push_back(LexerToken{ LexerToken::LexerType::Operator,token,array<size_t,2>{line,i} });
					i += 3;
				}
				else if (i < position - 1 && has(TwoCharacterMark, code.substr(i, 2))) {
					string token = code.substr(i, 2);
					parsed.push_back(LexerToken{ LexerToken::LexerType::Operator,token,array<size_t,2>{line,i} });
					i += 2;
				}
				else {
					string token = code.substr(i, 1);
					if (token == "\"") {
						parsed.push_back(parse_string(code, position, line, i));
						break;// 防止继续分割Operators，引发重复读取
					}
					else
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
		else
		{
			ret.type = LexerToken::LexerType::NameTag;
		}
		return ret;
		//ret.type=LexerToken::LexerType::
		//9558 8016 0210 4077 062
	}
public:
	deque<LexerToken> parse(const deque<string>& code) {
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
		}
		//删除注释 (字符串转义和处理、数字处理在Parser)
		size_t comment_now_line = UINT32_MAX;
		bool multi_line_comment = false;
		for (auto it = ret.begin(); it != ret.end();)
		{
			if (it->content == "//") {
				comment_now_line = it->source[0];
				it = ret.erase(it);
			}
			else if (it->content == "/*" && comment_now_line != it->source[0]) {
				multi_line_comment = true;
				it = ret.erase(it);
			}
			else if (it->content == "*/") {
				multi_line_comment = false;
				it = ret.erase(it);
			}
			else {
				if (comment_now_line == it->source[0]) {
					it = ret.erase(it);
				}
				else if (multi_line_comment) {
					it = ret.erase(it);
				}
				else it++;
			}
		}
		return ret;
	}
};
class Parser {
public:
	class ParserTreeNode {
	public:
		enum class NodeType {
			Literal,
			ReadVariable,
			Call,

		};
		vector<ParserTreeNode>childs;
	};
	class ParserTree {
	public:
		ParserTreeNode root;
	};
	class ParserSentence {
	public:
		enum class SentenceType {
			Assignment,
			Declear,
			Arithmetic,
			Using,
			Control,
		};

	};
	class ParserSentenceBlock {
	public:
	};
};

void write_IL_file(Parser::ParserTree tree, string filename) {

}

int main(int argc, char** argv)
{
	deque<string> code;
	vector<string>args;
	for (int i = 0; i < argc; i++)
	{
		args.push_back(string(argv[i]));
	}
	if (args.size() >= 2)
	{
		ifstream fin(args[1]);
		while (fin)
		{
			string line;
			getline(fin, line);
			code.push_back(line);
		}
		for (size_t i = 0; i < code.size(); i++)
		{
			cout << code[i] << '\n';
		}
		Lexer lexer;
		auto i = lexer.parse(code);
		for (auto j : i) {
			cout << "LexerToken:{type:" << (int)j.type << ",source:[" << j.source[0] << "," << j.source[1] << "],content:" << j.content << "}\n";
		}
	}
	else
		cout << "Hello World!\n";
}
