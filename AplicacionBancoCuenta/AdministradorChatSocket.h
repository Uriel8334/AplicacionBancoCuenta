#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <memory>
#include <functional>
#include <unordered_set>

// Enlazar la librería automáticamente
#pragma comment(lib, "ws2_32.lib")

class ConexionMongo; // Forward declaration

// Interfaces para cumplir con SOLID (Interface Segregation Principle)
class ISocketManager {
public:
    virtual ~ISocketManager() = default;
    virtual bool inicializar() = 0;
    virtual void limpiar() = 0;
    virtual bool configurar() = 0;
};

class IMessageHandler {
public:
    virtual ~IMessageHandler() = default;
    virtual void procesarMensaje(const std::string& mensaje) = 0;
    virtual void agregarMensajeAlHistorial(const std::string& mensaje) = 0;
};

class IUserInterface {
public:
    virtual ~IUserInterface() = default;
    virtual void actualizarPantalla() = 0;
    virtual std::string leerEntrada() = 0;
    virtual void mostrarMensaje(const std::string& mensaje) = 0;
};

// Clases específicas que implementan las interfaces
class SocketServerManager : public ISocketManager {
private:
    SOCKET& socketServidor;
    static constexpr int PUERTO_SERVIDOR = 8888;
    static constexpr int MAX_CLIENTES = 5;

public:
    explicit SocketServerManager(SOCKET& socket) : socketServidor(socket) {}
    bool inicializar() override;
    void limpiar() override;
    bool configurar() override;
};

class SocketClientManager : public ISocketManager {
private:
    SOCKET& socketCliente;
    const std::string& direccionServidor;
    static constexpr int PUERTO_SERVIDOR = 8888;

public:
    SocketClientManager(SOCKET& socket, const std::string& direccion)
        : socketCliente(socket), direccionServidor(direccion) {
    }
    bool inicializar() override;
    void limpiar() override;
    bool configurar() override;
};

class MessageProcessor : public IMessageHandler {
private:
    std::vector<std::string>& historial;
    std::mutex& mtxMensajes;
    std::atomic<bool>& nuevosMessages;
    std::function<std::string()> timestampGenerator;

public:
    MessageProcessor(std::vector<std::string>& hist, std::mutex& mtx,
        std::atomic<bool>& flag, std::function<std::string()> timestampGen)
        : historial(hist), mtxMensajes(mtx), nuevosMessages(flag), timestampGenerator(timestampGen) {
    }

    void procesarMensaje(const std::string& mensaje) override;
    void agregarMensajeAlHistorial(const std::string& mensaje) override;
};

class ConsoleInterface : public IUserInterface {
private:
    std::string& inputBuffer;
    size_t& cursorPos;
    const std::string& nombreUsuario;
    const std::vector<std::string>& historial;
    size_t& mensajesMostrados;
    std::mutex& mtxMensajes;
    std::atomic<bool>& chatActivo;
    std::atomic<bool>& nuevosMessages;

public:
    ConsoleInterface(std::string& buffer, size_t& cursor, const std::string& nombre,
        const std::vector<std::string>& hist, size_t& mostrados,
        std::mutex& mtx, std::atomic<bool>& activo, std::atomic<bool>& nuevos)
        : inputBuffer(buffer), cursorPos(cursor), nombreUsuario(nombre),
        historial(hist), mensajesMostrados(mostrados), mtxMensajes(mtx),
        chatActivo(activo), nuevosMessages(nuevos) {
    }

    void actualizarPantalla() override;
    std::string leerEntrada() override;
    void mostrarMensaje(const std::string& mensaje) override;
};

class AdministradorChatSocket {
private:
    // Configuración de red
    static constexpr int BUFFER_SIZE = 1024;

    // Estado del socket y conexión
    SOCKET socketServidor;
    SOCKET socketCliente;
    std::vector<SOCKET> clientesConectados;

    // Estado del chat
    std::atomic<bool> chatActivo;
    std::atomic<bool> servidorEnEjecucion;
    std::string nombreUsuario;

    // Control de interfaz no bloqueante
    std::atomic<bool> nuevosMessages;
    std::string inputBuffer;
    size_t cursorPos;
    size_t mensajesMostrados;

    // Hilos
    std::thread hiloServidor;
    std::thread hiloCliente;
    std::thread hiloEscucha;

    // Sincronización
    std::mutex mtxMensajes;
    std::mutex mtxClientes;
    std::vector<std::string> historialMensajes;

    // Dependencias (Dependency Injection)
    std::unique_ptr<ISocketManager> socketManager;
    std::unique_ptr<IMessageHandler> messageHandler;
    std::unique_ptr<IUserInterface> userInterface;

    // Métodos privados optimizados
    void procesarActividadSocket(int actividad, SOCKET socket,
        std::function<void()> onActivity,
        std::function<void()> onError);
    void manejarDesconexionCliente(SOCKET clienteSocket);
    bool procesarComandos(const std::string& mensaje);
    void mostrarEstadoConexion();

    // Command Pattern para comandos del chat
    using ComandoChat = std::function<void()>;
    std::unordered_map<std::string, ComandoChat> comandosDisponibles;
    void inicializarComandos();

public:
    AdministradorChatSocket();
    ~AdministradorChatSocket();

    // Métodos principales
    void iniciarChat();
    void detenerChat();
    bool hayClientesConectados() const;
    void configurarSegunModoMongoDB();

    // Factory methods para crear las dependencias
    std::unique_ptr<ISocketManager> crearSocketManager();
    std::unique_ptr<IMessageHandler> crearMessageHandler();
    std::unique_ptr<IUserInterface> crearUserInterface();

    // Utilidades
    std::string obtenerDireccionServidor() const;
    std::string obtenerTimestamp();
    std::string obtenerModoTexto();
};