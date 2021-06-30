# Compile shaders

cmake_minimum_required (VERSION 3.20)

file(GLOB_RECURSE SHADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS *.vert *.frag)

# partially inspired by https://github.com/ARM-software/vulkan-sdk/blob/a46f6762540c03040a7b3e861468d54f9ac1a4ea/Sample.cmake#L26
function (add_shader FILENAME WORKING_DIRECTORY OUTPUT_DIRECTORY OUTPUT_FILE)
    find_program(GLSLC glslc)
    add_custom_command(
        OUTPUT              "${OUTPUT_DIRECTORY}/${FILENAME}.spv"
        COMMAND ${GLSLC}    "${FILENAME}" -o "${OUTPUT_DIRECTORY}/${FILENAME}.spv"
        DEPENDS             "${FILENAME}"
        WORKING_DIRECTORY   "${WORKING_DIRECTORY}"
        IMPLICIT_DEPENDS CXX "${FILENAME}"
        COMMENT "${GLSLC}: ${FILENAME}"
        VERBATIM
    )
    set(${OUTPUT_FILE} "${OUTPUT_DIRECTORY}/${FILENAME}.spv" PARENT_SCOPE)
endfunction()

function (build_shaders SHADERS COMPILED_SHADERS WORKING_DIRECTORY OUTPUT_DIRECTORY)
    foreach(SHADER ${SHADERS})
        add_shader("${SHADER}" "${WORKING_DIRECTORY}" "${OUTPUT_DIRECTORY}" OUT_SHADER)
        set(COMPILED_SHADERS ${COMPILED_SHADERS} ${OUT_SHADER})
        set(COMPILED_SHADERS ${COMPILED_SHADERS} PARENT_SCOPE)
    endforeach()

endfunction()

build_shaders("${SHADERS}" "${COMPILED_SHADERS}" "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/../testapp")

add_custom_target(shaders ALL DEPENDS ${COMPILED_SHADERS})
