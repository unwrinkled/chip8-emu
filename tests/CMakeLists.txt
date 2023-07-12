find_package(GTest CONFIG REQUIRED)

macro(package_add_test TESTNAME)
  add_executable(${TESTNAME} ${ARGN})
  target_link_libraries(${TESTNAME} PRIVATE GTest::gtest GTest::gtest_main chip8_lib)
  gtest_discover_tests(${TESTNAME}
    WORKING_DIRECTORY ${chip8_SOURCE_DIR}
  )
endmacro()

package_add_test(memory_tests memory_test.cpp)
