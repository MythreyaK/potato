cmake_minimum_required (VERSION 3.20)

add_subdirectory("src")

add_executable(app "main.cpp")

target_link_libraries(app PUBLIC testapp PRIVATE core)

configure_file("icon_big.bin" "${CMAKE_CURRENT_BINARY_DIR}/icon.bin" COPYONLY)

# if (MSVC)
#     set_target_properties(testapp PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup /SUBSYSTEM:WINDOWS")
# endif(MSVC)
