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
#include <tuple>
#include <exception>//"
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
using std::tuple;
using std::getline;
using std::make_pair;
using std::make_tuple;
using std::make_unique;
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
const static array<string, 40> keywords{"class", "interface", "function", "var", "enum", "using", "namespace", "integer", "float", "string", "void", "char", "boolean", "true", "false", "virtual", "abstract", "override", "friend", "readonly", "try", "catch", "throw", "do", "while", "foreach", "in", "for", "if", "else", "break", "coutinue", "goto", "switch", "case", "default", "return", "_assembly", "_generic", "_meta_programming"};

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
		cout << "更多信息: https://gitee.com/zhaoliyan/Shower/wiki/CompilerErrors/Warn-" << code;
	}
	static void error(string message, size_t code, source_t s) {
		cout << "Error E" << code << ": " << message << '\n';
		cout << "At: <Source>(" << s[0] + 1 << ":?)" << '\n';
		cout << "更多信息: https://gitee.com/zhaoliyan/Shower/wiki/CompilerErrors/Error-" << code;
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
					error("\\。", 2031, s);
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
	template<class K, class V>
	class type_keeping_map :map<K, V> {

	};
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
			Identifier
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
		return { LexerToken::LexerType::Unknown,"",source_t{line,position} };
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
					else if (token == "\'") {
						start = i;
						for (i = i + 1; (code[i] != '\'' || code.substr(i - 1, 2) == "\\\'") && i < code.length(); i++) {}
						if (code[i] != '\'') {
							MyUtils::error("Missing \' in char define.", 2038, source_t{ line,i });
						}
						else {
							parsed.push_back(LexerToken{ LexerToken::LexerType::String,code.substr(start,i - start),source_t{line,start} });
							position = i + 1;
							break;//同""的处理
						}
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
			ret.type = LexerToken::LexerType::Identifier;
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
#pragma region 基本属性定义
using id_t = size_t;
struct RuntimeObject {
	RuntimeObject() { id = 5; }
	RuntimeObject(id_t _id) :id(_id) {}
	id_t id;
};
enum class Visable
{
	Private,
	PackageProtected,
	Protected,
	Public,
};
struct Namespace : RuntimeObject
{
	Namespace(id_t _id, string _name) : RuntimeObject(_id) {
		name = _name;
	}
	string name;
};
struct Type : RuntimeObject {
	Type(id_t _id, string _name, vector<RuntimeObject>_inner, vector<RuntimeObject>_methods, bool _basic = false)
		:RuntimeObject(_id), namespace_(Namespace(0, _name.substr(0, _name.find_last_of('.')))),
		name(_name.substr(_name.find_last_of('.') + 1)),
		inner(_inner), methods(_methods), basic(_basic), isGeneric(false), ganericTypes({})
	{
	}
	bool basic;
	RuntimeObject namespace_;
	string name;
	vector<RuntimeObject>inner;
	vector<RuntimeObject>methods;
	bool isGeneric;
	vector<RuntimeObject>ganericTypes;
};
struct Varible : RuntimeObject {
	Varible(id_t _id, RuntimeObject _type, void* _data, RuntimeObject _declearClass, Visable _visable, bool _isStatic) 
		:RuntimeObject(_id),type(_type),data(_data),declearClass(_declearClass),visable(_visable),isStatic(_isStatic)
	{

	}
	RuntimeObject type;
	void* data;
	bool isStatic;
	RuntimeObject declearClass;
	Visable visable;
};
struct Method : RuntimeObject {
	using CallType = Varible(*)(vector<Varible>types);
	Method(id_t _id, string _name, RuntimeObject _declearType, RuntimeObject _returnType,
		vector<RuntimeObject>_argType, Visable _visable, CallType _call) :RuntimeObject(_id),
		declearType(_declearType), returnType(_returnType), argType(_argType), call(_call), visable(_visable) {}
	RuntimeObject declearType;
	RuntimeObject returnType;
	vector<RuntimeObject>argType;
	CallType call;
	Visable visable;
};
#pragma endregion
class Parser {
public:
	class ParserTreeNode {
	public:
		enum class NodeType {
			Literal,
			AccessValue,
			Identifier,
			Call,
			Operator,
			Assignment,
			PointedValue,
			GetAddress,
			Member,
			Constructure,
			Destructure,
			Comma,
			Declear,
			Control,
			Goto,
			SentenceBlock,
		};
		NodeType type;
		vector<ParserTreeNode>childs;
		struct Data
		{
			string str_data;
			__int64 bits_data;
		}data;
	};
	class ParserTree {
	public:
		ParserTreeNode root;
	};
};

struct FunctionCall
{
	vector<Varible>vars;
};
stack<FunctionCall>calls;

map<id_t, RuntimeObject>RuntimeList = {
	{0,Namespace(0,"shower.io")},
	{1,Type(1,"shower.io.Console",{},{RuntimeObject(2)})},
	{2,Method(2,"Write",RuntimeObject(1),RuntimeObject(3),{RuntimeObject(4)},Visable::Public,
		[](vector<Varible>types)->Varible {
			if (types.size() >= 1 && types[0].type.id == 4) {
				cout << *(string*)types[0].data;
			}
			else {
				MyUtils::error("[Runtime Error] shower.io.Console.Write(string): args is not enough or is not a string.",5001,{0,0});
			}
			return *(Varible*)(&RuntimeList[5]);
		}
	)},
	{3,Type(3,"shower.Void",{},{},true)},
	{4,Type(4,"shower.String",{},{},true)},
	{5,Type(5,"shower.Float",{},{},true)},
	{6,Type(6,"shower.Integer",{},{},true)},
	{7,Varible(7,RuntimeObject{3},0,RuntimeObject(3),Visable::Public,true)},
};
void write_IL_file(Parser::ParserTree tree, string filename) {

}

void execute_IL_file(char* IL_file) {

}

RuntimeObject run_AST_node(Parser::ParserTreeNode node) {
	switch (node.type)
	{
	case Parser::ParserTreeNode::NodeType::Literal: {
		id_t id = RuntimeList.size() - 1;
		RuntimeList[id] = Varible(id, RuntimeList[4], &node.data.str_data, RuntimeList[0], Visable::PackageProtected, false);
		return RuntimeList[id];
	}
	case Parser::ParserTreeNode::NodeType::SentenceBlock:
		for (auto &cnode : node.childs)run_AST_node(cnode);
		return RuntimeList[5];
	case Parser::ParserTreeNode::NodeType::Call: {
		id_t id = RuntimeList.size() - 1;
		vector<Varible> args;
		for (size_t i = 1; i < node.childs.size(); i++)
		{
			RuntimeObject var = run_AST_node(node.childs[i]);
			args.push_back(*(Varible*)(&var));
		}
		RuntimeList[id] = ((Method*)(&node.childs[0]))->call(args);
		return RuntimeList[id];
	}
	case Parser::ParserTreeNode::NodeType::Member:
		break;
	case Parser::ParserTreeNode::NodeType::Identifier:
		break;
	default:
		break;
	}
}

void run_AST(Parser::ParserTree tree) {
	run_AST_node(tree.root);
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
#if 1
		for (auto j : i) {
			cout << "LexerToken:{type:" << (int)j.type << ",source:[" << j.source[0] << "," << j.source[1] << "],content:\"" << j.content << "\"}\n";
		}
		Parser::ParserTree tree = Parser::ParserTree{
			Parser::ParserTreeNode{//root
				Parser::ParserTreeNode::NodeType::Call,
				{//root.c
					Parser::ParserTreeNode{
						Parser::ParserTreeNode::NodeType::Member,
						{
							Parser::ParserTreeNode{
								Parser::ParserTreeNode::NodeType::Identifier,
								{},
								Parser::ParserTreeNode::Data{"shower.io.Console",0}
							},
							Parser::ParserTreeNode{
								Parser::ParserTreeNode::NodeType::Identifier,
								{},
								Parser::ParserTreeNode::Data{"Write",0}
							}
						},
						Parser::ParserTreeNode::Data{"",0}
					},
					Parser::ParserTreeNode{
						Parser::ParserTreeNode::NodeType::Literal,
						{},
						Parser::ParserTreeNode::Data{"Hello World!",0}
					}
				},//root.c
				Parser::ParserTreeNode::Data{"",0ull}
			}
		};//root
		run_AST(tree);
		//Console.Write("Hello World");
#endif
	}
	else
		cout << "Hello World!\n";
}
