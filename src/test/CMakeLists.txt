find_package(Qt6Test REQUIRED)

enable_testing()

file(GLOB grepQtTestSrc "*.cpp" "*.hpp")
qt_add_executable("grepQtTests" ${grepQtTestSrc})
target_precompile_headers("grepQtTests" PRIVATE "PCH.hpp")
target_link_libraries("grepQtTests" PRIVATE Qt6::Test grepQtCore)
