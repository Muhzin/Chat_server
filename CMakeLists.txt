cmake_minimum_required(VERSION 3.16...3.23)
project(ChatMScIT LANGUAGES C)

find_package(Doxygen)

add_executable(client client.c chat.c)

add_executable(server server.c chat.c)

if(DOXYGEN_FOUND)
    message(STATUS "Doxygen found")
    doxygen_add_docs(chatdoc ${PROJECT_SOURCE_DIR})
endif()
