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

// Enlazar la librería automáticamente
#pragma comment(lib, "ws2_32.lib")

class ConexionMongo; // Forward declaration

class AdministradorChatSocket {
private:
	// Configuración de red
	static constexpr int PUERTO_SERVIDOR = 8888;
	static constexpr const char* DIRECCION_SERVIDOR = "127.0.0.1";
	static constexpr int BUFFER_SIZE = 1024;
	static constexpr int MAX_CLIENTES = 5;

	// Estado del socket y conexión
	SOCKET socketServidor;
	SOCKET socketCliente;
	std::vector<SOCKET> clientesConectados;

	// Estado del chat
	std::atomic<bool> chatActivo;
	std::atomic<bool> servidorEnEjecucion;
	std::string nombreUsuario;

	// Hilos
	std::thread hiloServidor;
	std::thread hiloCliente;
	std::thread hiloEscucha;

	// Sincronización
	std::mutex mtxMensajes;
	std::mutex mtxClientes;
	std::vector<std::string> historialMensajes;

	// Métodos privados para manejo de sockets
	bool inicializarWinsock();
	void limpiarWinsock();
	bool configurarSocketServidor();
	bool configurarSocketCliente();
	void aceptarClientes();
	void escucharMensajes();
	void manejarCliente(SOCKET clienteSocket);
	bool enviarMensaje(const std::string& mensaje);
	void enviarMensajeATodos(const std::string& mensaje);
	void desconectarCliente(SOCKET clienteSocket);

	// Utilidades
	std::string obtenerTimestamp();
	std::string obtenerModoTexto();
	void mostrarMensaje(const std::string& mensaje);
	void limpiarPantallaChat();

public:
	AdministradorChatSocket();
	~AdministradorChatSocket();

	// Métodos principales
	void iniciarChat();
	void detenerChat();
	bool hayClientesConectados() const;
	void configurarSegunModoMongoDB();
};