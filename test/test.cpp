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

#define TEST_STRING(value,json) \
	do { \
		JsonValue v; \
		ST_EXPECT_EQ_INT(RetType::PARSE_OK, JsonParse(&v,json)); \
		ST_EXPECT_EQ_INT(JsonType::JSON_STRING, GetType(&v)); \
		ST_EXPECT_EQ_C_STR((value),GetString(&v),GetStringLength(&v)); \
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

	TEST_NUMBER(1.0000000000000002, "1.0000000000000002"); /* the smallest number > 1 */
	TEST_NUMBER( 4.9406564584124654e-324, "4.9406564584124654e-324"); /* minimum denormal */
	TEST_NUMBER(-4.9406564584124654e-324, "-4.9406564584124654e-324");
	TEST_NUMBER( 2.2250738585072009e-308, "2.2250738585072009e-308");  /* Max subnormal double */
	TEST_NUMBER(-2.2250738585072009e-308, "-2.2250738585072009e-308");
	TEST_NUMBER( 2.2250738585072014e-308, "2.2250738585072014e-308");  /* Min normal positive double */
	TEST_NUMBER(-2.2250738585072014e-308, "-2.2250738585072014e-308");
	TEST_NUMBER( 1.7976931348623157e+308, "1.7976931348623157e+308");  /* Max double */
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
	TEST_STRING("","\"\"");
	TEST_STRING("Hello","\"Hello\"");
}

static void TestParseMissingQuotationMark() {
	TEST_ERROR(PARSE_MISSING_QUOTATION_MARK, "\"");
	TEST_ERROR(PARSE_MISSING_QUOTATION_MARK, "\"abc");
}

int main() {
	// TestParseLiteral();
	// TestParseNumber();
	// TestInValidNumber();
	// TestParseNumberTooBig();
	TestParseMissingQuotationMark();
	// TestParseInValidValue();
	// TestParseString();

	ST_LOG_STAT();

	return 0;
}
