#include<iostream>

#include "st_json.h"
#include "../3rd/ST_UNIT_TEST/st_unit_test.h"
using namespace std;
using namespace ST_UNIT_TEST;
using namespace ST_JSON;

#ifdef _WINDOWS
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#define TEST_NUMBER(value,json) \
	do { \
		JsonValue v; \
		ST_EXPECT_EQ_INT(RetType::PARSE_OK, JsonParse(&v,json)); \
		ST_EXPECT_EQ_INT(JsonType::JSON_NUMBER, GetType(&v)); \
		ST_EXPECT_EQ_DOUBLE(value,GetNumber(&v)); \
	}while(0)

#define TEST_STRING(value,json) \
	do { \
		JsonValue v; \
		v.Init(); \
		ST_EXPECT_EQ_INT(RetType::PARSE_OK, JsonParse(&v,json)); \
		ST_EXPECT_EQ_INT(JsonType::JSON_STRING, GetType(&v)); \
		ST_EXPECT_EQ_C_STR((value),GetString(&v),GetStringLength(&v)); \
		v.Free(); \
	}while(0)

#define TEST_ERROR(retType,json)  \
	do { \
		JsonValue v; \
		ST_EXPECT_EQ_INT(RetType::retType, JsonParse(&v,json)); \
		ST_EXPECT_EQ_INT(JsonType::JSON_NULL, GetType(&v)); \
	}while(0)

static void TestParseLiteral() {
	printf("Test parse literal \n");
	JsonValue v;
	v._type = JsonType::JSON_FALSE;
	ST_EXPECT_EQ_INT(RetType::PARSE_OK, JsonParse(&v,"null"));
	ST_EXPECT_EQ_INT(JsonType::JSON_NULL, GetType(&v));

	ST_EXPECT_EQ_INT(RetType::PARSE_OK, JsonParse(&v,"true"));
	ST_EXPECT_EQ_INT(JsonType::JSON_TRUE, GetType(&v));

	ST_EXPECT_EQ_INT(RetType::PARSE_OK, JsonParse(&v,"false"));
	ST_EXPECT_EQ_INT(JsonType::JSON_FALSE, GetType(&v));
}

static void TestParseNumber() {
	printf("Test parse number \n");
	TEST_NUMBER(0.0, "0");
	TEST_NUMBER(0.0, "-0");
	TEST_NUMBER(0.0, "-0.0");
	TEST_NUMBER(1.0, "1");
	TEST_NUMBER(-1.0, "-1");
	TEST_NUMBER(1.5, "1.5");
	TEST_NUMBER(-1.5, "-1.5");
	TEST_NUMBER(3.1416, "3.1416");
	TEST_NUMBER(1E10, "1E10");
	TEST_NUMBER(1e10, "1e10");
	TEST_NUMBER(1E+10, "1E+10");
	TEST_NUMBER(1E-10, "1E-10");
	TEST_NUMBER(-1E10, "-1E10");
	TEST_NUMBER(-1e10, "-1e10");
	TEST_NUMBER(-1E+10, "-1E+10");
	TEST_NUMBER(-1E-10, "-1E-10");
	TEST_NUMBER(1.234E+10, "1.234E+10");
	TEST_NUMBER(1.234E-10, "1.234E-10");
	TEST_NUMBER(0.0, "1e-10000"); /* must underflow */

	TEST_NUMBER(1.0000000000000002, "1.0000000000000002");           /* the smallest number > 1 */
	TEST_NUMBER(4.9406564584124654e-324, "4.9406564584124654e-324"); /* minimum denormal */
	TEST_NUMBER(-4.9406564584124654e-324, "-4.9406564584124654e-324");
	TEST_NUMBER(2.2250738585072009e-308, "2.2250738585072009e-308"); /* Max subnormal double */
	TEST_NUMBER(-2.2250738585072009e-308, "-2.2250738585072009e-308");
	TEST_NUMBER(2.2250738585072014e-308, "2.2250738585072014e-308"); /* Min normal positive double */
	TEST_NUMBER(-2.2250738585072014e-308, "-2.2250738585072014e-308");
	TEST_NUMBER(1.7976931348623157e+308, "1.7976931348623157e+308"); /* Max double */
	TEST_NUMBER(-1.7976931348623157e+308, "-1.7976931348623157e+308");
}

static void TestInValidNumber() {
	printf("Test invalid number \n");
	TEST_ERROR(PARSE_INVALID_VALUE, "+0");
	TEST_ERROR(PARSE_INVALID_VALUE, "+1");
	TEST_ERROR(PARSE_INVALID_VALUE, ".123"); /* at least one digit before '.' */
	TEST_ERROR(PARSE_INVALID_VALUE, "1.");   /* at least one digit after '.' */
	TEST_ERROR(PARSE_INVALID_VALUE, "INF");
	TEST_ERROR(PARSE_INVALID_VALUE, "inf");
	TEST_ERROR(PARSE_INVALID_VALUE, "NAN");
	TEST_ERROR(PARSE_INVALID_VALUE, "nan");
}

static void TestParseNumberTooBig() {
	printf("Test parse number too big \n");
	TEST_ERROR(PARSE_NUMBER_TOO_BIG, "1e309");
	TEST_ERROR(PARSE_NUMBER_TOO_BIG, "-1e309");

}

static void TestParseInValidValue() {
	printf("Test parse invalid value \n");
	TEST_ERROR(PARSE_INVALID_VALUE, "nul");
	TEST_ERROR(PARSE_INVALID_VALUE, "?");

	/* invalid number */
	TEST_ERROR(PARSE_INVALID_VALUE, "+0");
	TEST_ERROR(PARSE_INVALID_VALUE, "+1");
	TEST_ERROR(PARSE_INVALID_VALUE, ".123"); /* at least one digit before '.' */
	TEST_ERROR(PARSE_INVALID_VALUE, "1.");   /* at least one digit after '.' */
	TEST_ERROR(PARSE_INVALID_VALUE, "INF");
	TEST_ERROR(PARSE_INVALID_VALUE, "inf");
	TEST_ERROR(PARSE_INVALID_VALUE, "NAN");
	TEST_ERROR(PARSE_INVALID_VALUE, "nan");
}

static void TestParseString() {
	TEST_STRING("", "\"\"");
	TEST_STRING("Hello", "\"Hello\"");
	TEST_STRING("Hello\nWorld", "\"Hello\\nWorld\"");
	TEST_STRING("\" \\ / \b \f \n \r \t", "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");
	TEST_STRING("Hello\0World", "\"Hello\\u0000World\"");
	TEST_STRING("\x24", "\"\\u0024\"");                    /* Dollar sign U+0024 */
	TEST_STRING("\xC2\xA2", "\"\\u00A2\"");                /* Cents sign U+00A2 */
	TEST_STRING("\xE2\x82\xAC", "\"\\u20AC\"");            /* Euro sign U+20AC */
	TEST_STRING("\xF0\x9D\x84\x9E", "\"\\uD834\\uDD1E\""); /* G clef sign U+1D11E */
	TEST_STRING("\xF0\x9D\x84\x9E", "\"\\ud834\\udd1e\""); /* G clef sign U+1D11E */
}

static void TestParseMissingQuotationMark() {
	TEST_ERROR(PARSE_MISSING_QUOTATION_MARK, "\"");
	TEST_ERROR(PARSE_MISSING_QUOTATION_MARK, "\"abc");
}

static void TestAccessBoolean() {
	JsonValue v;
	v.Init();
	SetString(&v, "a", 1);
	SetBoolean(&v, true);
	ST_EXPECT_TRUE(GetBoolean(&v));
	SetBoolean(&v, false);
	ST_EXPECT_FALSE(GetBoolean(&v));
	v.Free();
}

static void TestInvalidStringEscape() {
	TEST_ERROR(PARSE_INVALID_STRING_ESCAPE, "\"\\v\"");
	TEST_ERROR(PARSE_INVALID_STRING_ESCAPE, "\"\\'\"");
	TEST_ERROR(PARSE_INVALID_STRING_ESCAPE, "\"\\0\"");
	TEST_ERROR(PARSE_INVALID_STRING_ESCAPE, "\"\\x12\"");
}

static void TestInvalidStringChar() {
	TEST_ERROR(PARSE_INVALID_STRING_CHAR, "\"\x01\"");
	TEST_ERROR(PARSE_INVALID_STRING_CHAR, "\"\x1F\"");
}

static void TestParseInvalidUnicodeHex() {
	TEST_ERROR(PARSE_INVALID_UNICODE_HEX, "\"\\u\"");
	TEST_ERROR(PARSE_INVALID_UNICODE_HEX, "\"\\u0\"");
	TEST_ERROR(PARSE_INVALID_UNICODE_HEX, "\"\\u01\"");
	TEST_ERROR(PARSE_INVALID_UNICODE_HEX, "\"\\u012\"");
	TEST_ERROR(PARSE_INVALID_UNICODE_HEX, "\"\\u/000\"");
	TEST_ERROR(PARSE_INVALID_UNICODE_HEX, "\"\\uG000\"");
	TEST_ERROR(PARSE_INVALID_UNICODE_HEX, "\"\\u0/00\"");
	TEST_ERROR(PARSE_INVALID_UNICODE_HEX, "\"\\u0G00\"");
	TEST_ERROR(PARSE_INVALID_UNICODE_HEX, "\"\\u00/0\"");
	TEST_ERROR(PARSE_INVALID_UNICODE_HEX, "\"\\u00G0\"");
	TEST_ERROR(PARSE_INVALID_UNICODE_HEX, "\"\\u000/\"");
	TEST_ERROR(PARSE_INVALID_UNICODE_HEX, "\"\\u000G\"");
	TEST_ERROR(PARSE_INVALID_UNICODE_HEX, "\"\\u 123\"");
}

static void TestParseInvalidUnicodeSurrogate() {
	// TEST_ERROR(PARSE_INVALID_UNICODE_SURROGATE, "\"\\uD800\"");
	TEST_ERROR(PARSE_INVALID_UNICODE_SURROGATE, "\"\\uDBFF\"");
	// TEST_ERROR(PARSE_INVALID_UNICODE_SURROGATE, "\"\\uD800\\\\\"");
	// TEST_ERROR(PARSE_INVALID_UNICODE_SURROGATE, "\"\\uD800\\uDBFF\"");
	// TEST_ERROR(PARSE_INVALID_UNICODE_SURROGATE, "\"\\uD800\\uE000\"");
}

static void TestParseArray() {
	JsonValue v;
	v.Init();
	ST_EXPECT_EQ_INT(RetType::PARSE_OK, JsonParse(&v, "[ null , false , true , 123 , \"abc\" ]"));
	JsonValue* v2 = GetArrayElement(&v, 0);
	ST_EXPECT_EQ_INT(JsonType::JSON_ARRAY, GetType(&v));
	ST_EXPECT_EQ_SIZE_T(5, GetArraySize(&v));
	ST_EXPECT_EQ_INT(JsonType::JSON_NULL, GetType(GetArrayElement(&v, 0)));
	ST_EXPECT_EQ_INT(JsonType::JSON_FALSE, GetType(GetArrayElement(&v, 1)));
	ST_EXPECT_EQ_INT(JsonType::JSON_TRUE, GetType(GetArrayElement(&v, 2)));
	ST_EXPECT_EQ_INT(JsonType::JSON_NUMBER, GetType(GetArrayElement(&v, 3)));
	ST_EXPECT_EQ_INT(JsonType::JSON_STRING, GetType(GetArrayElement(&v, 4)));
	ST_EXPECT_EQ_DOUBLE(123.0, GetNumber(GetArrayElement(&v, 3)));
	ST_EXPECT_EQ_C_STR("abc", GetString(GetArrayElement(&v, 4)), GetStringLength(GetArrayElement(&v, 4)));
	v.Free();

	v.Init();
	ST_EXPECT_EQ_INT(RetType::PARSE_OK, JsonParse(&v, "[ [ ] , [ 0 ] , [ 0 , 1 ] , [ 0 , 1 , 2 ] ]"));
	ST_EXPECT_EQ_INT(JsonType::JSON_ARRAY, GetType(&v));
	ST_EXPECT_EQ_SIZE_T(4, GetArraySize(&v));
	for (size_t i = 0; i < 4; i++) {
		JsonValue* a = GetArrayElement(&v, i);
		ST_EXPECT_EQ_INT(JsonType::JSON_ARRAY, GetType(a));
		ST_EXPECT_EQ_SIZE_T(i, GetArraySize(a));
		for (size_t j = 0; j < i; j++) {
			JsonValue* e = GetArrayElement(a, j);
			ST_EXPECT_EQ_INT(JsonType::JSON_NUMBER, GetType(e));
			ST_EXPECT_EQ_DOUBLE((double)j, GetNumber(e));
		}
	}
	v.Free();
}

int main() {
#ifdef _WINDOWS
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(90);
#endif
	TestParseLiteral();
	TestParseNumber();
	TestInValidNumber();
	TestParseNumberTooBig();
	TestParseMissingQuotationMark();
	TestParseInValidValue();
	TestParseString();

	TestAccessBoolean();
	TestInvalidStringEscape();
	TestInvalidStringChar();
	TestParseInvalidUnicodeHex();
	TestParseInvalidUnicodeSurrogate();
	TestParseArray();
	ST_LOG_STAT();

	return 0;
}
