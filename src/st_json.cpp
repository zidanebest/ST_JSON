#pragma once
#include "st_json.h"

#include <cassert>
using namespace ST_JSON;

static void ParseWhitespace(JsonContext* context) {
	const char* p=context->json;
	if(*p==' '||*p=='\t'||*p=='\n'||*p=='\r')
		++p;
	context->json=p;
}

#define EXPECT(context,ch) \
	do{ \
		assert(*(context->json++)==ch); \
	}while(0)\

static RetType ParseNull(JsonContext* context,JsonValue* val) {
	EXPECT(context,'n');
	if(context->json[0]=='u'&&context->json[1]=='l'&&context->json[2]=='l') {
		context->json+=3;
		val->_type=JsonType::JSON_NULL;
		return RetType::PARSE_OK;
	}
	return RetType::PARSE_INVALID_VALUE;
}

static RetType ParseJsonValue(JsonContext* context,JsonValue* val) {
	switch (*context->json) {
		case 'n': return ParseNull(context,val);
		case '\0': return RetType::PARSE_EXPECT_VALUE;
		default: return RetType::PARSE_INVALID_VALUE;
	}
}

RetType ST_JSON::JsonParse(JsonValue* val, const char* json) {
	assert(val!=nullptr);
	JsonContext c;
	c.json=json;
	val->_type=JsonType::JSON_NULL;
	ParseWhitespace(&c);
	return ParseJsonValue(&c,val);
}

JsonType ST_JSON::GetType(const JsonValue* val) {
	assert(val!=nullptr);
	return val->_type;
}
