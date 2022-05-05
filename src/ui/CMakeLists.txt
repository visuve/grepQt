find_package(Qt6 REQUIRED COMPONENTS Widgets)

if(MSVC)
	file(GLOB grepQtUiSrc "*.cpp" "*.hpp" "../../res/*.qrc" "../../res/*.rc")
else()
	file(GLOB grepQtUiSrc "*.cpp" "*.hpp" "../../res/*.qrc")
endif()

qt_add_executable("grepQt" ${grepQtUiSrc})
target_include_directories("grepQt" PRIVATE "../core")
target_precompile_headers("grepQt" PRIVATE "PCH.hpp")
target_link_libraries("grepQt" PRIVATE Qt6::Widgets grepQtCore)
