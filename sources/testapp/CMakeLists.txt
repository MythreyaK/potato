cmake_minimum_required (VERSION 3.22)

add_subdirectory("src")

add_executable(app "main.cpp")

target_link_libraries(app PUBLIC testapp)

target_precompile_headers(app REUSE_FROM pch)

configure_file("icon.bin" "${CMAKE_CURRENT_BINARY_DIR}/icon.bin" COPYONLY)

# if (MSVC)
#     set_target_properties(testapp PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup /SUBSYSTEM:WINDOWS")
# endif(MSVC)
