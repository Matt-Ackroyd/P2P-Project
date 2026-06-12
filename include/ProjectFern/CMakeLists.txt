add_library(
    FernLib

    SHARED

    # Files
    ${PROJECT_SOURCE_DIR}/src/DataTypes/FileContainer.cpp
    ${PROJECT_SOURCE_DIR}/src/DataTypes/MessageContainer.cpp
    ${PROJECT_SOURCE_DIR}/src/DataTypes/FileIndicator.cpp
    ${PROJECT_SOURCE_DIR}/src/DataTypes/ID.cpp
    ${PROJECT_SOURCE_DIR}/src/Networking/Encryption.cpp
    ${PROJECT_SOURCE_DIR}/src/Networking/Packet.cpp
    ${PROJECT_SOURCE_DIR}/src/Networking/UDPConnection.cpp
    ${PROJECT_SOURCE_DIR}/src/Networking/IncomingHandler.cpp
    ${PROJECT_SOURCE_DIR}/src/Server/PrimaryClient.cpp
    ${PROJECT_SOURCE_DIR}/src/Server/Server.cpp
    ${PROJECT_SOURCE_DIR}/src/Server/RemoteUser.cpp
)

target_include_directories(FernLib PUBLIC DataTypes/)
target_include_directories(FernLib PUBLIC Networking/)
target_include_directories(FernLib PUBLIC Server/)

set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL)
target_link_libraries(FernLib OpenSSL::Crypto)

if(WIN32)
  target_link_libraries(FernLib wsock32 ws2_32)
  set_target_properties( FernLib PROPERTIES RUNTIME_OUTPUT_DIRECTORY 
      ${CMAKE_BINARY_DIR}/bin )
  set_target_properties( FernLib PROPERTIES LIBRARY_OUTPUT_DIRECTORY
      ${CMAKE_BINARY_DIR}/bin )
endif()

target_link_libraries(${PROJECT_NAME} FernLib)