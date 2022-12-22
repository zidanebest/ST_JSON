
add_library(ST_JSON_SRC "")
set_target_properties("ST_JSON_SRC" PROPERTIES LINKER_LANGUAGE CXX)

target_sources(ST_JSON_SRC
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/st_json.h
    ${CMAKE_CURRENT_LIST_DIR}/st_json.cpp
)

target_include_directories(ST_JSON_SRC
PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)