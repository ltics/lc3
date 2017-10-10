cmake_minimum_required(VERSION 3.1)
project(lc3-lang)

set(CATCH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/catch/single_include)
set(RANGEV3_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/range-v3/include)
set(FMT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/fmt)

include_directories(
  ${CATCH_INCLUDE_DIR}
  ${RANGEV3_INCLUDE_DIR}
  ${FMT_INCLUDE_DIR}
  )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14")
add_executable(lc3 src/main.cc)
target_link_libraries(lc3 edit)

add_executable(tests test/tests.cc)
add_dependencies(lc3 tests)

enable_testing()
add_test(NAME LC3Tests COMMAND tests)
