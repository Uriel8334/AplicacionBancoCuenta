#include "AdministradorChatRedLocal.h"
#include "Utilidades.h"
#include <iostream>
#include <conio.h>
#include <iomanip>
#include <sstream>
#include <chrono>

AdministradorChatRedLocal::AdministradorChatRedLocal()
    : chatActivo(false), servidorEnEjecucion(false), socketServidor(INVALID_SOCKET),
    socketCliente(INVALID_SOCKET), socketConexion(INVALID_SOCKET) {
    inicializarWinsock();
}

AdministradorChatRedLocal::~AdministradorChatRedLocal() {
    detenerChat();
    limpiarWinsock();
}

bool AdministradorChatRedLocal::inicializarWinsock() {
    WSADATA wsaData;
    int resultado = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (resultado != 0) {
        std::cerr << "Error inicializando Winsock: " << resultado << std::endl;
        return false;
    }
    return true;
}

void AdministradorChatRedLocal::limpiarWinsock() {
    WSACleanup();
}

void AdministradorChatRedLocal::configurarSegunModoMongoDB() {
    ConexionMongo::ModoConexion modo = ConexionMongo::getModoConexion();

    std::cout << "\n=== CONFIGURACIÓN DEL CHAT ===" << std::endl;
    if (modo == ConexionMongo::SERVIDOR) {
        std::cout << "• Modo: SERVIDOR (Esperará conexiones de clientes)" << std::endl;
        std::cout << "• IP de escucha: " << IP_SERVIDOR << std::endl;
        std::cout << "• Puerto: " << PUERTO_CHAT << std::endl;
    }
    else {
        std::cout << "• Modo: CLIENTE (Se conectará al servidor)" << std::endl;
        std::cout << "• IP del servidor: " << IP_SERVIDOR << std::endl;
        std::cout << "• Puerto: " << PUERTO_CHAT << std::endl;
    }
    std::cout << std::endl;
}

bool AdministradorChatRedLocal::iniciarServidor() {
    socketServidor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socketServidor == INVALID_SOCKET) {
        std::cerr << "Error creando socket servidor: " << WSAGetLastError() << std::endl;
        return false;
    }

    // Configurar dirección del servidor
    sockaddr_in direccionServidor;
    direccionServidor.sin_family = AF_INET;
    direccionServidor.sin_port = htons(PUERTO_CHAT);
    inet_pton(AF_INET, IP_SERVIDOR, &direccionServidor.sin_addr);

    // Vincular socket
    if (bind(socketServidor, (SOCKADDR*)&direccionServidor, sizeof(direccionServidor)) == SOCKET_ERROR) {
        std::cerr << "Error vinculando socket: " << WSAGetLastError() << std::endl;
        closesocket(socketServidor);
        return false;
    }

    // Escuchar conexiones
    if (listen(socketServidor, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Error escuchando en socket: " << WSAGetLastError() << std::endl;
        closesocket(socketServidor);
        return false;
    }

    servidorEnEjecucion = true;
    std::cout << "Servidor iniciado correctamente en " << IP_SERVIDOR << ":" << PUERTO_CHAT << std::endl;
    return true;
}

void AdministradorChatRedLocal::ejecutarServidor() {
    while (servidorEnEjecucion && chatActivo) {
        sockaddr_in direccionCliente;
        int tamDireccion = sizeof(direccionCliente);

        socketConexion = accept(socketServidor, (SOCKADDR*)&direccionCliente, &tamDireccion);
        if (socketConexion != INVALID_SOCKET) {
            char ipCliente[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &direccionCliente.sin_addr, ipCliente, INET_ADDRSTRLEN);

            std::lock_guard<std::mutex> lock(mtxMensajes);
            std::string mensajeConexion = "[SISTEMA]: Cliente conectado desde " + std::string(ipCliente);
            historialMensajes.push_back(mensajeConexion);
            mostrarMensaje(mensajeConexion);

            // Iniciar hilo de escucha para este cliente
            if (hiloEscucha.joinable()) {
                hiloEscucha.detach();
            }
            hiloEscucha = std::thread(&AdministradorChatRedLocal::escucharMensajes, this);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

bool AdministradorChatRedLocal::conectarAServidor() {
    socketCliente = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socketCliente == INVALID_SOCKET) {
        std::cerr << "Error creando socket cliente: " << WSAGetLastError() << std::endl;
        return false;
    }

    sockaddr_in direccionServidor;
    direccionServidor.sin_family = AF_INET;
    direccionServidor.sin_port = htons(PUERTO_CHAT);
    inet_pton(AF_INET, IP_SERVIDOR, &direccionServidor.sin_addr);

    if (connect(socketCliente, (SOCKADDR*)&direccionServidor, sizeof(direccionServidor)) == SOCKET_ERROR) {
        std::cerr << "Error conectando al servidor: " << WSAGetLastError() << std::endl;
        closesocket(socketCliente);
        return false;
    }

    std::cout << "Conectado al servidor exitosamente." << std::endl;
    return true;
}

void AdministradorChatRedLocal::escucharMensajes() {
    char buffer[1024];
    SOCKET socketActivo = (ConexionMongo::getModoConexion() == ConexionMongo::SERVIDOR) ?
        socketConexion : socketCliente;

    while (chatActivo && socketActivo != INVALID_SOCKET) {
        int bytesRecibidos = recv(socketActivo, buffer, sizeof(buffer) - 1, 0);
        if (bytesRecibidos > 0) {
            buffer[bytesRecibidos] = '\0';
            std::string mensaje(buffer);

            std::lock_guard<std::mutex> lock(mtxMensajes);
            historialMensajes.push_back(mensaje);
            mostrarMensaje(mensaje);
        }
        else if (bytesRecibidos == 0) {
            std::lock_guard<std::mutex> lock(mtxMensajes);
            std::string mensajeDesconexion = "[SISTEMA]: Cliente desconectado";
            historialMensajes.push_back(mensajeDesconexion);
            mostrarMensaje(mensajeDesconexion);
            break;
        }
        else {
            std::cerr << "Error recibiendo mensaje: " << WSAGetLastError() << std::endl;
            break;
        }
    }
}

void AdministradorChatRedLocal::mostrarMensaje(const std::string& mensaje) {
    // Obtener timestamp
    auto ahora = std::chrono::system_clock::now();
    auto tiempo = std::chrono::system_clock::to_time_t(ahora);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&tiempo), "%H:%M:%S");

    std::cout << "[" << ss.str() << "] " << mensaje << std::endl;
}

void AdministradorChatRedLocal::limpiarPantallaChat() {
    Utilidades::limpiarPantallaPreservandoMarquesina(1);
    std::cout << "=== CHAT LOCAL - " << obtenerModoTexto() << " ===" << std::endl;
    std::cout << "Escriba sus mensajes. Presione ESC para salir." << std::endl;
    std::cout << std::string(50, '-') << std::endl;

    // Mostrar historial reciente
    std::lock_guard<std::mutex> lock(mtxMensajes);
    size_t inicio = historialMensajes.size() > 10 ? historialMensajes.size() - 10 : 0;
    for (size_t i = inicio; i < historialMensajes.size(); ++i) {
        std::cout << historialMensajes[i] << std::endl;
    }
}

std::string AdministradorChatRedLocal::obtenerModoTexto() {
    return (ConexionMongo::getModoConexion() == ConexionMongo::SERVIDOR) ? "SERVIDOR" : "CLIENTE";
}

bool AdministradorChatRedLocal::hayClientesConectados() const {
    return (ConexionMongo::getModoConexion() == ConexionMongo::SERVIDOR) ?
        (socketConexion != INVALID_SOCKET) : (socketCliente != INVALID_SOCKET);
}

void AdministradorChatRedLocal::iniciarChat() {
    configurarSegunModoMongoDB();

    // Solicitar nombre de usuario
    std::cout << "Ingrese su nombre de usuario: ";
    std::getline(std::cin, nombreUsuario);
    if (nombreUsuario.empty()) {
        nombreUsuario = "Usuario" + std::to_string(std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count() % 1000);
    }

    chatActivo = true;

    // Configurar según el modo
    bool conexionExitosa = false;
    if (ConexionMongo::getModoConexion() == ConexionMongo::SERVIDOR) {
        conexionExitosa = iniciarServidor();
        if (conexionExitosa) {
            hiloServidor = std::thread(&AdministradorChatRedLocal::ejecutarServidor, this);
            std::cout << "Esperando conexiones de clientes..." << std::endl;
        }
    }
    else {
        conexionExitosa = conectarAServidor();
        if (conexionExitosa) {
            hiloEscucha = std::thread(&AdministradorChatRedLocal::escucharMensajes, this);
        }
    }

    if (!conexionExitosa) {
        std::cout << "No se pudo establecer la conexión. Verifique la red." << std::endl;
        system("pause");
        chatActivo = false;
        return;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    limpiarPantallaChat();

    // Bucle principal del chat
    std::string mensaje;
    while (chatActivo) {
        std::cout << nombreUsuario << ": ";
        std::getline(std::cin, mensaje);

        if (mensaje.empty()) continue;

        // Verificar si presionó ESC (esto requiere manejo especial en Windows)
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            break;
        }

        // Enviar mensaje
        if (hayClientesConectados()) {
            std::string mensajeCompleto = "[" + nombreUsuario + ":" + obtenerModoTexto() + "]: " + mensaje;

            SOCKET socketEnvio = (ConexionMongo::getModoConexion() == ConexionMongo::SERVIDOR) ?
                socketConexion : socketCliente;

            int resultado = send(socketEnvio, mensajeCompleto.c_str(),
                static_cast<int>(mensajeCompleto.length()), 0);

            if (resultado == SOCKET_ERROR) {
                std::cout << "[ERROR]: No se pudo enviar el mensaje" << std::endl;
            }
            else {
                // Mostrar nuestro propio mensaje
                std::lock_guard<std::mutex> lock(mtxMensajes);
                historialMensajes.push_back(mensajeCompleto);
                mostrarMensaje(mensajeCompleto);
            }
        }
        else {
            std::cout << "[SISTEMA]: No hay dispositivos conectados actualmente" << std::endl;
        }
    }

    detenerChat();
}

void AdministradorChatRedLocal::detenerChat() {
    chatActivo = false;
    servidorEnEjecucion = false;

    // Cerrar sockets
    if (socketConexion != INVALID_SOCKET) {
        closesocket(socketConexion);
        socketConexion = INVALID_SOCKET;
    }
    if (socketCliente != INVALID_SOCKET) {
        closesocket(socketCliente);
        socketCliente = INVALID_SOCKET;
    }
    if (socketServidor != INVALID_SOCKET) {
        closesocket(socketServidor);
        socketServidor = INVALID_SOCKET;
    }

    // Esperar hilos
    if (hiloServidor.joinable()) {
        hiloServidor.join();
    }
    if (hiloEscucha.joinable()) {
        hiloEscucha.join();
    }
    if (hiloEnvio.joinable()) {
        hiloEnvio.join();
    }
}