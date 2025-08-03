#include "ConfiguradorSistema.h"
#include <iostream>
#include <windows.h>
#include <conio.h>

bool ConfiguradorSistema::inicializarSistema() {
    configurarConsolaUTF8();

    std::cout << "SISTEMA BANCARIO EDUCATIVO" << std::endl
        << "===================================\n" << std::endl;

    if (!configurarConexionMongoDB()) {
        return false;
    }

    verificarConexionBaseDatos();
    inicializarMarquesina();

    return true;
}

void ConfiguradorSistema::configurarConsolaUTF8() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
}

bool ConfiguradorSistema::configurarConexionMongoDB() {
    if (!seleccionarModoConexion()) {
        return false;
    }
    return true;
}

bool ConfiguradorSistema::seleccionarModoConexion() {
    std::vector<std::string> opcionesConexion = {
        "SERVIDOR (Local - localhost:27017)",
        "CLIENTE (Remoto - Red Local Automática)",
        "INTERNET (MongoDB Atlas - Nube)"
    };

    std::cout << "=== CONFIGURACIÓN DE CONEXIÓN MONGODB ===" << std::endl;
    std::cout << std::endl;
    std::cout << "Seleccione el modo de conexión:" << std::endl;
    std::cout << std::endl;
    std::cout << "• SERVIDOR: Para ejecutar en la máquina que tiene MongoDB instalado localmente" << std::endl;
    std::cout << "• CLIENTE: Para ejecutar en máquinas remotas conectadas al servidor (detección automática)" << std::endl;
    std::cout << "• INTERNET: Para conectar a MongoDB Atlas en la nube (acceso desde cualquier lugar)" << std::endl;
    std::cout << std::endl;

    int seleccionConexion = Utilidades::menuInteractivo("Modo de Conexión MongoDB", opcionesConexion, 0, 0);

    system("cls");
    system("pause");

    if (seleccionConexion == -1) {
        std::cout << "Configuración cancelada. Saliendo del sistema..." << std::endl;
        return false;
    }

    mostrarInformacionModo(seleccionConexion);
    system("pause");
    system("cls");

    return true;
}

void ConfiguradorSistema::mostrarInformacionModo(int seleccion) {
    std::cout << std::endl;

    switch (seleccion) {
    case 0:
        ConexionMongo::setModoConexion(ConexionMongo::SERVIDOR);
        std::cout << "=== MODO SERVIDOR SELECCIONADO ===" << std::endl;
        std::cout << "• Conectará a: mongodb://localhost:27017" << std::endl;
        std::cout << "• Asegúrese de que MongoDB esté ejecutándose localmente" << std::endl;
        break;

    case 1:
        ConexionMongo::setModoConexion(ConexionMongo::CLIENTE);
        std::cout << "=== MODO CLIENTE SELECCIONADO ===" << std::endl;
        std::cout << "• Detectando configuración de red automáticamente..." << std::endl;
        std::cout << "• Se escaneará la red local para encontrar servidores MongoDB" << std::endl;
        std::cout << "• Asegúrese de estar conectado a la misma red que el servidor" << std::endl;
        break;

    case 2:
        ConexionMongo::setModoConexion(ConexionMongo::INTERNET);
        std::cout << "=== MODO INTERNET SELECCIONADO ===" << std::endl;
        std::cout << "• Conectará a: MongoDB Atlas (sistemaaplicacioncuenta.qnzmjnz.mongodb.net)" << std::endl;
        std::cout << "• Base de datos compartida en la nube - ideal para desarrollo colaborativo" << std::endl;
        std::cout << "• Asegúrese de tener conexión a Internet estable" << std::endl;
        std::cout << "• Perfecto para trabajo remoto y control de versiones con Git" << std::endl;
        break;
    }
}

void ConfiguradorSistema::verificarConexionBaseDatos() {
    try {
        ConexionMongo::getCliente();
        std::cout << "Conexión MongoDB establecida correctamente\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Error al conectar con MongoDB: " << e.what() << "\n";
        system("pause");
        exit(-1);
    }
    system("pause");
}

void ConfiguradorSistema::inicializarMarquesina() {
    Utilidades::limpiarPantallaPreservandoMarquesina(0);

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    int anchoConsola = csbi.srWindow.Right - csbi.srWindow.Left + 1;

    marquesina = std::make_unique<Marquesina>(0, 0, anchoConsola, "marquesina.html", 200);
    marquesina->iniciar();

    Utilidades::centrarVentanaConsola();
    std::cout << std::endl << std::endl;
    Utilidades::ocultarCursor();
}

Marquesina* ConfiguradorSistema::getMarquesina() const {
    return marquesina.get();
}

void ConfiguradorSistema::finalizarSistema() {
    if (marquesina) {
        marquesina.reset();
    }
}