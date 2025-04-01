set(UFONT_UNIT_TESTS ufont_tests)

include(CTest)

file(GLOB TARGET_SRC "./tests.cpp" )

add_executable(${UFONT_UNIT_TESTS} ${TARGET_SRC})

add_test(${UFONT_UNIT_TESTS} ${UFONT_UNIT_TESTS})