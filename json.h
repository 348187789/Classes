#ifndef __JSON_H__
#define __JSON_H__

#include <string>
#include <array>
#include <unordered_map>
#include <vector>
#include <memory>
#include <utility>


class JsonErrorCode
{
private:
	bool hasError;
	std::string message;
public:
	JsonErrorCode() : hasError(false) {}
	JsonErrorCode(std::string inputMessage)
		: hasError(true)
		, message(std::move(inputMessage))
	{
	}
	void reset() { hasError = false; std::string s; message.swap(s); }
	void reset(std::string str) { hasError = true; message = std::move(str); }
	const std::string& what() { return message; }
	operator bool() { return hasError; }
};


class JsonObject;
struct JsonValue
{
    bool boolValue;
    int  intValue;
    float floatValue;
    std::unique_ptr< std::string > stringPtr;
    std::unique_ptr< std::unordered_map<std::string, JsonObject> > dictPtr;
    std::unique_ptr< std::vector<JsonObject> > listPtr;
};
class JsonObject
{
public:
	
	enum JsonType
	{
		typeBool = 0,
		typeInt,
		typeFloat,
		typeString,
		typeList,
		typeDict,
		typeEmpty 
	};

	JsonObject() : type_(typeEmpty){}

	JsonObject(const char* filename, JsonErrorCode& e);

	JsonObject(std::tuple<JsonType, std::unique_ptr<JsonValue> > t);


	JsonObject(JsonObject&& j)
		: type_(j.type_)
		, contentPtr_(std::move(j.contentPtr_))
	{}

	JsonObject& operator=(JsonObject&& j)
	{
		type_ = j.type_;
		contentPtr_ = std::move(j.contentPtr_);
		return *this;
	}

	
	 
	bool& toBool() const;
	const int& toInt() const;
	const float& toFloat() const;
	const std::string& toString() const;
	const std::unordered_map<std::string, JsonObject>& toDict() const;
	const std::vector<JsonObject>& toList() const;

	JsonObject& operator[](const std::string& str) const;
	JsonObject& operator[](int index) const;

	int getListSize() const;
	int getDictSize() const;

    inline bool isBool()   const { return type_ == typeBool;   } 
    inline bool isInt()    const { return type_ == typeInt;    } 
    inline bool isFloat()  const { return type_ == typeFloat;  } 
    inline bool isString() const { return type_ == typeString; }
    inline bool isList()   const { return type_ == typeList;   }
    inline bool isDict()   const { return type_ == typeDict;   }
    inline bool isEmpty()  const { return type_ == typeEmpty;  }
	
	const std::string& type() const 
	{
		return nameOfType[type_];
	}

private:

	static const std::array<std::string, 7> nameOfType;

private: // member var
	JsonObject(const JsonObject&);
	JsonObject& operator=(const JsonObject&);

	JsonType type_;

	std::unique_ptr<JsonValue> contentPtr_;

}; // class JsonObject

inline const std::string& type(const JsonObject& o) 
{
	return o.type();
}

inline void operator<<(bool& lhs, const JsonObject& rhs)
{
	lhs = rhs.toBool();
}
inline void operator<<(int& lhs, const JsonObject& rhs)
{
	lhs = rhs.toInt();
}
inline void operator<<(float& lhs, const JsonObject& rhs)
{
	lhs = rhs.toFloat();
}
inline void operator<<(std::string& lhs,  const JsonObject& rhs)
{
	lhs = rhs.toString();
}



#endif // !__JSON_H__
