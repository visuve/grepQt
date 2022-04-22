set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core5Compat)

if(MSVC)
	file(GLOB grepQtSrc "*.cpp" "*.hpp" "*.ui" "../res/*.qrc" "../res/*.rc")
else()
	file(GLOB grepQtSrc "*.cpp" "*.hpp" "*.ui" "../res/*.qrc")
endif()

qt_add_executable("grepQt" ${grepQtSrc})
target_precompile_headers("grepQt" PRIVATE "PCH.hpp")
target_include_directories("grepQt" PRIVATE "${uchardet_SOURCE_DIR}/src")
target_link_directories("grepQt" PRIVATE "${uchardet_BINARY_DIR}/src")
target_link_libraries("grepQt" PRIVATE Qt6::Widgets Qt6::Core5Compat uchardet)
