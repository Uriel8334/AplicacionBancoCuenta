cmake_minimum_required(VERSION 3.15)
project(AplicacionBancoCuenta LANGUAGES CXX)

# Usa C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SRC_FILES
       "AplicacionBancoCuenta/*.cpp"
   )
   file(GLOB HEADER_FILES
       "AplicacionBancoCuenta/*.h"
   )
   add_executable(AplicacionBancoCuenta ${SRC_FILES} ${HEADER_FILES})

add_executable(AplicacionBancoCuenta ${SRC_FILES} ${HEADER_FILES})



# Busca los paquetes de MongoDB C++ y sus dependencias (vcpkg)
find_package(bsoncxx CONFIG REQUIRED)
find_package(mongocxx CONFIG REQUIRED)

target_link_libraries(AplicacionBancoCuenta PRIVATE
    $<IF:$<TARGET_EXISTS:mongo::bsoncxx_static>,mongo::bsoncxx_static,mongo::bsoncxx_shared>
    $<IF:$<TARGET_EXISTS:mongo::mongocxx_static>,mongo::mongocxx_static,mongo::mongocxx_shared>
)

# Si tienes subcarpetas para fuentes o headers, añade sus rutas aquí:
# target_include_directories(AplicacionBancoCuenta PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Si usas Windows y necesitas la API de Windows explícitamente:
if (WIN32)
    target_link_libraries(AplicacionBancoCuenta PRIVATE ws2_32 Secur32 Crypt32)
endif()