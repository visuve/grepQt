find_package(Qt6 REQUIRED COMPONENTS Core)

file(GLOB grepQtCoreSrc "*.cpp" "*.hpp")
qt_add_library("grepQtCore" STATIC ${grepQtCoreSrc})
target_precompile_headers("grepQtCore" PRIVATE "PCH.hpp")
target_link_libraries("grepQtCore" PRIVATE Qt6::Core icu)
