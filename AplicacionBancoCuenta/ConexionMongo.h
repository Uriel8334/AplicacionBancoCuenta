// @file ConexionMongo.h

#pragma once
#ifndef CONEXIONMONGO_H
#define CONEXIONMONGO_H

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/exception/operation_exception.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <windows.h>
#include <shlobj.h>
#include <filesystem>
#include <chrono>
#include <thread>

class ConexionMongo {
public:
    // Enumeración para los modos de conexión
    enum ModoConexion {
        SERVIDOR = 0,  // Conexión local (pcServidor)
        CLIENTE = 1    // Conexión remota (pcCliente)
    };

private:
    static ModoConexion modoActual;
    static std::string uriServidor;
    static std::string uriCliente;

public:
    /**
     * @brief Establece el modo de conexión (servidor o cliente)
     * @param modo Modo de conexión a establecer
     */
    static void setModoConexion(ModoConexion modo) {
        modoActual = modo;
        std::cout << "Modo de conexión establecido: " << (modo == SERVIDOR ? "SERVIDOR (Local)" : "CLIENTE (Remoto)") << std::endl;
    }

    /**
     * @brief Obtiene el modo actual de conexión
     * @return Modo de conexión actual
     */
    static ModoConexion getModoConexion() {
        return modoActual;
    }

    /**
     * @brief Configura las URIs para servidor y cliente
     * @param uriServ URI para el modo servidor
     * @param uriCli URI para el modo cliente
     */
    static void configurarURIs(const std::string& uriServ = "mongodb://localhost:27017/?connectTimeoutMS=5000&serverSelectionTimeoutMS=3000",
        const std::string& uriCli = "mongodb://192.168.1.10:27017/?connectTimeoutMS=5000&serverSelectionTimeoutMS=3000") {
        uriServidor = uriServ;
        uriCliente = uriCli;
    }

    static mongocxx::client& getCliente() {
        static mongocxx::instance instance{};

        // Obtener la URI según el modo seleccionado
        std::string uri = obtenerURI();
        std::cout << "Inicializando cliente MongoDB con URI: " << uri << std::endl;
        std::cout << "Modo: " << (modoActual == SERVIDOR ? "SERVIDOR (Local)" : "CLIENTE (Remoto)") << std::endl;

        static mongocxx::client cliente{ mongocxx::uri{uri} };

        // Verificar que el cliente funcione con la base de datos real "Banco"
        if (!verificarCliente(cliente)) {
            std::cerr << "ADVERTENCIA: Cliente MongoDB creado pero no responde correctamente" << std::endl;
        }

        return cliente;
    }

    /**
     * @brief Verifica que el cliente MongoDB funcione correctamente con la base de datos "Banco"
     */
    static bool verificarCliente(mongocxx::client& cliente);

    /**
     * @brief Prueba la conexión a MongoDB con timeout y mejor diagnóstico
     * @param uri URI de conexión a probar
     * @param timeoutSegundos Timeout en segundos (por defecto 10)
     * @return true si la conexión es exitosa, false en caso contrario
     */
    static bool probarConexion(const std::string& uri, int timeoutSegundos = 10);

    /**
     * @brief Ejecuta un diagnóstico completo de la conexión
     */
    static void ejecutarDiagnosticoCompleto();

private:
    static std::string obtenerURI();
};

#endif // CONEXIONMONGO_H