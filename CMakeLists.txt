cmake_minimum_required(VERSION 3.16)

project("grepQt" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(MSVC)
	add_compile_options(/W4 /WX /MP /wd26812)
else()
	add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()


add_subdirectory("src/core")
add_subdirectory("src/ui")
add_subdirectory("src/test")
