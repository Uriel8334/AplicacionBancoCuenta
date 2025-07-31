#pragma once

#define _CRT_SECURE_NO_WARNINGS
 

#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <fstream>
#include <chrono>
#include <filesystem>
#include "ConexionMongo.h"

// Chat local usando archivos temporales (sin sockets)
class AdministradorChatRedLocal
{
private:
    // Configuración de comunicación
    static constexpr const char* DIRECTORIO_CHAT = "C:\\temp\\chat_bancario\\";
    static constexpr const char* ARCHIVO_SERVIDOR = "servidor_mensajes.txt";
    static constexpr const char* ARCHIVO_CLIENTE = "cliente_mensajes.txt";
    static constexpr const char* ARCHIVO_ESTADO = "estado_conexion.txt";

    // Estado del chat
    std::atomic<bool> chatActivo;
    std::atomic<bool> servidorEnEjecucion;
    std::string nombreUsuario;
    std::string archivoMensajesLocal;
    std::string archivoMensajesRemoto;
    std::string rutaCompleta;

    // Hilos
    std::thread hiloMonitoreo;
    std::thread hiloEscucha;

    // Sincronización
    std::mutex mtxMensajes;
    std::vector<std::string> historialMensajes;
    size_t ultimoMensajeLeido;

    // Métodos de archivos
    bool crearDirectorioChat();
    bool escribirMensaje(const std::string& mensaje);
    std::vector<std::string> leerMensajesNuevos();
    bool marcarConexion(bool conectado);
    bool verificarConexionRemota();
    void limpiarArchivos();

    // Funciones del chat
    void ejecutarMonitoreo();
    void escucharMensajes();

    // Utilidades
    void mostrarMensaje(const std::string& mensaje);
    void limpiarPantallaChat();
    std::string obtenerModoTexto();
    std::string obtenerTimestamp();

public:
    AdministradorChatRedLocal();
    ~AdministradorChatRedLocal();

    // Métodos principales
    void iniciarChat();
    void detenerChat();
    bool hayClientesConectados() const;

    // Configuración según el modo de conexión MongoDB
    void configurarSegunModoMongoDB();
};