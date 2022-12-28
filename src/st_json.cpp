#pragma once
#include "st_json.h"

#include <cassert>
#include <cstdlib>
#include <stdio.h>
#include <errno.h>   /* errno, ERANGE */
#include <math.h>    /* HUGE_VAL */

using namespace ST_JSON;

static void ParseWhitespace(JsonContext* context) {
	const char* p = context->_json;
	if (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
		++p;
	context->_json = p;
}

#define EXPECT(context,ch) \
	do{ \
		assert(*(context->_json++)==ch); \
	}while(0)

static RetType ParseLiteral(JsonContext* context, JsonValue* val, JsonType type,const char* literal) {
	EXPECT(context,literal[0]);
	for(size_t i=1;literal[i];++i) {
		if(literal[i]!=*context->_json++) {
			return RetType::PARSE_INVALID_VALUE;
		}
	}
	val->_type = type;
	return RetType::PARSE_OK;
}

#define IS_DIGIT_1TO9(ch) ((ch) >= '1' && (ch) <= '9')

#define IS_DIGIT(ch) ((ch) >= '0' && (ch) <= '9')

#define PUTC(context,ch) \
	do{ \
		*static_cast<char*>((context)->Push(sizeof(char)))=(ch); \
	}while(0) \
	

static RetType ParseNumber(JsonContext* context, JsonValue* val) {
	const char* p = context->_json;
	if (*p == '-')
		++p;
	if (*p == '0')
		++p;
	else {
		if (!IS_DIGIT_1TO9(*p))
			return RetType::PARSE_INVALID_VALUE;
		else {
			for (++p; IS_DIGIT(*p); ++p) {}
		}
	}
	if (*p == '.') {
		++p;
		if (!IS_DIGIT(*p))
			return RetType::PARSE_INVALID_VALUE;
		else {
			for (++p; IS_DIGIT(*p); ++p) {}
		}
	}
	if (*p == 'e' || *p == 'E') {
		++p;
		if (*p == '+' || *p == '-')
			++p;
		if (!IS_DIGIT(*p))
			return RetType::PARSE_INVALID_VALUE;
		else {
			for (++p; IS_DIGIT(*p); ++p) {}
		}
	}
	errno        = 0;
	val->_number = strtod(context->_json, nullptr);
	if (errno == ERANGE && (val->_number == HUGE_VAL || val->_number == -HUGE_VAL))
		return RetType::PARSE_NUMBER_TOO_BIG;
	context->_json = p;
	val->_type     = JsonType::JSON_NUMBER;
	return RetType::PARSE_OK;
}


static const char* ParseHex4(const char* json,unsigned int* u) {
	*u=0;
	for(size_t i=0;i<4;++i) {
		char ch=*json++;
		*u<<=4;
		if(ch>='0'&& ch<='9') *u|=ch-'0';
		else if(ch>='A'&& ch<='F') *u|=ch-'A'+10;
		else if(ch>='a'&& ch<='f') *u|=ch-'a'+10;
		else return nullptr;
	}
	return json;
}

static void EncodeUtf8(JsonContext* context,unsigned int u) {
	if(u<=0x007F) {
		PUTC(context,u & 0x7F);
	}
	else if(u<=0x07FF) {
		PUTC(context,((u>>6)	& 0x1F)		|0xC0);
		PUTC(context,(u			& 0x3F)		|0x80);
	}
	else if(u<=0xFFFF) {
		PUTC(context,((u>>12)	& 0x0F)		|0xE0);
		PUTC(context,((u>>6)	& 0x3F)		|0x80);
		PUTC(context,(u			& 0x3F)		|0x80);
	}
	else {
		assert(u<=0x10FFFF);
		PUTC(context,((u>>18)	& 0x07)		|0xF0);
		PUTC(context,((u>>12)	& 0x3F)		|0x80);
		PUTC(context,((u>>6)	& 0x3F)		|0x80);
		PUTC(context,(u			& 0x3F)		|0x80);
	}
}

#define STRING_ERROR(ret) do { context->_top = cacheTop; return RetType::ret; } while(0)

static RetType ParseString(JsonContext* context,JsonValue* val) {
	size_t cacheTop=context->_top;
	EXPECT(context,'\"');
	const char* p = context->_json;
	unsigned int u,u2;
	for(;;) {
		char ch = *p++;
		switch(ch) {
			case '\"': {
				size_t size = context->_top-cacheTop;
				SetString(val, static_cast<const char*>(context->Pop(size)),size);
				context->_json=p;
				return RetType::PARSE_OK;
			}
			case '\0':
				context->_top=cacheTop;
				return RetType::PARSE_MISSING_QUOTATION_MARK;
			case '\\':
				switch(*p++) {
					case '\\': PUTC(context,'\\');break;
					case '\"': PUTC(context,'\"');break;
					case '/': PUTC(context,'/');break;
					case 'b': PUTC(context,'\b');break;
					case 'f': PUTC(context,'\f');break;
					case 'n': PUTC(context,'\n');break;
					case 'r': PUTC(context,'\r');break;
					case 't': PUTC(context,'\t');break;
					case 'u': {
						if(!(p=ParseHex4(p,&u))) {
							STRING_ERROR(PARSE_INVALID_UNICODE_HEX);
						}
						if(u>=0xD800 && u<= 0xDBFF) {
							if(*p++ !='\\') {
								STRING_ERROR(PARSE_INVALID_UNICODE_SURROGATE);
							}
							if(*p++!='u') {
								STRING_ERROR(PARSE_INVALID_UNICODE_SURROGATE);
							}
							if(!(p=ParseHex4(p,&u2))) {
								STRING_ERROR(PARSE_INVALID_UNICODE_HEX);
							}
							if(u2< 0xDC00 || u2 > 0xDFFF) {
								STRING_ERROR(PARSE_INVALID_UNICODE_SURROGATE);
							}
							u = (((u - 0xD800) << 10) | (u2 - 0xDC00)) + 0x10000;
						}
						EncodeUtf8(context,u);
						break;
					}
					default:
						context->_top=cacheTop;
					return RetType::PARSE_INVALID_STRING_ESCAPE;
				}
				break;
			default:
				if((unsigned char)ch<0x20) {
					context->_top=cacheTop;
					return RetType::PARSE_INVALID_STRING_CHAR;
				}
				PUTC(context,ch);
		}
	}
}


static RetType ParseJsonValue(JsonContext* context, JsonValue* val);

static RetType ParseArray(JsonContext* context, JsonValue* val) {
	size_t size=0;
	RetType ret;
	EXPECT(context,'[');
	ParseWhitespace(context);
	if(*context->_json==']') {
		++context->_json;
		val->_type=JsonType::JSON_ARRAY;
		val->_arrData=nullptr;
		val->_arrSize=0;
		return RetType::PARSE_OK;
	}
	for(;;) {
		JsonValue v;
		v.Init();
		if((ret=ParseJsonValue(context,&v))!=RetType::PARSE_OK){
			return ret;
		}
		memcpy(context->Push(sizeof(JsonValue)),&v,sizeof(JsonValue));
		++size;
		ParseWhitespace(context);
		if(*context->_json==',') {
			++context->_json;
			ParseWhitespace(context);
		}
		else if(*context->_json==']') {
			++context->_json;
			val->_type=JsonType::JSON_ARRAY;
			val->_arrSize=size;
			size*=sizeof(JsonValue);
			val->_arrData=(JsonValue*)malloc(size);
			memcpy(val->_arrData,context->Pop(size),size);
			return RetType::PARSE_OK;
		}
		else
			return RetType::PARSE_MISSING_COMMA_OR_SQUARE_BRACKET;
	}
}

static RetType ParseJsonValue(JsonContext* context, JsonValue* val) {
	switch (*context->_json) {
		case 'n': return ParseLiteral(context, val, JsonType::JSON_NULL,"null");
		case 'f': return ParseLiteral(context, val, JsonType::JSON_FALSE,"false");
		case 't': return ParseLiteral(context, val, JsonType::JSON_TRUE,"true");
		case '"': return ParseString(context,val);
		case '[': return ParseArray(context,val);
		default: return ParseNumber(context, val);
		case '\0': return RetType::PARSE_EXPECT_VALUE;
	}
}

void* JsonContext::Push(size_t size) {
	void* ret;
	assert(size!=0);
	if (_top + size >= _size) {
		if (_size == 0)
			_size = JSON_STACK_INIT_SIZE;
		while (_top + size >= _size)
			_size += _size >> 1;
		_stack = (char*)realloc(_stack, _size);
	}
	ret = _stack + _top;
	_top += size;
	return ret;
}

void* JsonContext::Pop(size_t size) {
	assert(_top>=size);
	_top -= size;
	return _stack + _top;
}

void ST_JSON::JsonInit(JsonValue* val) {}

void ST_JSON::JsonFree(JsonValue* val) {}

RetType ST_JSON::JsonParse(JsonValue* val, const char* json) {
	assert(val!=nullptr);
	JsonContext c;
	c._json    = json;
	c._stack =nullptr;
	c._size =0;
	c._top=0;

	val->Init();
	ParseWhitespace(&c);
	RetType ret;
	if((ret=ParseJsonValue(&c,val))==RetType::PARSE_OK) {
		ParseWhitespace(&c);
		if(*c._json!='\0') {
			val->_type=JsonType::JSON_NULL;
			return RetType::PARSE_ROOT_NOT_SINGULAR;
		}
	}
	assert(c._top==0);
	free(c._stack);
	return ret;
}

JsonType ST_JSON::GetType(const JsonValue* val) {
	assert(val!=nullptr);
	return val->_type;
}

double ST_JSON::GetNumber(const JsonValue* val) {
	assert(val&&val->_type==JsonType::JSON_NUMBER);
	return val->_number;
}

bool ST_JSON::GetBoolean(const JsonValue* val) {
	assert(val&&(val->_type==JsonType::JSON_TRUE||val->_type==JsonType::JSON_FALSE));
	return val->_type==JsonType::JSON_TRUE ? true:false;
}

const char* ST_JSON::GetString(const JsonValue* val) {
	assert(val&&val->_type==JsonType::JSON_STRING);
	return val->_str;
}

size_t ST_JSON::GetStringLength(const JsonValue* val) {
	assert(val&&val->_type==JsonType::JSON_STRING);
	return val->_strSize;
}

size_t ST_JSON::GetArraySize(const JsonValue* val) {
	assert(val&&val->_type==JsonType::JSON_ARRAY);
	return val->_arrSize;
}

JsonValue* ST_JSON::GetArrayElement(const JsonValue* val, size_t index) {
	assert(val&&val->_type==JsonType::JSON_ARRAY&&index<val->_arrSize);
	return &val->_arrData[index];
}

void ST_JSON::SetBoolean(JsonValue* val, bool b) {
	assert(val);
	val->Free();
	val->_type = b? JsonType::JSON_TRUE:JsonType::JSON_FALSE;
}

void ST_JSON::SetNumber(JsonValue* val, double n) {
	assert(val);
	val->Free();
	val->_type=JsonType::JSON_NUMBER;
	val->_number=n;
}

void ST_JSON::SetString(JsonValue* val, const char* str,size_t size) {
	assert(val&&(str||size==0));
	val->Free();
	val->_str = (char*)malloc(size+1);
	memcpy(val->_str,str,size);
	val->_str[size]='\0';
	val->_strSize=size;
	val->_type=JsonType::JSON_STRING;
}
