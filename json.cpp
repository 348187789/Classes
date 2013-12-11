#include <unordered_map>
#include <vector>
#include <string>
#include <tuple>
#include <iterator>

#include <fstream>
#include <exception>
#include <algorithm>
#include <functional>

#include <memory>
#include <cassert>

#include "Log.h"

#include "json.h"


//  class JsonObject
using std::array;
using std::unordered_map;
using std::vector;
using std::string;
using std::unique_ptr;
using std::tuple;

static array<std::string, 7> init()
{
	typedef decltype(init()) retType;
	retType ret = {"Bool", "Int", "Float", "String", "List", "Dict"};
	return std::move(ret);
}

const array<std::string, 7> JsonObject::nameOfType = init();


bool& JsonObject::toBool() const
{
	assert(isBool()); 
	return contentPtr_->boolValue;
}

const int& JsonObject::toInt() const 
{
	assert(isInt());
	return contentPtr_->intValue;
}

const float& JsonObject::toFloat()  const
{
	assert(isFloat());
	return contentPtr_->floatValue;
}

const std::string& JsonObject::toString() const
{
	assert(isString());
	return *(contentPtr_->stringPtr);
}
const std::unordered_map<std::string, JsonObject>& JsonObject::toDict() const
{
	assert(isDict());
	return *(contentPtr_->dictPtr);
}
const std::vector<JsonObject>& JsonObject::toList() const
{
	assert(isList());
	return *(contentPtr_->listPtr);
}

JsonObject& JsonObject::operator[](const string& str) const
{
	assert(isDict());
	return (*(contentPtr_->dictPtr))[str];
}
JsonObject& JsonObject::operator[](int index) const
{
	assert(isList());
	return (*(contentPtr_->listPtr))[index];
}

int JsonObject::getListSize() const
{
	assert(isList());
	return contentPtr_->listPtr->size();
}

int JsonObject::getDictSize() const
{
	assert(isDict());
	return contentPtr_->dictPtr->size();
}

class ParseError : public std::exception 
{
public:	
	bool useString;
	std::string str;
	ParseError(const char* m) : std::exception(m), useString(false) {}
	ParseError(const std::string& inputStr) : std::exception(), useString(true), str(inputStr){}
	const char* what() const
	{
		if(!useString) return std::exception::what();
		return str.c_str();
	}
};

void parseAssert(bool expr, const char* message)
{
	assert(message);
	if(!expr) throw ParseError(message);
}
void parseAssert(bool expr, const string& message)
{
	if(!expr) throw ParseError(message);
}

void readJsonFromString(const string& str, JsonObject::JsonType& jsonType, unique_ptr<JsonValue>& contentPtr,
						string::const_iterator& iter);

//******************************************************
//			constructors
//******************************************************

JsonObject::JsonObject(std::tuple<JsonType, unique_ptr<JsonValue> > t)
	: type_ (std::get<0>(t))
	, contentPtr_(std::move(std::get<1>(t)))
{
}


JsonObject::JsonObject(const char* filename, JsonErrorCode& e)
	: type_(typeEmpty)
{
	std::ifstream s(filename);
	assert(s.is_open());
	string str = string(std::istreambuf_iterator<char>(s), 
		std::istreambuf_iterator<char>());
	string::const_iterator endAt;
	try{
		readJsonFromString(str, type_, contentPtr_, endAt);
	}
	catch(const ParseError& excp)
	{
		type_ = typeEmpty;
		contentPtr_.reset();
		unsigned int line = std::count_if(str.cbegin(), endAt, [](const char& c){return c == '\n';}) + 1;
		e.reset(string("at line ") + std::to_string(line) + ':' + excp.what());  
	}
}


//******************************************************
//			parser
//******************************************************


void readJsonFromString(const string& str, JsonObject::JsonType& jsonType, unique_ptr<JsonValue>& contentPtr,
										  string::const_iterator& iter)
{
	iter = str.cbegin();
	auto end = str.cend();
	
	auto skipMeaninglessCharacter = [&iter, end]()
	{
		for(;iter != end; ++iter)
		{
			if(*iter != ' ' && *iter != '\t' && *iter != '\n')
			{
				break;
			}
		}
	};

	typedef  tuple<JsonObject::JsonType, unique_ptr<JsonValue> > TypeValueTuple;
	std::function<TypeValueTuple()> readJsonObject = [&]
	() -> TypeValueTuple
	{
		//*************************************************************
		//          readXXX()
		//*************************************************************

		// key of dict 
        auto readString = [&]
        () -> string
        {
            string ret;
			parseAssert(iter != end && *iter == '\"', string() + "should be '\"'");
			++iter;
			for(;iter != end && *iter != '\"'; ++iter)
			{
				if(*iter == '\"')
				{
					break;
				}
				else if(*iter == '\\')
				{
					++iter;
					parseAssert(iter != end && (*iter == '\"' || *iter == '\\'),
						"'\\' is supported just for '\"' and '\\'");
					ret.push_back(*iter);
				}
				else
				{
					ret.push_back(*iter);
				}
			}
			parseAssert(iter != end && *iter == '\"', "unmatched '\"'");
			++iter;
			return ret;
        };

		typedef vector<JsonObject> ListType;
        auto readList = [&]
        () -> vector<JsonObject>
        {
            ListType ret;
			parseAssert(iter != end && *iter == '[', "?1");
			++iter;
			skipMeaninglessCharacter();
			if(iter != end && *iter == ']') { ++iter; return ret; }

			ret.push_back(JsonObject(readJsonObject()));
			skipMeaninglessCharacter();
			while(iter != end && *iter == ',')
			{
				++iter;
				skipMeaninglessCharacter();
				ret.push_back(JsonObject(readJsonObject()));
				skipMeaninglessCharacter();
			}
			parseAssert(iter != end && *iter == ']', "unmatched '['");
			++iter;
			return ret;
        };
        
        typedef std::unordered_map<string, JsonObject> DictType;
        auto readDict = [&]
        () -> DictType
        {
            DictType ret;
			parseAssert(iter != end && *iter == '{', "?2");
			++iter;
			skipMeaninglessCharacter();
			if(iter != end && *iter  == '}') { ++iter; return ret; }
		
			auto readMapPair = [&]
			() -> DictType::value_type
			{
				string key; JsonObject value;
				key = readString();
				skipMeaninglessCharacter();
				parseAssert(iter != end && *iter == ':', "invalid character, should input ':'");
				++iter;
				skipMeaninglessCharacter();
				value = readJsonObject();
				return DictType::value_type(std::move(key), std::move(value));
			};

			ret.insert(readMapPair());
			skipMeaninglessCharacter();
			while(iter != end && *iter == ',')
			{
				++iter;
				skipMeaninglessCharacter();
				ret.insert(readMapPair());
				skipMeaninglessCharacter();
			}
			parseAssert(iter != end && *iter == '}', "unmatched '{'");
			++iter;
			return ret;
        };


        auto readBool = [&]
        () -> bool
        {
			parseAssert(iter != end && (*iter == 'f' || *iter == 't'), "?3");
            if(*iter == 'f')
            {
                parseAssert(iter + 5 <= end,  "invalid bool value");
                parseAssert(*(++iter) == 'a', "invalid bool value");
                parseAssert(*(++iter) == 'l', "invalid bool value");
                parseAssert(*(++iter) == 's', "invalid bool value");
                parseAssert(*(++iter) == 'e', "invalid bool value");
                ++iter;
				return false;
            }
            else if(*iter == 't')
            {
                parseAssert(iter + 4 <= end,  "invalid bool value");
                parseAssert(*(++iter) == 'r', "invalid bool value");
                parseAssert(*(++iter) == 'u', "invalid bool value");
                parseAssert(*(++iter) == 'e', "invalid bool value");
                ++iter;
				return true;
            }
			parseAssert(false, "impossible");
			return false;
        };

        auto readNum = [&]
        () -> tuple<JsonObject::JsonType, unique_ptr<JsonValue> >
        {
            bool isFloat = false;
            int isPositive = 1; // realNum = num * isPositive

            int intResult = 0;
            float floatResult = 0;
			unique_ptr<JsonValue> resultValue(new JsonValue);
            
			float floatScale = 0.1f;
            parseAssert(iter != end &&( *iter >= '0' && *iter <= '9' || *iter == '-' || *iter == '+'), "?4");
            switch(*iter)
            {
                case '-' : isPositive = -1; ++iter; break;
                case '+' : ++iter; break;
            }
            parseAssert(iter != end, "unexcept EOF");
            parseAssert('0' <= *iter && *iter < '9', "invalid number");
            for(;iter != end; ++iter)
            {
                if(*iter >= '0' && *iter <= '9')
                {
                    if(!isFloat)
                    {
                        intResult *= 10;
                        intResult += *iter - '0';
                    }                
                    else
                    {
                        floatResult += (*iter - '0')*floatScale;
                        floatScale /= 10;
                    }
                }
                else if(*iter == '.')
                {
                    parseAssert(isFloat == false, "unexcept '.'");
                    isFloat = true;
                    floatResult = intResult;
                }
                else if(*iter == ' ' || *iter == '\t' || *iter == '\n' || *iter == ']' || *iter == ',' )
                {
                    break;
                }
                else
                {
                    parseAssert(false, "unexcept character ss326");
                }
            }
            if(isFloat)
			{
				resultValue->floatValue = floatResult * isPositive; // POSITIVE
				return std::make_tuple(JsonObject::typeFloat, std::move(resultValue));
			}
			else
			{
				resultValue->intValue = intResult * isPositive;  // POSITIVE
				return std::make_tuple(JsonObject::typeInt, std::move(resultValue));
			}
        };

		//*************************************************************
		//		parse Json object
		//*************************************************************
		 
		JsonObject::JsonType type = JsonObject::typeEmpty;
		unique_ptr<JsonValue> value(new JsonValue);
		skipMeaninglessCharacter();
		tuple<JsonObject::JsonType, unique_ptr<JsonValue> > readNumRet;
		parseAssert(iter != end, "invalid element");
		switch(*iter)
		{
            case '['  : type = JsonObject::typeList; value->listPtr.reset(new vector<JsonObject>(readList())); break;
			case '{'  : type = JsonObject::typeDict; value->dictPtr.reset(new std::unordered_map<string, JsonObject>(readDict())); break;
            case '\"' : type = JsonObject::typeString; value->stringPtr.reset(new string(readString())); break;
            case 't'  :
            case 'f'  : type = JsonObject::typeBool; value->boolValue = readBool(); break;
            case '0' :case '1' :case '2' :case '3' :case '4' :
			case '5' :case '6' :case '7' :case '8' :case '9' : case '+': case '-':
				readNumRet = readNum();
				type = std::get<0>(readNumRet);
				value = std::move(std::get<1>(readNumRet));
				break;
			default :
				parseAssert(false, string() + "unexcept character ss363" +*iter);
		}
		skipMeaninglessCharacter();
		return TypeValueTuple(type, std::move(value));
	};

	auto result = readJsonObject();
	parseAssert(iter == end, string() + "unexcept character ss366:");
	jsonType = std::get<0>(result);
	contentPtr = std::move(std::get<1>(result));
}