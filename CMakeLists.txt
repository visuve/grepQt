cmake_minimum_required(VERSION 3.16)

project(grepQt LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

file(GLOB grepQtSrc "*.cpp" "*.ui" "*.qrc")
qt_add_executable("grepQt" ${grepQtSrc})
target_precompile_headers("grepQt" PRIVATE "PCH.hpp")
target_link_libraries("grepQt" PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
