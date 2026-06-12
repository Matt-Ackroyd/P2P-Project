cmake_minimum_required(VERSION 3.23)

project(project_fern)

add_compile_options(-std=gnu++14)
#add_compile_options(-fsanitize=address)
#add_link_options(-fsanitize=address)

string(REGEX REPLACE "\\b-g\\b" "-ggdb3" tmp_value "${CMAKE_C_FLAGS_DEBUG}")
set_property(CACHE CMAKE_C_FLAGS_DEBUG PROPERTY VALUE "${tmp_value}")


add_executable(${PROJECT_NAME} src/main.cpp)

# Adds All Internal Library Files
add_subdirectory("include/ProjectFern")

if(WIN32)
    set_target_properties( ${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY 
        ${CMAKE_BINARY_DIR}/bin )
    set_target_properties( ${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY
        ${CMAKE_BINARY_DIR}/bin )
endif()

#find_package(OpenSSL)
#target_link_libraries(${PROJECT_NAME} OpenSSL::Crypto)
