cmake_minimum_required(VERSION 4.0)

project(SparkLang LANGUAGES C)

add_subdirectory("src/")

# Output Directories
set_target_properties(SparkVM
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
