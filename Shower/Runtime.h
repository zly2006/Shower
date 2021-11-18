#pragma once
#include "Basic.h"
using id_t = size_t;
struct RuntimeObject {
	enum class ROType {
		Undefined,
		Constant,
		Namespace,
		Type,
		Varible,
		Method,
	};
	RuntimeObject() { id = 0; type = ROType::Undefined; }
	RuntimeObject(id_t _id, ROType _type) :id(_id), type(_type) {}
	id_t id;
	ROType type;
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
	Namespace(id_t _id, u16string _name) : RuntimeObject(_id, RuntimeObject::ROType::Namespace) {
		name = _name;
	}
	u16string name;
};
struct Type : RuntimeObject {
	Type(id_t _id, u16string _name, vector<RuntimeObject*>_inner, vector<RuntimeObject*>_methods, bool _basic = false)
		:RuntimeObject(_id, RuntimeObject::ROType::Type), namespace_(Namespace(0, _name.substr(0, _name.find_last_of('.')))),
		name(_name.substr(_name.find_last_of('.') + 1)),
		inner(_inner), methods(_methods), basic(_basic), isGeneric(false), ganericTypes({})
	{
	}
	bool basic;
	RuntimeObject namespace_;
	u16string name;
	vector<RuntimeObject*>inner;
	vector<RuntimeObject*>methods;
	bool isGeneric;
	vector<RuntimeObject*>ganericTypes;
};
//Varible(id_t _id, RuntimeObject* _type, void* _data, RuntimeObject* _declearClass, Visable _visable, bool _isStatic)
struct Varible : RuntimeObject {
	Varible(id_t _id, RuntimeObject* _type, void* _data, RuntimeObject* _declearClass, Visable _visable, bool _isStatic) 
		:RuntimeObject(_id, RuntimeObject::ROType::Varible),type(_type),data(_data),declearClass(_declearClass),visable(_visable),isStatic(_isStatic)
	{

	}
	RuntimeObject* type;
	void* data;
	bool isStatic;
	RuntimeObject* declearClass;
	Visable visable;
#pragma region 对应的C++类型
	template<int name>
	class CppTypes {
		using type = exception;
	};
	template<>
	class CppTypes<1> {
		using type = void;
	};
	template<>
	class CppTypes<2> {
		using type = u16string;
	};
	template<>
	class CppTypes<3> {
		using type = double;
	};
	template<>
	class CppTypes<4> {
		using type = long long;
	};
	template<>
	class CppTypes<5> {
		using type = bool;
	};
	template <class T,class P>
	class SameType {
	public:
		static bool same() { return false; }
	};
	template <class T>
	class SameType<T, T> {
	public:
		static bool same() { return true; }
	};
#pragma endregion
};
//Method(id_t _id, u16string _name, RuntimeObject _declearType, RuntimeObject _returnType,
//	vector<RuntimeObject*>_argType, Visable _visable, CallType _call) :RuntimeObject(_id, RuntimeObject::ROType::Method),
struct Method : RuntimeObject {
	using CallType = Varible*(*)(vector<Varible>types);
	Method(id_t _id, u16string _name, RuntimeObject* _declearType, RuntimeObject* _returnType,
		vector<RuntimeObject*>_argType, Visable _visable, CallType _call) :RuntimeObject(_id, RuntimeObject::ROType::Method),
		declearType(_declearType), returnType(_returnType), argType(_argType), call(_call), visable(_visable) {}
	RuntimeObject* declearType;
	RuntimeObject* returnType;
	vector<RuntimeObject*>argType;
	CallType call;
	Visable visable;
};
