#ifndef SHOWER_BASIC_H
#define SHOWER_BASIC_H
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
#include <exception>
#include <codecvt>
#include <conio.h>
using std::codecvt;
using std::codecvt_utf16;
using std::u16string;
using std::string;
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
using std::move;
using std::swap;
#pragma endregion
using source_t = std::array<size_t, 2>;
const static std::array<std::u16string, 40> keywords{ u"class", u"interface", u"function", u"var", u"enum", u"using", u"namespace", u"integer", u"float", u"string", u"void", u"char", u"boolean", u"true", u"false", u"virtual", u"abstract", u"override", u"friend", u"readonly", u"try", u"catch", u"throw", u"do", u"while", u"foreach", u"in", u"for", u"if", u"else", u"break", u"coutinue", u"goto", u"switch", u"case", u"default", u"return", u"_assembly", u"_generic", u"_meta_programming" };
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
template<class T>
T& as(void* ptr) {
	return *(T*)ptr;
}
template<class T>
T* as_ptr(void* ptr) {
	return (T*)ptr;
}
#else
#endif
