cmake_minimum_required(VERSION 3.23)

project(project_fern)

add_executable(${PROJECT_NAME} src/main.cpp)

# Adds All Internal Library Files
add_subdirectory("include/ProjectFern")

find_package(OpenSSL)
target_link_libraries(${PROJECT_NAME} OpenSSL::Crypto)

add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)