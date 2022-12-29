#pragma once
#include <string>
using std::string;

#define JSON_PARSE_STACK_INIT_SIZE 256
#define JSON_STRINGIFY_STACK_INIT_SIZE 256

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
	PARSE_MISSING_COLON,
	LEPT_PARSE_MISS_COMMA_OR_CURLY_BRACKET,
	PARSE_MISSING_KEY
};


struct JsonObjMember;
struct JsonValue {
	void Init();

	void Free();

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

		struct {
			JsonObjMember* _objData;

			size_t _objSize;
		};
	};

	JsonType _type;
};

struct JsonObjMember {
	char* _key;

	size_t _keySize;

	JsonValue _val;

	void Free() {
		free(_key);
		_val.Free();
	}
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

char* JsonStringify(const JsonValue* val,size_t* size);

JsonType GetType(const JsonValue* val);

double GetNumber(const JsonValue* val);

bool GetBoolean(const JsonValue* val);

char const* GetString(const JsonValue* val);

size_t GetStringSize(const JsonValue* val);

size_t GetArraySize(const JsonValue* val);

JsonValue* GetArrayElement(const JsonValue* val, size_t index);

size_t GetObjSize(const JsonValue* val);

char const* GetObjKey(const JsonValue* val, size_t index);

size_t GetObjKeySize(const JsonValue* val, size_t index);

JsonValue* GetObjValue(const JsonValue* val, size_t index);

void SetBoolean(JsonValue* val, bool b);

void SetNumber(JsonValue* val, double n);

void SetString(JsonValue* val, const char* str, size_t size);

}
