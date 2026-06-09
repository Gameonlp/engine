# Initialize header file
file(WRITE "${OUTPUT_HEADER}" "// Generated file. Do not edit.\n#pragma once\n#include <vector>\n#include <cstdint>\n\nnamespace Engine::Shaders {\n")

# Convert the space-separated string from the command line into a valid CMake semicolon list
string(REPLACE " " ";" INPUT_FILES "${INPUT_FILES}")

foreach(FILE_PATH ${INPUT_FILES})
    # Clean off any stray literal quotes just in case your shell cached them
    string(REPLACE "\"" "" FILE_PATH "${FILE_PATH}")

    if(NOT EXISTS "${FILE_PATH}")
        message(FATAL_ERROR "Could not find file to embed: '${FILE_PATH}'")
    endif()

    get_filename_component(FILE_NAME ${FILE_PATH} NAME)
    string(MAKE_C_IDENTIFIER "${FILE_NAME}" VAR_NAME)

    file(READ "${FILE_PATH}" HEX_DATA HEX)
    string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1, " C_ARRAY "${HEX_DATA}")

    file(APPEND "${OUTPUT_HEADER}" "inline const std::vector<uint8_t> ${VAR_NAME} = {\n    ${C_ARRAY}\n};\n\n")
endforeach()

file(APPEND "${OUTPUT_HEADER}" "}\n")