cmake_minimum_required(VERSION 3.31)

project(webserver)
set(CMAKE_CXX_STANDARD 23)
file(GLOB_RECURSE SOURCE_FILES CMAKE_CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    )
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC ROOT_FLDR="${CMAKE_CURRENT_SOURCE_DIR}/")
add_compile_options(-Wall -Wextra -Wpedantic -Werror)