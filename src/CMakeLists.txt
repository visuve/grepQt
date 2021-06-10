set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 COMPONENTS Widgets REQUIRED)
find_package(Qt6 COMPONENTS Widgets REQUIRED)

if(MSVC)
	file(GLOB grepQtSrc "*.cpp" "*.hpp" "*.ui" "../res/*.qrc" "../res/*.rc")
else()
	file(GLOB grepQtSrc "*.cpp" "*.hpp" "*.ui" "../res/*.qrc")
endif()

qt_add_executable("grepQt" ${grepQtSrc})
target_precompile_headers("grepQt" PRIVATE "PCH.hpp")
target_link_libraries("grepQt" PRIVATE Qt6::Widgets)
