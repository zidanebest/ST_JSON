#pragma once
#include <string>
using std::string;

#define JSON_STACK_INIT_SIZE 256

namespace ST_JSON {
enum class JsonType {
	JSON_NULL=0,
	JSON_TRUE,
	JSON_FALSE,
	JSON_NUMBER,
	JSON_STRING,
	JSON_ARRAY,
	JSON_OBJECT
};

enum class RetType {
	PARSE_OK=0,
	PARSE_EXPECT_VALUE,
	PARSE_INVALID_VALUE,
	PARSE_ROOT_NOT_SINGULAR,
	PARSE_NUMBER_TOO_BIG,
	PARSE_MISSING_QUOTATION_MARK,
	PARSE_INVALID_STRING_ESCAPE,
	PARSE_INVALID_STRING_CHAR,
	PARSE_INVALID_UNICODE_SURROGATE,
	PARSE_INVALID_UNICODE_HEX,
	PARSE_MISSING_COMMA_OR_SQUARE_BRACKET,
};

struct JsonValue {
	union {
		double _number;
		struct {
			JsonValue* _arrData;
			size_t _arrSize;
		};
		struct {
			char* _str;
			size_t _strSize;
		};
	};
	void Init() {
		_type = JsonType::JSON_NULL;
	}
	void Free() {
		switch (_type) {
			case JsonType::JSON_STRING: {
				free(_str);
				break;
			}
			case JsonType::JSON_ARRAY: {
				for(size_t i=0;i<_arrSize;++i) {
					_arrData[i].Free();
				}
				free(_arrData);
			}
		}
		_type=JsonType::JSON_NULL;
	}
	
	JsonType _type;
};

struct JsonContext {
	const char* _json;

	char* _stack;

	size_t _size, _top;
	
	void* Push(size_t size);
	
	void* Pop(size_t size);
};

void JsonInit(JsonValue* val);

void JsonFree(JsonValue* val);

RetType JsonParse(JsonValue* val, const char* json);

JsonType GetType(const JsonValue* val);

double GetNumber(const JsonValue* val);

bool GetBoolean(const JsonValue* val);

const char* GetString(const JsonValue* val);

size_t GetStringLength(const JsonValue* val);

size_t GetArraySize(const JsonValue* val);

JsonValue* GetArrayElement(const JsonValue* val,size_t index);

void SetBoolean(JsonValue* val, bool b);

void SetNumber(JsonValue* val, double n);

void SetString(JsonValue* val, const char* str,size_t size);

}
