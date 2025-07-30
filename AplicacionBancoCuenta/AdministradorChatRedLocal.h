#pragma once
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "ConexionMongo.h"

#pragma comment(lib, "ws2_32.lib")

class AdministradorChatRedLocal
{
private:
    // Configuración de red
    static constexpr int PUERTO_CHAT = 8080;
    static constexpr const char* IP_SERVIDOR = "192.168.1.10";

    // Estado del chat
    std::atomic<bool> chatActivo;
    std::atomic<bool> servidorEnEjecucion;
    std::string nombreUsuario;

    // Sockets
    SOCKET socketServidor;
    SOCKET socketCliente;
    SOCKET socketConexion; // Para conexiones entrantes

    // Hilos
    std::thread hiloServidor;
    std::thread hiloEscucha;
    std::thread hiloEnvio;

    // Sincronización
    std::mutex mtxMensajes;
    std::vector<std::string> historialMensajes;

    // Inicialización de Winsock
    bool inicializarWinsock();
    void limpiarWinsock();

    // Funciones del servidor
    bool iniciarServidor();
    void ejecutarServidor();
    void manejarConexionEntrante();

    // Funciones del cliente
    bool conectarAServidor();
    void escucharMensajes();

    // Utilidades
    void mostrarMensaje(const std::string& mensaje);
    void limpiarPantallaChat();
    std::string obtenerModoTexto();

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