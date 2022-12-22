#pragma once
#include "st_json.h"

#include <cassert>
#include <cstdlib>
#include <stdio.h>
#include <errno.h>   /* errno, ERANGE */
#include <math.h>    /* HUGE_VAL */

using namespace ST_JSON;

static void ParseWhitespace(JsonContext* context) {
	const char* p = context->json;
	if (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
		++p;
	context->json = p;
}

#define EXPECT(context,ch) \
	do{ \
		assert(*(context->json++)==ch); \
	}while(0)

static RetType ParseLiteral(JsonContext* context, JsonValue* val, JsonType type) {
	switch (type) {
		case JsonType::JSON_NULL: {
			EXPECT(context, 'n');
			if (context->json[0] == 'u' && context->json[1] == 'l' && context->json[2] == 'l') {
				context->json += 3;
				val->_type = JsonType::JSON_NULL;
				return RetType::PARSE_OK;
			}
			break;
		}
		case JsonType::JSON_TRUE: {
			EXPECT(context, 't');
			if (context->json[0] == 'r' && context->json[1] == 'u' && context->json[2] == 'e') {
				context->json += 3;
				val->_type = JsonType::JSON_TRUE;
				return RetType::PARSE_OK;
			}
			break;
		}
		case JsonType::JSON_FALSE: {
			EXPECT(context, 'f');
			if (context->json[0] == 'a' && context->json[1] == 'l' && context->json[2] == 's' && context->json[3] ==
			    'e') {
				context->json += 4;
				val->_type = JsonType::JSON_FALSE;
				return RetType::PARSE_OK;
			}
			break;
		}
		default: return RetType::PARSE_INVALID_VALUE;
	}

	return RetType::PARSE_INVALID_VALUE;
}

#define IS_DIGIT_1TO9(ch) ((ch) >= '1' && (ch) <= '9')

#define IS_DIGIT(ch) ((ch) >= '0' && (ch) <= '9')

static RetType ParseNumber(JsonContext* context, JsonValue* val) {
	const char* p=context->json;
	if(*p=='-')
		++p;
	if(*p=='0')
		++p;
	else {
		if(!IS_DIGIT_1TO9(*p))
			return RetType::PARSE_INVALID_VALUE;
		else {
			for(++p;IS_DIGIT(*p);++p){}
		}
	}
	if(*p=='.') {
		++p;
		if(!IS_DIGIT(*p))
			return RetType::PARSE_INVALID_VALUE;
		else {
			for(++p;IS_DIGIT(*p);++p){}
		}
	}
	if(*p=='e'||*p=='E') {
		++p;
		if(*p=='+'||*p=='-')
			++p;
		if(!IS_DIGIT(*p))
			return RetType::PARSE_INVALID_VALUE;
		else {
			for(++p;IS_DIGIT(*p);++p){}
		}
	}
	errno = 0;
	val->_number = strtod(context->json,nullptr);
	if (errno == ERANGE && (val->_number == HUGE_VAL || val->_number == -HUGE_VAL))
		return RetType::PARSE_NUMBER_TOO_BIG;
	context->json = p;
	val->_type    = JsonType::JSON_NUMBER;
	return RetType::PARSE_OK;
}

static RetType ParseJsonValue(JsonContext* context, JsonValue* val) {
	switch (*context->json) {
		case 'n': return ParseLiteral(context, val, JsonType::JSON_NULL);
		case 'f': return ParseLiteral(context, val, JsonType::JSON_FALSE);
		case 't': return ParseLiteral(context, val, JsonType::JSON_TRUE);
		default: return ParseNumber(context, val);
		case '\0': return RetType::PARSE_EXPECT_VALUE;
	}
}

RetType ST_JSON::JsonParse(JsonValue* val, const char* json) {
	assert(val!=nullptr);
	JsonContext c;
	c.json     = json;
	val->_type = JsonType::JSON_NULL;
	ParseWhitespace(&c);
	return ParseJsonValue(&c, val);
}

JsonType ST_JSON::GetType(const JsonValue* val) {
	assert(val!=nullptr);
	return val->_type;
}

double ST_JSON::GetNumber(const JsonValue* val) {
	assert(val&&val->_type==JsonType::JSON_NUMBER);
	return val->_number;
}
