#include<iostream>

#include "st_json.h"
#include "../3rd/ST_UNIT_TEST/st_unit_test.h"
using namespace std;
using namespace ST_UNIT_TEST;
using namespace ST_JSON;

#define TEST_NUMBER(value,json) \
do { \
JsonValue v; \
ST_EXPECT_EQ_INT(RetType::PARSE_OK, JsonParse(&v,json)); \
ST_EXPECT_EQ_INT(JsonType::JSON_NUMBER, GetType(&v)); \
ST_EXPECT_EQ_DOUBLE(value,GetNumber(&v)); \
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

int main() {
	TestParseLiteral();
	TestParseNumber();
	TestInValidNumber();
	TestParseNumberTooBig();
	ST_LOG_STAT();

	return 0;
}
