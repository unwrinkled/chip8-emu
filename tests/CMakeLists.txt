include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.17.0
  GIT_SHALLOW ON
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

include(GoogleTest)

macro(package_add_test TESTNAME)
  add_executable(${TESTNAME} ${ARGN})
  target_link_libraries(${TESTNAME} PRIVATE GTest::gtest GTest::gtest_main chip8_lib)
  target_include_directories(${TESTNAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)
  gtest_discover_tests(${TESTNAME}
    WORKING_DIRECTORY ${chip8_SOURCE_DIR}
  )
endmacro()

package_add_test(memory_tests memory_test.cpp)
