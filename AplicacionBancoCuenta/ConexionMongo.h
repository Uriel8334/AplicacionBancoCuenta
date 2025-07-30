// @file ConexionMongo.h

#pragma once
#ifndef CONEXIONMONGO_H
#define CONEXIONMONGO_H

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <windows.h>
#include <shlobj.h>
#include <filesystem>


class ConexionMongo {
public:
    static mongocxx::client& getCliente() {
        static mongocxx::instance instance{};
        static mongocxx::client cliente{ mongocxx::uri{obtenerURI()} };
        return cliente;
    }

private:
    static std::string obtenerURI() {
        // Obtener ruta del escritorio y carpeta BancoApp
        char pathEscritorio[MAX_PATH];
        std::string rutaConfig;
        if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_DESKTOPDIRECTORY, NULL, 0, pathEscritorio))) {
            std::string rutaBancoApp = std::string(pathEscritorio) + "\\BancoApp";
            std::filesystem::create_directories(rutaBancoApp);
            rutaConfig = rutaBancoApp + "\\config.json";
        }
        else {
            rutaConfig = "config.json"; // Fallback a la carpeta actual
        }

        // Si el archivo no existe, crearlo con la estructura por defecto
        std::ifstream archivoLectura(rutaConfig);
        if (!archivoLectura.is_open()) {
            std::ofstream archivoNuevo(rutaConfig);
            archivoNuevo << "{\n  \"mongo_uri\": \"mongodb://localhost:27017/\"\n}\n";
            archivoNuevo.close();
            return "mongodb://localhost:27017/";
        }
        archivoLectura.close();

        // Leer el archivo y extraer el valor de mongo_uri
        std::ifstream archivo(rutaConfig);
        if (!archivo.is_open()) {
            std::cerr << "No se pudo abrir el archivo de configuración.\n";
            return "mongodb://localhost:27017/";
        }

        std::string linea;
        while (std::getline(archivo, linea)) {
            linea.erase(std::remove_if(linea.begin(), linea.end(),
                [](char c) { return c == ' ' || c == '\"'; }), linea.end());
            size_t pos = linea.find("mongo_uri:");
            if (pos != std::string::npos) {
                std::string valor = linea.substr(pos + std::string("mongo_uri:").length());
                valor.erase(std::remove(valor.begin(), valor.end(), ','), valor.end());
                valor.erase(std::remove(valor.begin(), valor.end(), '}'), valor.end());
                return valor;
            }
        }
        return "mongodb://localhost:27017/";
    }
};

#endif // CONEXIONMONGO_H
