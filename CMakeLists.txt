cmake_minimum_required(VERSION 3.10)

# Nome do projeto
project(portfolio_Catolica)

# Definir o padrão C++
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Adicionar o primeiro programa (Main)
add_executable(Main
    Main/Main/Main.cpp
)

# Adicionar o segundo programa (NFC_Uno)
add_executable(NFC_Uno
    Main/Arduino Uno/NFC_Uno/NFC_Uno.cpp
)
