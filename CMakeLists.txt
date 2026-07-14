cmake_minimum_required(VERSION 3.23)

project(project_fern VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup(REQUIRES 6.10)

qt_add_executable(app_${PROJECT_NAME}
    main.cpp
    ${PROJECT_SOURCE_DIR}/src/DataTypes/Files/FileContainer.cpp
    ${PROJECT_SOURCE_DIR}/src/DataTypes/MessageContainer.cpp
    ${PROJECT_SOURCE_DIR}/src/DataTypes/Files/FileIndicator.cpp
    ${PROJECT_SOURCE_DIR}/src/DataTypes/ID.cpp
    ${PROJECT_SOURCE_DIR}/src/Networking/Encryption.cpp
    ${PROJECT_SOURCE_DIR}/src/Networking/Packet.cpp
    ${PROJECT_SOURCE_DIR}/src/Networking/UDPConnection.cpp
    ${PROJECT_SOURCE_DIR}/src/Networking/IncomingHandler.cpp
    ${PROJECT_SOURCE_DIR}/src/Server/PrimaryClient.cpp
    ${PROJECT_SOURCE_DIR}/src/Server/Server.cpp
    ${PROJECT_SOURCE_DIR}/src/Server/RemoteUser.cpp
    ${PROJECT_SOURCE_DIR}/src/Server/TextChannel.cpp
    ${PROJECT_SOURCE_DIR}/src/Relay/RelayClient.cpp
    ${PROJECT_SOURCE_DIR}/src/Networking/ML-KEM_Handshake.cpp
    src/ConfigLoader.cpp
    src/Networking/OutgoingHandler.cpp
    src/DatabaseConnection.cpp
    src/DataTypes/ServerContainer.cpp
    src/DataTypes/TextChannelContainer.cpp
    src/DataTypes/JoinRequest.cpp
    src/DataTypes/UserContainer.cpp
    src/DataTypes/SyncRequest.cpp
    src/DataTypes/SyncResponse.cpp
    src/DataTypes/Request.cpp
    src/DataTypes/Files/FileHostClaim.cpp
)
target_include_directories(app_${PROJECT_NAME}
    PUBLIC include/ProjectFern/DataTypes/
    PUBLIC include/ProjectFern/Networking/
    PUBLIC include/ProjectFern/Server/
    PUBLIC include/ProjectFern/GUI/
    PUBLIC include/ProjectFern/Relay/
    PUBLIC include/ProjectFern/
)

#add_subdirectory("include/ProjectFern")

qt_add_qml_module(app_${PROJECT_NAME}
    URI ${PROJECT_NAME}
    QML_FILES
        Main.qml
        ChannelStructure.qml
        ServerStructure.qml
    SOURCES
        ${PROJECT_SOURCE_DIR}/include/ProjectFern/GUI/CppInterface.h
        ${PROJECT_SOURCE_DIR}/src/GUI/CppInterface.cpp
)


# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(app_${PROJECT_NAME} PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.app_${PROJECT_NAME}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

find_package(OpenSSL REQUIRED)
find_package(SQLite3 REQUIRED)
target_link_libraries(app_${PROJECT_NAME}
    PRIVATE Qt6::Quick
    PRIVATE Qt6::Widgets
    #FernLib
    PRIVATE OpenSSL::Crypto
    PRIVATE SQLite3::SQLite3
)

if(WIN32)
    target_link_libraries(app_${PROJECT_NAME} PUBLIC wsock32 ws2_32)
    # set_target_properties( app_${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY
    #     ${CMAKE_BINARY_DIR}/bin )
    # set_target_properties( app_${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY
    #     ${CMAKE_BINARY_DIR}/bin )
endif()

install(TARGETS app_${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)


add_subdirectory("src/Relay")