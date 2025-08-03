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
#include <vector>
#include <iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")


class ConexionMongo {
public:
    // Enumeración para los modos de conexión
    enum ModoConexion {
        SERVIDOR = 0,  // Conexión local (pcServidor)
        CLIENTE = 1,  // Conexión remota (pcCliente)
        INTERNET = 2   // Conexión a través de Internet (opcional)
    };

private:
    static ModoConexion modoActual;
    static std::string uriServidor;
    static std::string uriCliente;
    static std::string uriInternet;

public:
    /**
     * @brief Establece el modo de conexión (servidor, cliente o internet)
     * @param modo Modo de conexión a establecer
     */
    static void setModoConexion(ModoConexion modo) {
        modoActual = modo;
        std::string modoTexto;
        switch (modo) {
        case SERVIDOR:
            modoTexto = "SERVIDOR (Local)";
            break;
        case CLIENTE:
            modoTexto = "CLIENTE (Remoto)";
            break;
        case INTERNET:
            modoTexto = "INTERNET (MongoDB Atlas)";
            break;
        }
        std::cout << "Modo de conexión establecido: " << modoTexto << std::endl;
    }

    /**
     * @brief Obtiene el modo actual de conexión
     * @return Modo de conexión actual
     */
    static ModoConexion getModoConexion() {
        return modoActual;
    }

    /**
     * @brief Configura las URIs para servidor, cliente e internet
     * @param uriServ URI para el modo servidor
     * @param uriCli URI para el modo cliente
     * @param uriInt URI para el modo internet
     */
    static void configurarURIs(const std::string& uriServ = "mongodb://localhost:27017/?connectTimeoutMS=5000&serverSelectionTimeoutMS=3000",
        const std::string& uriCli = "",
        const std::string& uriInt = "mongodb+srv://uaandrade:uriel8334@sistemaaplicacioncuenta.qnzmjnz.mongodb.net/") {
        uriServidor = uriServ;
        uriInternet = uriInt;

        // Si no se proporciona URI de cliente, detectar automáticamente
        if (uriCli.empty()) {
            uriCliente = detectarURIClienteAutomatica();
        }
        else {
            uriCliente = uriCli;
        }
    }

    /**
     * @brief Obtiene el cliente MongoDB con mensajes de diagnóstico (para configuración inicial)
     * @return Referencia al cliente MongoDB
     */
    static mongocxx::client& getCliente() {
        static mongocxx::instance instance{};

        // Obtener la URI según el modo seleccionado
        std::string uri = obtenerURI();
        std::cout << "Inicializando cliente MongoDB con URI: " << uri << std::endl;

        std::string modoTexto;
        switch (modoActual) {
        case SERVIDOR:
            modoTexto = "SERVIDOR (Local)";
            break;
        case CLIENTE:
            modoTexto = "CLIENTE (Remoto)";
            break;
        case INTERNET:
            modoTexto = "INTERNET (MongoDB Atlas)";
            break;
        }
        std::cout << "Modo: " << modoTexto << std::endl;

        static mongocxx::client cliente{ mongocxx::uri{uri} };

        // Verificar que el cliente funcione con la base de datos real "Banco"
        if (!verificarCliente(cliente)) {
            std::cerr << "ADVERTENCIA: Cliente MongoDB creado pero no responde correctamente" << std::endl;
        }

        return cliente;
    }

    /**
     * @brief Obtiene el cliente MongoDB sin mensajes de diagnóstico (para operaciones normales)
     * @return Referencia al cliente MongoDB
     */
    static mongocxx::client& obtenerClienteBaseDatos() {
        static mongocxx::client cliente{ mongocxx::uri{obtenerURISilenciosa()} };
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

    /**
     * @brief Detecta automáticamente las IPs de red local para configurar URI de cliente
     * @return URI configurada automáticamente para red local
     */
    static std::string detectarURIClienteAutomatica();

    /**
     * @brief Obtiene la IP local del adaptador de red principal
     * @return IP local detectada
     */
    static std::string obtenerIPLocal();

    /**
     * @brief Escanea la red local para encontrar servidores MongoDB
     * @param redBase Red base para escanear (ej: "192.168.1")
     * @return Vector de IPs que tienen MongoDB disponible
     */
    static std::vector<std::string> escanearRedMongoDB(const std::string& redBase);

private:
    static std::string obtenerURI();

    /**
     * @brief Obtiene la URI sin imprimir mensajes de diagnóstico
     * @return URI de conexión apropiada sin logging
     */
    static std::string obtenerURISilenciosa();
};

#endif // CONEXIONMONGO_H