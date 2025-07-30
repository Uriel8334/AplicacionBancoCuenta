#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX

#include "AdministradorChatRedLocal.h"
#include "Utilidades.h"
#include <iostream>
#include <conio.h>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <fstream>
#include <filesystem>
#include <windows.h>

AdministradorChatRedLocal::AdministradorChatRedLocal()
    : chatActivo(false), servidorEnEjecucion(false), ultimoMensajeLeido(0) {

    rutaCompleta = DIRECTORIO_CHAT;

    // Configurar archivos según el modo
    ConexionMongo::ModoConexion modo = ConexionMongo::getModoConexion();
    if (modo == ConexionMongo::SERVIDOR) {
        archivoMensajesLocal = ARCHIVO_SERVIDOR;
        archivoMensajesRemoto = ARCHIVO_CLIENTE;
    }
    else {
        archivoMensajesLocal = ARCHIVO_CLIENTE;
        archivoMensajesRemoto = ARCHIVO_SERVIDOR;
    }
}

AdministradorChatRedLocal::~AdministradorChatRedLocal() {
    detenerChat();
}

bool AdministradorChatRedLocal::crearDirectorioChat() {
    try {
        if (!std::filesystem::exists(rutaCompleta)) {
            std::filesystem::create_directories(rutaCompleta);
        }
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error creando directorio de chat: " << e.what() << std::endl;
        return false;
    }
}

void AdministradorChatRedLocal::configurarSegunModoMongoDB() {
    ConexionMongo::ModoConexion modo = ConexionMongo::getModoConexion();

    std::cout << "\n=== CONFIGURACIÓN DEL CHAT LOCAL ===" << std::endl;
    if (modo == ConexionMongo::SERVIDOR) {
        std::cout << "• Modo: SERVIDOR (Esperará mensajes de clientes)" << std::endl;
        std::cout << "• Archivo local: " << archivoMensajesLocal << std::endl;
        std::cout << "• Archivo remoto: " << archivoMensajesRemoto << std::endl;
    }
    else {
        std::cout << "• Modo: CLIENTE (Enviará mensajes al servidor)" << std::endl;
        std::cout << "• Archivo local: " << archivoMensajesLocal << std::endl;
        std::cout << "• Archivo remoto: " << archivoMensajesRemoto << std::endl;
    }
    std::cout << "• Directorio: " << rutaCompleta << std::endl;
    std::cout << std::endl;
}

bool AdministradorChatRedLocal::escribirMensaje(const std::string& mensaje) {
    try {
        std::ofstream archivo(rutaCompleta + archivoMensajesLocal, std::ios::app);
        if (archivo.is_open()) {
            archivo << obtenerTimestamp() << " " << mensaje << std::endl;
            archivo.close();
            return true;
        }
        return false;
    }
    catch (const std::exception& e) {
        std::cerr << "Error escribiendo mensaje: " << e.what() << std::endl;
        return false;
    }
}

std::vector<std::string> AdministradorChatRedLocal::leerMensajesNuevos() {
    std::vector<std::string> mensajesNuevos;
    try {
        std::ifstream archivo(rutaCompleta + archivoMensajesRemoto);
        if (archivo.is_open()) {
            std::string linea;
            size_t contador = 0;
            while (std::getline(archivo, linea)) {
                if (contador >= ultimoMensajeLeido) {
                    mensajesNuevos.push_back(linea);
                }
                contador++;
            }
            ultimoMensajeLeido = contador;
            archivo.close();
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error leyendo mensajes: " << e.what() << std::endl;
    }
    return mensajesNuevos;
}

bool AdministradorChatRedLocal::marcarConexion(bool conectado) {
    try {
        // Crear archivos de estado específicos para cada modo
        std::string archivoEstadoLocal;
        if (ConexionMongo::getModoConexion() == ConexionMongo::SERVIDOR) {
            archivoEstadoLocal = "estado_servidor.txt";
        }
        else {
            archivoEstadoLocal = "estado_cliente.txt";
        }

        std::ofstream archivo(rutaCompleta + archivoEstadoLocal);
        if (archivo.is_open()) {
            archivo << (conectado ? "CONECTADO" : "DESCONECTADO") << std::endl;
            archivo << obtenerModoTexto() << std::endl;
            archivo << obtenerTimestamp() << std::endl;
            archivo.close();
            return true;
        }
        return false;
    }
    catch (const std::exception& e) {
        std::cerr << "Error marcando conexión: " << e.what() << std::endl;
        return false;
    }
}

bool AdministradorChatRedLocal::verificarConexionRemota() {
    try {
        // Verificar el archivo de estado del lado remoto
        std::string archivoEstadoRemoto;
        if (ConexionMongo::getModoConexion() == ConexionMongo::SERVIDOR) {
            archivoEstadoRemoto = "estado_cliente.txt";
        }
        else {
            archivoEstadoRemoto = "estado_servidor.txt";
        }

        std::ifstream archivo(rutaCompleta + archivoEstadoRemoto);
        if (archivo.is_open()) {
            std::string estado, modo, timestamp;
            std::getline(archivo, estado);
            std::getline(archivo, modo);
            std::getline(archivo, timestamp);
            archivo.close();

            // Verificar que el estado sea CONECTADO y que no sea muy antiguo
            if (estado == "CONECTADO") {
                // Verificar también que el archivo de mensajes remoto exista
                std::ifstream archivoMensajes(rutaCompleta + archivoMensajesRemoto);
                bool existeArchivoMensajes = archivoMensajes.is_open();
                if (existeArchivoMensajes) {
                    archivoMensajes.close();
                }
                return existeArchivoMensajes;
            }
        }
        return false;
    }
    catch (const std::exception& e) {
        return false;
    }
}

void AdministradorChatRedLocal::limpiarArchivos() {
    try {
        // Limpiar archivo de mensajes local al iniciar
        std::ofstream archivoLocal(rutaCompleta + archivoMensajesLocal, std::ios::trunc);
        archivoLocal.close();

        // Marcar como desconectado al salir
        marcarConexion(false);
    }
    catch (const std::exception& e) {
        std::cerr << "Error limpiando archivos: " << e.what() << std::endl;
    }
}

void AdministradorChatRedLocal::ejecutarMonitoreo() {
    while (chatActivo) {
        // AMBOS (servidor y cliente) marcan su conexión regularmente
        marcarConexion(true);

        if (ConexionMongo::getModoConexion() == ConexionMongo::SERVIDOR) {
            servidorEnEjecucion = true;
        }

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

void AdministradorChatRedLocal::escucharMensajes() {
    while (chatActivo) {
        std::vector<std::string> mensajesNuevos = leerMensajesNuevos();

        for (const auto& mensaje : mensajesNuevos) {
            std::lock_guard<std::mutex> lock(mtxMensajes);
            historialMensajes.push_back(mensaje);
            std::cout << "\r" << mensaje << std::endl;
            std::cout << nombreUsuario << ": " << std::flush;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void AdministradorChatRedLocal::mostrarMensaje(const std::string& mensaje) {
    std::cout << mensaje << std::endl;
}

void AdministradorChatRedLocal::limpiarPantallaChat() {
    Utilidades::limpiarPantallaPreservandoMarquesina(1);
    std::cout << "=== CHAT LOCAL - " << obtenerModoTexto() << " ===" << std::endl;
    std::cout << "Escriba sus mensajes. Escriba '/exit' para salir." << std::endl;
    std::cout << "Chat por archivos temporales - Directorio: " << rutaCompleta << std::endl;
    std::cout << std::string(60, '-') << std::endl;

    // Mostrar historial reciente
    std::lock_guard<std::mutex> lock(mtxMensajes);
    size_t inicio = historialMensajes.size() > 8 ? historialMensajes.size() - 8 : 0;
    for (size_t i = inicio; i < historialMensajes.size(); ++i) {
        std::cout << historialMensajes[i] << std::endl;
    }
}

std::string AdministradorChatRedLocal::obtenerModoTexto() {
    return (ConexionMongo::getModoConexion() == ConexionMongo::SERVIDOR) ? "SERVIDOR" : "CLIENTE";
}

std::string AdministradorChatRedLocal::obtenerTimestamp() {
    auto ahora = std::chrono::system_clock::now();
    auto tiempo = std::chrono::system_clock::to_time_t(ahora);

    struct tm tiempoLocal;
    localtime_s(&tiempoLocal, &tiempo);

    std::stringstream ss;
    ss << "[" << std::put_time(&tiempoLocal, "%H:%M:%S") << "]";
    return ss.str();
}

bool AdministradorChatRedLocal::hayClientesConectados() const {
    return const_cast<AdministradorChatRedLocal*>(this)->verificarConexionRemota();
}

void AdministradorChatRedLocal::iniciarChat() {
    configurarSegunModoMongoDB();

    // Crear directorio de chat
    if (!crearDirectorioChat()) {
        std::cout << "Error: No se pudo crear el directorio de chat." << std::endl;
        system("pause");
        return;
    }

    // Solicitar nombre de usuario
    std::cout << "Ingrese su nombre de usuario: ";
    std::getline(std::cin, nombreUsuario);
    if (nombreUsuario.empty()) {
        nombreUsuario = "Usuario" + std::to_string(std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count() % 1000);
    }

    chatActivo = true;

    // Limpiar archivos anteriores solo del modo actual
    try {
        std::ofstream archivoLocal(rutaCompleta + archivoMensajesLocal, std::ios::trunc);
        archivoLocal.close();
    }
    catch (const std::exception& e) {
        std::cerr << "Error limpiando archivo local: " << e.what() << std::endl;
    }

    // Iniciar hilos
    hiloMonitoreo = std::thread(&AdministradorChatRedLocal::ejecutarMonitoreo, this);
    hiloEscucha = std::thread(&AdministradorChatRedLocal::escucharMensajes, this);

    // Dar tiempo para que se establezca la conexión
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    limpiarPantallaChat();

    std::cout << "Chat iniciado correctamente." << std::endl;
    std::cout << "Estado de conexión: " << (verificarConexionRemota() ? "CONECTADO" : "ESPERANDO CONEXIÓN") << std::endl;
    std::cout << std::endl;

    // Bucle principal del chat
    std::string mensaje;
    while (chatActivo) {
        std::cout << nombreUsuario << ": ";
        std::getline(std::cin, mensaje);

        if (mensaje.empty()) continue;

        // Verificar comando de salida
        if (mensaje == "/exit" || mensaje == "/salir") {
            break;
        }

        // Verificar comando de estado
        if (mensaje == "/estado") {
            std::cout << "[SISTEMA]: Estado de conexión: " << (verificarConexionRemota() ? "CONECTADO" : "DESCONECTADO") << std::endl;
            continue;
        }

        // Verificar si hay otro usuario conectado (solo para cliente)
        if (ConexionMongo::getModoConexion() == ConexionMongo::CLIENTE && !verificarConexionRemota()) {
            std::cout << "[SISTEMA]: No hay servidor conectado actualmente. Use '/estado' para verificar." << std::endl;
            continue;
        }

        // Para servidor, siempre permitir enviar mensajes
        if (ConexionMongo::getModoConexion() == ConexionMongo::SERVIDOR) {
            if (!verificarConexionRemota()) {
                std::cout << "[SISTEMA]: Mensaje enviado (esperando cliente). Use '/estado' para verificar conexiones." << std::endl;
            }
        }

        // Enviar mensaje
        std::string mensajeCompleto = "[" + nombreUsuario + ":" + obtenerModoTexto() + "]: " + mensaje;

        if (escribirMensaje(mensajeCompleto)) {
            // Mostrar nuestro propio mensaje
            std::lock_guard<std::mutex> lock(mtxMensajes);
            std::string mensajeConTimestamp = obtenerTimestamp() + " " + mensajeCompleto;
            historialMensajes.push_back(mensajeConTimestamp);
            mostrarMensaje(mensajeConTimestamp);
        }
        else {
            std::cout << "[ERROR]: No se pudo enviar el mensaje" << std::endl;
        }
    }

    detenerChat();
}

void AdministradorChatRedLocal::detenerChat() {
    chatActivo = false;
    servidorEnEjecucion = false;

    // Marcar como desconectado
    marcarConexion(false);

    // Esperar hilos
    if (hiloMonitoreo.joinable()) {
        hiloMonitoreo.join();
    }
    if (hiloEscucha.joinable()) {
        hiloEscucha.join();
    }
}