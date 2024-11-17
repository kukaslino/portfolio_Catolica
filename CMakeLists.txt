cmake_minimum_required(VERSION 3.10)

# Nome do projeto
project(portfolio_Catolica)

# Definir o padrão C++
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Incluir todos os subdiretórios dentro de libs
file(GLOB_RECURSE LIB_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/libs/*")
foreach(dir ${LIB_INCLUDE_DIRS})
    if(IS_DIRECTORY ${dir})
        include_directories(${dir})
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
