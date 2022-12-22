add_executable(ST_JSON_TEST "test.cpp")
set_target_properties("ST_JSON_TEST" PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries("ST_JSON_TEST" ST_JSON_SRC ST_UNIT_TEST)

add_test(
  NAME
    test_st_json
  COMMAND
  $<TARGET_FILE:ST_JSON_TEST>
)
