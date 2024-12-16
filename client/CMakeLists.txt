cmake_minimum_required(VERSION 3.16)
project(client LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Quick Protobuf ProtobufTools Grpc GrpcTools Network)

qt_standard_project_setup()

qt_add_executable(client
    main.cpp
    grpcclient.h
    grpcclient.cpp
    discoveryclient.h
)

qt_add_protobuf(client
    PROTO_FILES
        ../proto/api.proto
)

qt_add_grpc(client CLIENT
    PROTO_FILES
        ../proto/api.proto
)

qt_add_qml_module(client
    URI grpctest.client
    VERSION 1.0
    RESOURCE_PREFIX "/qt/qml"
    QML_FILES
        "Main.qml"
)
target_link_libraries(client
    PRIVATE
        Qt::Core
        Qt::Quick
        Qt::Grpc
        Qt::Network
)

set_target_properties(client PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)