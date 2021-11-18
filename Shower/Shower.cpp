#pragma warning(disable:4996)
#include "Lexer.h"
#include "Basic.h"
#include "Runtime.h"
#pragma message("\
        _________    __\n\
       /  ______/   / /\n\
      /  /         / /\n\
     /  /_____    / /_____   ______   _   __  _    ______    _ ___\n\
     \\_____   /  / ____  /  / ____ \\ | | /  |/ /  / ____ \\  / / _ / \n\
          /  /  / /   / /  / /   / / | |/  |  /  / /___/ / / _/\n\
         /  /  / /   / /  / /   / /  | / / | /  / ______/ / /\n\
  ______/  /  / /   / /  / /___/ /   |  /|  /  / /_____  / /\n\
 /________/  /_/   /_/   \\______/    |_/ |_/   \\______/ /_/\n\
")
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

/*
Parser:
1. Scan
*
*
*
*/
class lexer_error_exception : exception {
public:
	int err_type = 0;
	source_t src;
	lexer_error_exception(int err_code, const source_t &_src, const string &message = "") :exception(message.c_str()) {
		err_type = err_code;
		src = _src;
	}
};
std::string UnicodeToANSI(const std::wstring& wstr)
{
	std::string ret;
	std::mbstate_t state = {};
	const wchar_t* src = wstr.data();
	std::wstring_convert<std::codecvt_utf8<wchar_t>>cv;
	/*
	size_t len = std::wcsrtombs(nullptr, &src, 0, &state);
	if (static_cast<size_t>(-1) != len) {
		std::unique_ptr< char[] > buff(new char[len + 1]);
		len = std::wcsrtombs(buff.get(), &src, len, &state);
		if (static_cast<size_t>(-1) != len) {
			ret.assign(buff.get(), len);
		}
	}
	*/
	u16string str;
	return cv.to_bytes(wstr);
	return ret;
}

std::u16string ANSIToUnicode(const std::string& str)
{
	std::wstring ret;
	std::mbstate_t state = {};
	const char* src = str.data();
	size_t len = std::mbsrtowcs(nullptr, &src, 0, &state);
	if (static_cast<size_t>(-1) != len) {
		std::unique_ptr< wchar_t[] > buff(new wchar_t[len + 1]);
		len = std::mbsrtowcs(buff.get(), &src, len, &state);
		if (static_cast<size_t>(-1) != len) {
			ret.assign(buff.get(), len);
		}
	}
	u16string rret;
	for (wchar_t i : ret)rret += i;
	return rret;
}
std::ostream& operator<<(std::ostream& output, u16string str) {
	std::wstring wstr;
	for (wchar_t i : str)
	{
		wstr += (char16_t)i;
	}
	output << UnicodeToANSI(wstr);
	return output;
}
class MyUtils {
public:
	static void warn(u16string message, size_t code, source_t s) {
		cout << "Warn W" << code << ": " << message << '\n';
		cout << "At: <Source>(" << s[0] << ":" << s[1] << ")" << '\n';
		cout << "更多信息: https://gitee.com/zhaoliyan/Shower/wiki/CompilerErrors/Warn-" << code;
	}
	static void error(u16string message, size_t code, source_t s) {
		cout << "Error E" << code << ": " << message << '\n';
		cout << "At: <Source>(" << s[0] << ":" << s[1] << ")" << '\n';
		cout << "更多信息: https://gitee.com/zhaoliyan/Shower/wiki/CompilerErrors/Error-" << code;
		exit(code);
	}
	static char16_t parse_hex_char(u16string str, source_t s) {
		if (str.length() != 4) {
			error(u"无效的Hex在字符串中", 2032, s);
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
				error(u"无效的Hex在字符串中", 2032, s);
			}
		}
		return ret;
	}
	static char16_t parse_u16string_char(const u16string& str, size_t& position, source_t s) {
		if (str[position] == '\\') {
			if (position == str.length()) {
				throw lexer_error_exception(2031, source_t{ s[0],s[1] + position });
			}
			switch (str[++position])
			{
			case 'n':
				return u'\n';
			case 'a':
				return u'\a';
			case 'v':
				return u'\v';
			case 't':
				return u'\t';
			case 'f':
				return u'\f';
			case '0':
				return u'\0';
			case 'r':
				return u'\r';
			case 'b':
				return u'\b';
			case '\'':
				return u'\'';
			case '\"':
				return u'\"';
			case '\\':
				return u'\\';
			case 'x':
				if (position > str.length() - 4) {
					throw lexer_error_exception(2031, source_t{ s[0],s[1] + position });
				}
				return parse_hex_char(str.substr(position + 2, 2), source_t{ s[0],s[1] + position });
			default:
				throw lexer_error_exception(2031, source_t{ s[0],s[1] + position });
			}
		}
		else if (str[position] == '\"')return 65535;
	}
};

class Parser {
	deque<Lexer::LexerToken>tokens;
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
			u16string str_data;
			__int64 bits_data;
		}data;
	};
	class ParserTree {
	public:
		ParserTreeNode root;
	};
	Parser(deque<Lexer::LexerToken> _tokens) {
		tokens = move(_tokens);
	}
private:
	source_t match(ParserTreeNode::NodeType type) {

	}
	ParserTreeNode expr(source_t src) {

	}
public:
	void parse(deque<Lexer::LexerToken>) {

	}
};

map<id_t, RuntimeObject*>RuntimeList;
void write_IL_file(Parser::ParserTree tree, u16string filename) {

}

void execute_IL_file(char* IL_file) {

}

RuntimeObject* run_AST_node(Parser::ParserTreeNode& node) {
	switch (node.type)
	{
	case Parser::ParserTreeNode::NodeType::Literal: {
		id_t id = RuntimeList.size() - 1;
		RuntimeList[id] = (RuntimeObject*)new Varible(id, RuntimeList[4], &node.data.str_data, RuntimeList[0], Visable::PackageProtected, false);
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
			RuntimeObject* var = run_AST_node(node.childs[i]);
			args.push_back(*(Varible*)var);
		}
		Method* pmethod = (Method*)run_AST_node(node.childs[0]);
		RuntimeList[id] = pmethod->call(args);
		return RuntimeList[id];
	}
	case Parser::ParserTreeNode::NodeType::Member:
		if (node.childs.size() == 2) {
			auto it = RuntimeList.find(node.childs[0].data.bits_data);
			if (it != RuntimeList.end()) {
				Type* pt = (Type*)it->second;
			}
		}
		else {

		}
		break;
	case Parser::ParserTreeNode::NodeType::Identifier:
		return RuntimeList[node.data.bits_data];
	default:
		break;
	}
}

void run_AST(Parser::ParserTree tree) {
	run_AST_node(tree.root);
}
int main(int argc, char** argv)
{
	RuntimeList = {
		{0,new Namespace(0,u"shower")},
		{1,new Type(1,u"shower.Void",{},{},true)},
		{2,new Type(2,u"shower.String",{},{},true)},
		{3,new Type(3,u"shower.Float",{},{},true)},
		{4,new Type(4,u"shower.Integer",{},{},true)},
		{5,new Type(5,u"shower.Boolean",{},{},true)},
		{6,new Varible(6,RuntimeList[1],0,RuntimeList[1],Visable::Public,true)},
		{7,new Namespace(7,u"shower.io")},
		{8,new Type(8,u"shower.io.Console",{},{RuntimeList[9]})},
		{9,new Method(9,u"Write",RuntimeList[8],RuntimeList[1],{RuntimeList[2]},Visable::Public,
			[](vector<Varible>types)->Varible* {
				if (types.size() >= 1 && types[0].type->id == 4) {
					u16string* p = (u16string*)types[0].data;
					cout << *(u16string*)types[0].data;
				}
				else {
					MyUtils::error(u"[Runtime Error] shower.io.Console.Write(u16string): args is not enough or is not a u16string.",5001,{0,0});
				}
				return (Varible*)(RuntimeList[6]);
			}
		)},
		{10,new Varible(10,RuntimeList[5],(void*)0,RuntimeList[5],Visable::Public,true)},
		{11,new Varible(11,RuntimeList[5],(void*)1,RuntimeList[5],Visable::Public,true)},
		{12,new Method(12,u"@operator==",RuntimeList[2],RuntimeList[5],{RuntimeList[2],RuntimeList[2]},Visable::Public,
			[](vector<Varible>args)->Varible* {
				if (as<u16string>(args[0].data) == as<u16string>(args[1].data))
					return as_ptr<Varible>(RuntimeList[11]);
				else
					return as_ptr<Varible>(RuntimeList[10]);
			}
		)},
		{13,new Method(13,u"@operator!=",RuntimeList[2],RuntimeList[5],{RuntimeList[2],RuntimeList[2]},Visable::Public,
			[](vector<Varible>args)->Varible* {
				if (as<u16string>(args[0].data) == as<u16string>(args[1].data))
					return as_ptr<Varible>(RuntimeList[10]);
				else
					return as_ptr<Varible>(RuntimeList[11]);
			}
		)},
	};
	cout << 
"\
        _________    __\n\
       /  ______/   / /\n\
      /  /         / /\n\
     /  /_____    / /_____   ______   _   __  _    ______    _ ___\n\
     \\_____   /  / ____  /  / ____ \\ | | /  |/ /  / ____ \\  / / _ / \n\
          /  /  / /   / /  / /   / / | |/  |  /  / /___/ / / _/\n\
         /  /  / /   / /  / /   / /  | / / | /  / ______/ / /\n\
  ______/  /  / /   / /  / /___/ /   |  /|  /  / /_____  / /\n\
 /________/  /_/   /_/   \\______/    |_/ |_/   \\______/ /_/\n\
Version: 0.0.01-dev1 (git commit 15f021cb558b9cd2179b3a30a396471ecbace850)\n";
	deque<u16string> code;
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
			code.push_back(ANSIToUnicode(line));
		}
	/*	for (size_t i = 0; i < code.size(); i++)
		{
			
			cout << code[i] << '\n';
		}*/
		Lexer lexer;
		auto i = lexer.parse(code);
#if 1
		cout << "[Debug] Lexer finished. display the result?\n";
		if (_getch() == 'y') {
			for (auto j : i) {
				cout << "LexerToken:{type:" << (int)j.type << ",source:[" << j.source[0] << "," << j.source[1] << "],content:\"" << j.content << "\"}\n";
			}
		}
		else cout << std::endl;
		Parser::ParserTree tree = Parser::ParserTree{
			Parser::ParserTreeNode{//root
				Parser::ParserTreeNode::NodeType::Call,
				{//root.c
					Parser::ParserTreeNode{
						Parser::ParserTreeNode::NodeType::Identifier,
						{},
						Parser::ParserTreeNode::Data{u"shower.io.Console.Write",9}
					},
					Parser::ParserTreeNode{
						Parser::ParserTreeNode::NodeType::Literal,
						{},
						Parser::ParserTreeNode::Data{u16string(u"Hello World!"),0}
					}
				},//root.c
				Parser::ParserTreeNode::Data{u"",0}
			}
		};//root
		run_AST(tree);
		//Console.Write("Hello World");
#endif
	}
	else
		cout << "Hello World!\n";
}
