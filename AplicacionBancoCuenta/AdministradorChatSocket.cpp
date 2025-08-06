#define _CRT_SECURE_NO_WARNINGS

#include "AdministradorChatSocket.h"
#include "ConexionMongo.h"
#include "Utilidades.h"
#include <conio.h>

AdministradorChatSocket::AdministradorChatSocket()
	: socketServidor(INVALID_SOCKET), socketCliente(INVALID_SOCKET),
	chatActivo(false), servidorEnEjecucion(false), nuevosMessages(false),
	inputBuffer(""), cursorPos(0), mensajesMostrados(0) {
}

AdministradorChatSocket::~AdministradorChatSocket() {
	detenerChat();
}

std::string AdministradorChatSocket::obtenerDireccionServidor() const {
	// Usar la misma lógica que MongoDB: si es servidor usa localhost, si es cliente usa la IP del servidor
	if (ConexionMongo::getModoConexion() == ConexionMongo::SERVIDOR) {
		return "127.0.0.1";  // Servidor local
	}
	else {
		return "192.168.1.10";  // Servidor remoto
	}
}

bool AdministradorChatSocket::inicializarWinsock() {
	WSADATA wsaData;
	int resultado = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (resultado != 0) {
		std::cerr << "Error al inicializar Winsock: " << resultado << std::endl;
		return false;
	}
	std::cout << "Winsock inicializado correctamente." << std::endl;
	return true;
}

void AdministradorChatSocket::limpiarWinsock() {
	WSACleanup();
	std::cout << "Winsock limpiado." << std::endl;
}

bool AdministradorChatSocket::configurarSocketServidor() {
	// Crear socket servidor
	socketServidor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketServidor == INVALID_SOCKET) {
		std::cerr << "Error creando socket servidor: " << WSAGetLastError() << std::endl;
		return false;
	}

	// Permitir reutilizar la dirección
	char optval = 1;
	if (setsockopt(socketServidor, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == SOCKET_ERROR) {
		std::cerr << "Error configurando SO_REUSEADDR: " << WSAGetLastError() << std::endl;
	}

	// Configurar dirección del servidor
	sockaddr_in direccionServidor;
	ZeroMemory(&direccionServidor, sizeof(direccionServidor));
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = INADDR_ANY;
	direccionServidor.sin_port = htons(PUERTO_SERVIDOR);

	// Vincular socket a la dirección
	if (bind(socketServidor, reinterpret_cast<sockaddr*>(&direccionServidor), sizeof(direccionServidor)) == SOCKET_ERROR) {
		std::cerr << "Error en bind: " << WSAGetLastError() << std::endl;
		closesocket(socketServidor);
		socketServidor = INVALID_SOCKET;
		return false;
	}

	// Poner el socket en modo escucha
	if (listen(socketServidor, MAX_CLIENTES) == SOCKET_ERROR) {
		std::cerr << "Error en listen: " << WSAGetLastError() << std::endl;
		closesocket(socketServidor);
		socketServidor = INVALID_SOCKET;
		return false;
	}

	std::cout << "Servidor iniciado en puerto " << PUERTO_SERVIDOR << std::endl;
	return true;
}

bool AdministradorChatSocket::configurarSocketCliente() {
	// Crear socket cliente
	socketCliente = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketCliente == INVALID_SOCKET) {
		std::cerr << "Error creando socket cliente: " << WSAGetLastError() << std::endl;
		return false;
	}

	// Configurar dirección del servidor
	sockaddr_in direccionServidor;
	ZeroMemory(&direccionServidor, sizeof(direccionServidor));
	direccionServidor.sin_family = AF_INET;

	// Obtener la dirección correcta según el modo
	std::string direccionIP = obtenerDireccionServidor();

	// Usar inet_addr en lugar de inet_pton para mayor compatibilidad
	direccionServidor.sin_addr.s_addr = inet_addr(direccionIP.c_str());
	if (direccionServidor.sin_addr.s_addr == INADDR_NONE) {
		std::cerr << "Error: Dirección IP inválida: " << direccionIP << std::endl;
		closesocket(socketCliente);
		socketCliente = INVALID_SOCKET;
		return false;
	}

	direccionServidor.sin_port = htons(PUERTO_SERVIDOR);

	// Conectar al servidor
	std::cout << "Intentando conectar al servidor..." << std::endl;
	if (connect(socketCliente, reinterpret_cast<sockaddr*>(&direccionServidor), sizeof(direccionServidor)) == SOCKET_ERROR) {
		std::cerr << "Error conectando al servidor: " << WSAGetLastError() << std::endl;
		closesocket(socketCliente);
		socketCliente = INVALID_SOCKET;
		return false;
	}

	std::cout << "Conectado al servidor " << direccionIP << ":" << PUERTO_SERVIDOR << std::endl;
	return true;
}

void AdministradorChatSocket::aceptarClientes() {
	while (chatActivo && servidorEnEjecucion) {
		fd_set readfds;
		FD_ZERO(&readfds);
		FD_SET(socketServidor, &readfds);

		struct timeval timeout;
		timeout.tv_sec = 1;  // 1 segundo de timeout
		timeout.tv_usec = 0;

		int actividad = select(0, &readfds, nullptr, nullptr, &timeout);

		if (actividad > 0 && FD_ISSET(socketServidor, &readfds)) {
			SOCKET nuevoCliente = accept(socketServidor, nullptr, nullptr);
			if (nuevoCliente != INVALID_SOCKET) {
				{
					std::lock_guard<std::mutex> lock(mtxClientes);
					clientesConectados.push_back(nuevoCliente);
				}

				std::cout << "[SERVIDOR]: Cliente conectado. Total: " << clientesConectados.size() << std::endl;

				// Crear hilo para manejar este cliente
				std::thread hiloCliente(&AdministradorChatSocket::manejarCliente, this, nuevoCliente);
				hiloCliente.detach();
			}
		}
	}
}

void AdministradorChatSocket::manejarCliente(SOCKET clienteSocket) {
	char buffer[BUFFER_SIZE];
	while (chatActivo) {
		// Configurar timeout para recv
		fd_set readfds;
		FD_ZERO(&readfds);
		FD_SET(clienteSocket, &readfds);

		struct timeval timeout;
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;

		int actividad = select(0, &readfds, nullptr, nullptr, &timeout);

		if (actividad > 0 && FD_ISSET(clienteSocket, &readfds)) {
			int bytesRecibidos = recv(clienteSocket, buffer, BUFFER_SIZE - 1, 0);
			if (bytesRecibidos > 0) {
				buffer[bytesRecibidos] = '\0';
				std::string mensaje(buffer);

				// Agregar mensaje al historial y marcar para actualización
				{
					std::lock_guard<std::mutex> lock(mtxMensajes);
					std::string mensajeConTimestamp = obtenerTimestamp() + " " + mensaje;
					historialMensajes.push_back(mensajeConTimestamp);
					nuevosMessages = true;
				}

				// Reenviar mensaje a otros clientes
				enviarMensajeATodos(mensaje);
			}
			else if (bytesRecibidos == 0) {
				// Cliente desconectado
				break;
			}
			else {
				// Error
				int error = WSAGetLastError();
				if (error != WSAEWOULDBLOCK) {
					std::cerr << "Error recibiendo datos: " << error << std::endl;
					break;
				}
			}
		}
		else if (actividad == SOCKET_ERROR) {
			std::cerr << "Error en select: " << WSAGetLastError() << std::endl;
			break;
		}
		// Si actividad == 0, timeout - continuar el bucle
	}

	desconectarCliente(clienteSocket);
}

void AdministradorChatSocket::escucharMensajes() {
	char buffer[BUFFER_SIZE];
	while (chatActivo) {
		if (socketCliente != INVALID_SOCKET) {
			// Configurar timeout para recv
			fd_set readfds;
			FD_ZERO(&readfds);
			FD_SET(socketCliente, &readfds);

			struct timeval timeout;
			timeout.tv_sec = 0;
			timeout.tv_usec = 500000; // 500ms

			int actividad = select(0, &readfds, nullptr, nullptr, &timeout);

			if (actividad > 0 && FD_ISSET(socketCliente, &readfds)) {
				int bytesRecibidos = recv(socketCliente, buffer, BUFFER_SIZE - 1, 0);
				if (bytesRecibidos > 0) {
					buffer[bytesRecibidos] = '\0';
					std::string mensaje(buffer);

					// Agregar mensaje al historial y marcar para actualización
					{
						std::lock_guard<std::mutex> lock(mtxMensajes);
						std::string mensajeConTimestamp = obtenerTimestamp() + " " + mensaje;
						historialMensajes.push_back(mensajeConTimestamp);
						nuevosMessages = true;
					}
				}
				else if (bytesRecibidos == 0) {
					{
						std::lock_guard<std::mutex> lock(mtxMensajes);
						historialMensajes.push_back("[SISTEMA]: Desconectado del servidor");
						nuevosMessages = true;
					}
					break;
				}
				else {
					int error = WSAGetLastError();
					if (error != WSAEWOULDBLOCK) {
						std::cerr << "Error recibiendo datos: " << error << std::endl;
						break;
					}
				}
			}
			else if (actividad == SOCKET_ERROR) {
				std::cerr << "Error en select: " << WSAGetLastError() << std::endl;
				break;
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

void AdministradorChatSocket::actualizarPantalla() {
	while (chatActivo) {
		if (nuevosMessages) {
			// Limpiar la línea de entrada actual
			std::cout << "\r" << std::string(80, ' ') << "\r";

			// Mostrar mensajes nuevos
			{
				std::lock_guard<std::mutex> lock(mtxMensajes);
				if (historialMensajes.size() > mensajesMostrados) {
					for (size_t i = mensajesMostrados; i < historialMensajes.size(); ++i) {
						std::cout << historialMensajes[i] << std::endl;
					}
					mensajesMostrados = historialMensajes.size();
				}
				nuevosMessages = false;
			}

			// Restaurar la línea de entrada
			std::cout << nombreUsuario << ": " << inputBuffer << std::flush;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

std::string AdministradorChatSocket::leerEntradaNoBloquante() {
	std::string resultado;

	while (chatActivo) {
		if (_kbhit()) {
			char ch = _getch();

			if (ch == '\r' || ch == '\n') {
				// Enter presionado
				resultado = inputBuffer;
				inputBuffer.clear();
				cursorPos = 0;
				std::cout << std::endl;
				break;
			}
			else if (ch == '\b' || ch == 127) {
				// Backspace
				if (!inputBuffer.empty() && cursorPos > 0) {
					inputBuffer.erase(cursorPos - 1, 1);
					cursorPos--;

					// Limpiar línea y redibujar
					std::cout << "\r" << std::string(80, ' ') << "\r";
					std::cout << nombreUsuario << ": " << inputBuffer << std::flush;
				}
			}
			else if (ch >= 32 && ch <= 126) {
				// Carácter imprimible
				inputBuffer.insert(cursorPos, 1, ch);
				cursorPos++;

				// Limpiar línea y redibujar
				std::cout << "\r" << std::string(80, ' ') << "\r";
				std::cout << nombreUsuario << ": " << inputBuffer << std::flush;
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

	return resultado;
}

bool AdministradorChatSocket::enviarMensaje(const std::string& mensaje) {
	if (ConexionMongo::getModoConexion() == ConexionMongo::SERVIDOR) {
		// En modo servidor, enviar a todos los clientes
		enviarMensajeATodos(mensaje);
		return true;
	}
	else {
		// En modo cliente, enviar al servidor
		if (socketCliente != INVALID_SOCKET) {
			int resultado = send(socketCliente, mensaje.c_str(), static_cast<int>(mensaje.length()), 0);
			if (resultado == SOCKET_ERROR) {
				std::cerr << "Error enviando mensaje: " << WSAGetLastError() << std::endl;
				return false;
			}
			return true;
		}
		return false;
	}
}

void AdministradorChatSocket::enviarMensajeATodos(const std::string& mensaje) {
	std::lock_guard<std::mutex> lock(mtxClientes);
	for (auto it = clientesConectados.begin(); it != clientesConectados.end();) {
		SOCKET cliente = *it;
		int resultado = send(cliente, mensaje.c_str(), static_cast<int>(mensaje.length()), 0);
		if (resultado == SOCKET_ERROR) {
			// Cliente desconectado, remover de la lista
			closesocket(cliente);
			it = clientesConectados.erase(it);
			std::cout << "[SERVIDOR]: Cliente desconectado automáticamente." << std::endl;
		}
		else {
			++it;
		}
	}
}

void AdministradorChatSocket::desconectarCliente(SOCKET clienteSocket) {
	{
		std::lock_guard<std::mutex> lock(mtxClientes);
		auto it = std::find(clientesConectados.begin(), clientesConectados.end(), clienteSocket);
		if (it != clientesConectados.end()) {
			clientesConectados.erase(it);
		}
	}

	closesocket(clienteSocket);
	std::cout << "[SERVIDOR]: Cliente desconectado. Total: " << clientesConectados.size() << std::endl;
}

void AdministradorChatSocket::configurarSegunModoMongoDB() {
	ConexionMongo::ModoConexion modo = ConexionMongo::getModoConexion();
	std::string direccionServidor = obtenerDireccionServidor();

	std::cout << "\n=== CONFIGURACIÓN DEL CHAT CON SOCKETS ===" << std::endl;
	if (modo == ConexionMongo::SERVIDOR) {
		std::cout << "• Modo: SERVIDOR (Aceptará conexiones de clientes)" << std::endl;
		std::cout << "• Puerto: " << PUERTO_SERVIDOR << std::endl;
		std::cout << "• Máximo clientes: " << MAX_CLIENTES << std::endl;
	}
	else {
		std::cout << "• Modo: CLIENTE (Se conectará al servidor)" << std::endl;
		std::cout << "• Servidor: " << direccionServidor << ":" << PUERTO_SERVIDOR << std::endl;
	}
	std::cout << "• Protocolo: TCP/IP" << std::endl;
	std::cout << std::endl;
}

std::string AdministradorChatSocket::obtenerTimestamp() {
	auto ahora = std::chrono::system_clock::now();
	auto tiempo = std::chrono::system_clock::to_time_t(ahora);

	struct tm tiempoLocal;
	localtime_s(&tiempoLocal, &tiempo);

	std::stringstream ss;
	ss << "[" << std::put_time(&tiempoLocal, "%H:%M:%S") << "]";
	return ss.str();
}

std::string AdministradorChatSocket::obtenerModoTexto() {
	return (ConexionMongo::getModoConexion() == ConexionMongo::SERVIDOR) ? "SERVIDOR" : "CLIENTE";
}

void AdministradorChatSocket::mostrarMensaje(const std::string& mensaje) {
	std::cout << mensaje << std::endl;
}

void AdministradorChatSocket::limpiarPantallaChat() {
	Utilidades::limpiarPantallaPreservandoMarquesina(1);
	std::cout << "=== CHAT CON SOCKETS - " << obtenerModoTexto() << " ===" << std::endl;
	std::cout << "Escriba sus mensajes. Escriba '/exit' para salir." << std::endl;
	std::cout << "Comunicación TCP/IP - Puerto: " << PUERTO_SERVIDOR << std::endl;
	std::cout << std::string(60, '-') << std::endl;

	// Mostrar historial reciente
	std::lock_guard<std::mutex> lock(mtxMensajes);
	size_t inicio = historialMensajes.size() > 8 ? historialMensajes.size() - 8 : 0;
	for (size_t i = inicio; i < historialMensajes.size(); ++i) {
		std::cout << historialMensajes[i] << std::endl;
	}
	mensajesMostrados = historialMensajes.size();
}

bool AdministradorChatSocket::hayClientesConectados() const {
	if (ConexionMongo::getModoConexion() == ConexionMongo::SERVIDOR) {
		std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(mtxClientes));
		return !clientesConectados.empty();
	}
	else {
		return socketCliente != INVALID_SOCKET;
	}
}

void AdministradorChatSocket::iniciarChat() {
	configurarSegunModoMongoDB();

	// Inicializar Winsock
	if (!inicializarWinsock()) {
		std::cout << "Error: No se pudo inicializar Winsock." << std::endl;
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
	mensajesMostrados = 0;
	inputBuffer.clear();
	cursorPos = 0;

	// Configurar según el modo
	bool conectado = false;
	if (ConexionMongo::getModoConexion() == ConexionMongo::SERVIDOR) {
		if (configurarSocketServidor()) {
			servidorEnEjecucion = true;
			hiloServidor = std::thread(&AdministradorChatSocket::aceptarClientes, this);
			conectado = true;
		}
	}
	else {
		if (configurarSocketCliente()) {
			hiloEscucha = std::thread(&AdministradorChatSocket::escucharMensajes, this);
			conectado = true;
		}
	}

	if (!conectado) {
		std::cout << "Error: No se pudo establecer la conexión." << std::endl;
		detenerChat();
		system("pause");
		return;
	}

	// Dar tiempo para establecer conexiones
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	limpiarPantallaChat();

	std::cout << "Chat iniciado correctamente." << std::endl;
	std::cout << "Estado: " << (hayClientesConectados() ? "CONECTADO" : "ESPERANDO CONEXIONES") << std::endl;
	std::cout << std::endl;

	// Iniciar hilo de actualización de pantalla
	std::thread hiloActualizacion(&AdministradorChatSocket::actualizarPantalla, this);

	// Bucle principal del chat con entrada no bloqueante
	while (chatActivo) {
		std::cout << nombreUsuario << ": " << std::flush;
		std::string mensaje = leerEntradaNoBloquante();

		std::transform(mensaje.begin(), mensaje.end(), mensaje.begin(),
			[](unsigned char c) { return std::tolower(c); });

		if (mensaje.empty()) continue;

		// Verificar comando de salida
		if (mensaje  == "/exit" || mensaje == "/salir") {
			chatActivo = false;  // Detener todos los hilos
			Utilidades::limpiarPantallaPreservandoMarquesina(1);
			break;  // Cambiar return por break para salir del bucle
		}

		// Verificar comando de estado
		if (mensaje == "/estado" || mensaje == "/state") {
			std::lock_guard<std::mutex> lock(mtxClientes);
			{
				std::lock_guard<std::mutex> lockMsg(mtxMensajes);
				historialMensajes.push_back("[SISTEMA]: Conectados: " + std::to_string(clientesConectados.size()) + " clientes");
				nuevosMessages = true;
			}
			continue;
		}

		// /help para ayuda del usuario 

		if(mensaje == "/help" || mensaje == "/ayuda") {
			{
				std::lock_guard<std::mutex> lock(mtxMensajes);
				historialMensajes.push_back("[SISTEMA]: Comandos disponibles:");
				historialMensajes.push_back("  /exit o /salir - Salir del chat");
				historialMensajes.push_back("  /state o /estado - Mostrar estado de conexiones");
				historialMensajes.push_back("  /help o /ayuda - Mostrar esta ayuda");
				nuevosMessages = true;
			}
			continue;
		}

		// Verificar si hay conexiones (solo para cliente)
		if (ConexionMongo::getModoConexion() == ConexionMongo::CLIENTE && !hayClientesConectados()) {
			{
				std::lock_guard<std::mutex> lock(mtxMensajes);
				historialMensajes.push_back("[SISTEMA]: No hay conexión con el servidor. Use '/estado' para verificar.");
				nuevosMessages = true;
			}
			continue;
		}

		// Enviar mensaje
		std::string mensajeCompleto = "[" + nombreUsuario + ":" + obtenerModoTexto() + "]: " + mensaje;

		if (enviarMensaje(mensajeCompleto)) {
			// SOLO EL SERVIDOR muestra sus propios mensajes inmediatamente
			if (ConexionMongo::getModoConexion() == ConexionMongo::SERVIDOR) {
				std::lock_guard<std::mutex> lock(mtxMensajes);
				std::string mensajeConTimestamp = obtenerTimestamp() + " " + mensajeCompleto;
				historialMensajes.push_back(mensajeConTimestamp);
				nuevosMessages = true;
			}
		}
		else {
			{
				std::lock_guard<std::mutex> lock(mtxMensajes);
				historialMensajes.push_back("[ERROR]: No se pudo enviar el mensaje");
				nuevosMessages = true;
			}
		}
	}
	if (hiloActualizacion.joinable()) {
		hiloActualizacion.join();
	}
	detenerChat();
}

void AdministradorChatSocket::detenerChat() {
	chatActivo = false;
	servidorEnEjecucion = false;

	// Cerrar sockets
	if (socketServidor != INVALID_SOCKET) {
		closesocket(socketServidor);
		socketServidor = INVALID_SOCKET;
	}

	if (socketCliente != INVALID_SOCKET) {
		closesocket(socketCliente);
		socketCliente = INVALID_SOCKET;
	}

	// Cerrar conexiones de clientes
	{
		std::lock_guard<std::mutex> lock(mtxClientes);
		for (SOCKET cliente : clientesConectados) {
			closesocket(cliente);
		}
		clientesConectados.clear();
	}

	// Esperar hilos
	if (hiloServidor.joinable()) {
		hiloServidor.join();
	}
	if (hiloCliente.joinable()) {
		hiloCliente.join();
	}
	if (hiloEscucha.joinable()) {
		hiloEscucha.join();
	}

	// Limpiar Winsock
	limpiarWinsock();
}

void AdministradorChatSocket::iniciarChatInteractivo() {
	Utilidades::limpiarPantallaPreservandoMarquesina(1);

	std::cout << "=== SISTEMA DE CHAT LOCAL ===" << std::endl;
	std::cout << std::endl;
	std::cout << "Este chat utiliza la misma configuración de red que MongoDB:" << std::endl;

	if (ConexionMongo::getModoConexion() == ConexionMongo::SERVIDOR) {
		std::cout << "• Actuando como SERVIDOR (esperará conexiones)" << std::endl;
	}
	else {
		std::cout << "• Actuando como CLIENTE (se conectará al servidor)" << std::endl;
	}

	std::cout << std::endl;
	std::cout << "¿Desea continuar? (s/n): ";
	char respuesta;
	std::cin >> respuesta;
	std::cin.ignore(); // Limpiar el buffer

	if (respuesta == 's' || respuesta == 'S') {
		try {
			AdministradorChatSocket chat;
			chat.iniciarChat();
		}
		catch (const std::exception& e) {
			std::cout << "Error en el chat: " << e.what() << std::endl;
			system("pause");
		}
	}

	Utilidades::limpiarPantallaPreservandoMarquesina(1);
}