cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB LIB_DIRS RELATIVE ${CMAKE_SOURCE_DIR} "${CMAKE_SOURCE_DIR}/libs/*")
foreach(dir ${LIB_DIRS})
    if(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/${dir}")
        include_directories("${CMAKE_SOURCE_DIR}/${dir}")
    endif()
endforeach()

add_executable(Main
    Main/Main/Main.cpp
)

add_executable(NFC_Uno
    Main/NFC_Uno/NFC_Uno.cpp
)
