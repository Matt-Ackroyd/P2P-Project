

#Relay Server Executable THIS NEEDS TO BE CLEANED UP
add_executable(${PROJECT_NAME}_relay_server 
    RelayServer.cpp
    ${PROJECT_SOURCE_DIR}/src/DataTypes/ID.cpp
    ${PROJECT_SOURCE_DIR}/src/Networking/Encryption.cpp
    ${PROJECT_SOURCE_DIR}/src/Networking/ML-KEM_Handshake.cpp
    ${PROJECT_SOURCE_DIR}/src/Networking/Packet.cpp
    ${PROJECT_SOURCE_DIR}/src/ConfigLoader.cpp
)

target_include_directories(${PROJECT_NAME}_relay_server  
    PUBLIC ${PROJECT_SOURCE_DIR}/include/ProjectFern/DataTypes/
    PUBLIC ${PROJECT_SOURCE_DIR}/include/ProjectFern/Networking/
    PUBLIC ${PROJECT_SOURCE_DIR}/include/ProjectFern/Server/
    PUBLIC ${PROJECT_SOURCE_DIR}/include/ProjectFern/GUI/
    PUBLIC ${PROJECT_SOURCE_DIR}/include/ProjectFern/Relay/
    PUBLIC ${PROJECT_SOURCE_DIR}/include/ProjectFern/
)
find_package(OpenSSL)
target_link_libraries(${PROJECT_NAME}_relay_server 
    PUBLIC OpenSSL::Crypto
)
if(WIN32)
    target_link_libraries(${PROJECT_NAME}_relay_server PUBLIC wsock32 ws2_32)
    set_target_properties(${PROJECT_NAME}_relay_server PROPERTIES RUNTIME_OUTPUT_DIRECTORY 
        ${CMAKE_BINARY_DIR}/bin )
    set_target_properties(${PROJECT_NAME}_relay_server PROPERTIES LIBRARY_OUTPUT_DIRECTORY
        ${CMAKE_BINARY_DIR}/bin )
endif()