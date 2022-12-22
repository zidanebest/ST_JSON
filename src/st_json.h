#pragma once

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
	PARSE_NUMBER_TOO_BIG
};

struct JsonValue {
	double _number;
	JsonType _type;
};

struct JsonContext {
	const char* json;
};

RetType JsonParse(JsonValue* val, const char* json);

JsonType GetType(const JsonValue* val);

double GetNumber(const JsonValue* val);

}
