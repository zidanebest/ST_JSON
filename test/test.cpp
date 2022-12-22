#include<iostream>

#include "st_json.h"
#include "../3rd/ST_UNIT_TEST/st_unit_test.hpp"
using namespace std;
using namespace ST_UNIT_TEST;
using namespace ST_JSON;

static void TestParseNull() {
	JsonValue v;
	v._type=JsonType::JSON_FALSE;
	ST_EXPECT_EQ_INT(RetType::PARSE_OK,JsonParse(&v,"null"));
	ST_EXPECT_EQ_INT(JsonType::JSON_NULL,GetType(&v));
}

int main() {
	TestParseNull();
	ST_LOG_STAT();
	
	return 0;
}