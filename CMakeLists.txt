cmake_minimum_required(VERSION 3.10)

# Definir o padrão C++
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Adicionar o diretório base de include
include_directories("${CMAKE_SOURCE_DIR}/libs")

# Adicionar o primeiro programa (Main)
add_executable(Main
    Main/Main/Main.cpp
)

# Especificar os diretórios de includes para o programa Main
target_include_directories(Main PRIVATE
    "${CMAKE_SOURCE_DIR}/libs/NDEF"
)

# Adicionar o segundo programa (NFC_Uno)
add_executable(NFC_Uno
    Main/NFC_Uno/NFC_Uno.cpp
)

# Especificar os diretórios de includes para o programa NFC_Uno
target_include_directories(NFC_Uno PRIVATE
    "${CMAKE_SOURCE_DIR}/libs/NDEF"
)
