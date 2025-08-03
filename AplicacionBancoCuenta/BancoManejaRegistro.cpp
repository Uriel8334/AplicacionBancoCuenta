#define _CRT_SECURE_NO_WARNINGS

#include "BancoManejaRegistro.h"
#include "ConexionMongo.h"
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream> 

BancoManejaRegistro::BancoManejaRegistro() {
    // Constructor vacío
}

bool BancoManejaRegistro::agregarRegistroBaseDatos(const std::string& tipoOperacion, const std::string& cedula) {
    try {
        // 1. Guardar en archivo local (mantener funcionalidad existente)
        std::ofstream archivo(ARCHIVO_REGISTROS, std::ios::app);
        if (!archivo.is_open()) {
            std::cerr << "Error: No se pudo abrir el archivo de registros." << std::endl;
            return false;
        }

        std::string fechaHora = obtenerFechaHoraActual();

        // Formato: [FECHA_HORA] TIPO_OPERACION - CEDULA
        archivo << "[" << fechaHora << "] " << tipoOperacion << " - Cedula: " << cedula << std::endl;
        archivo.close();

        // 2. Guardar en MongoDB (nueva funcionalidad)
        mongocxx::client& client = ConexionMongo::obtenerClienteBaseDatos();
        auto db = client["Banco"];
        auto collection = db["registros"];

        // Crear documento para MongoDB
        auto registroDoc = bsoncxx::builder::basic::document{};
        registroDoc.append(
            bsoncxx::builder::basic::kvp("fechaHora", fechaHora),
            bsoncxx::builder::basic::kvp("tipoOperacion", tipoOperacion),
            bsoncxx::builder::basic::kvp("cedula", cedula),
            bsoncxx::builder::basic::kvp("timestamp", std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count())
        );

        auto result = collection.insert_one(registroDoc.view());

        if (!result) {
            std::cerr << "Error: No se pudo insertar el registro en MongoDB." << std::endl;
            return false;
        }

        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error al escribir registro: " << e.what() << std::endl;
        return false;
    }
}

std::string BancoManejaRegistro::obtenerFechaHoraActual() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%d/%m/%Y %H:%M:%S");
    return ss.str();
}