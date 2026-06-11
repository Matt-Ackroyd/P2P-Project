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



target_link_libraries(${PROJECT_NAME} FernLib)