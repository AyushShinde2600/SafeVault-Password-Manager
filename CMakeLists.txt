cmake_minimum_required(VERSION 3.16)

project(SafeVault VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql Core)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        passwordentry.h
        passwordentry.cpp
        encryptionmanager.h
        encryptionmanager.cpp
        databasemanager.h
        databasemanager.cpp
)

add_executable(SafeVault ${PROJECT_SOURCES}
    passwordentry.h passwordentry.cpp
    encryptionmanager.h encryptionmanager.cpp
    databasemanager.h databasemanager.cpp
    passwordgenerator.h
    passwordgenerator.cpp)

target_link_libraries(SafeVault PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Core
)