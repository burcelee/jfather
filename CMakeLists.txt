cmake_minimum_required(VERSION 3.26.4)

project (jfather)

file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "include/*.h")
add_executable (${CMAKE_PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE  ${PROJECT_SOURCE_DIR}/include)

target_compile_features(${CMAKE_PROJECT_NAME} PUBLIC cxx_std_17)

file(COPY test.json DESTINATION ${CMAKE_CURRENT_BINARY_DIR})