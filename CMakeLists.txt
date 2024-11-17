cmake_minimum_required(VERSION 3.10)

# Definir o padrão C++
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Adicionar todos os diretórios dentro de libs
file(GLOB LIB_DIRS RELATIVE ${CMAKE_SOURCE_DIR} "${CMAKE_SOURCE_DIR}/libs/*")
foreach(dir ${LIB_DIRS})
    if(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/${dir}")
        include_directories("${CMAKE_SOURCE_DIR}/${dir}")
    endif()
endforeach()

# Adicionar o primeiro programa (Main)
add_executable(Main
    Main/Main/Main.cpp
)

# Adicionar o segundo programa (NFC_Uno)
add_executable(NFC_Uno
    Main/NFC_Uno/NFC_Uno.cpp
)
