cmake_minimum_required(VERSION 3.16)
project(geometriaComputacional LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(PkgConfig REQUIRED)
pkg_check_modules(RAYLIB REQUIRED IMPORTED_TARGET raylib)
add_executable(geometriaComputacional main.cpp vetores.cpp planos.cpp visualizacao3D.cpp)
target_link_libraries(geometriaComputacional PRIVATE PkgConfig::RAYLIB)
add_executable(testesMatematicos testes.cpp vetores.cpp planos.cpp)
enable_testing()
add_test(NAME testesMatematicos COMMAND testesMatematicos)

# Modulo independente da Parte 4, reutilizando as operacoes vetoriais.
add_executable(simulacaoRobotica
    simulacaoRobotica/mainRobotica.cpp
    simulacaoRobotica/trajetoria.cpp
    vetores.cpp)
target_link_libraries(simulacaoRobotica PRIVATE PkgConfig::RAYLIB)

# Testes do modulo robotico sem dependencia da interface grafica.
add_executable(testesRobotica
    simulacaoRobotica/testesRobotica.cpp
    simulacaoRobotica/trajetoria.cpp
    vetores.cpp)
add_test(NAME testesRobotica COMMAND testesRobotica)
