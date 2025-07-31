#define _CRT_SECURE_NO_WARNINGS

#include "AdministradorChatSocket.h"
#include "ConexionMongo.h"
#include "Utilidades.h"
#include <conio.h>

// ========================= IMPLEMENTACIONES DE INTERFACES =========================

bool SocketServerManager::inicializar() {
    WSADATA wsaData;
    int resultado = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (resultado != 0) {
        std::cerr << "Error al inicializar Winsock: " << resultado << std::endl;
        return false;
    }
    std::cout << "Winsock inicializado correctamente." << std::endl;
    return true;
}

void SocketServerManager::limpiar() {
    WSACleanup();
    std::cout << "Winsock limpiado." << std::endl;
}

bool SocketServerManager::configurar() {
    socketServidor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socketServidor == INVALID_SOCKET) {
        std::cerr << "Error creando socket servidor: " << WSAGetLastError() << std::endl;
        return false;
    }

    char optval = 1;
    if (setsockopt(socketServidor, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == SOCKET_ERROR) {
        std::cerr << "Error configurando SO_REUSEADDR: " << WSAGetLastError() << std::endl;
    }

    sockaddr_in direccionServidor{};
    direccionServidor.sin_family = AF_INET;
    direccionServidor.sin_addr.s_addr = INADDR_ANY;
    direccionServidor.sin_port = htons(PUERTO_SERVIDOR);

    // Eliminar if dentro de for usando std::all_of
    const std::vector<std::function<int()>> operacionesSocket = {
        [&]() { return bind(socketServidor, reinterpret_cast<sockaddr*>(&direccionServidor), sizeof(direccionServidor)); },
        [&]() { return listen(socketServidor, MAX_CLIENTES); }
    };

    auto operacionFallida = std::find_if(operacionesSocket.begin(), operacionesSocket.end(),
        [](const auto& operacion) {
            return operacion() == SOCKET_ERROR;
        });

    if (operacionFallida != operacionesSocket.end()) {
        std::cerr << "Error en operación socket: " << WSAGetLastError() << std::endl;
        closesocket(socketServidor);
        socketServidor = INVALID_SOCKET;
        return false;
    }

    std::cout << "Servidor iniciado en puerto " << PUERTO_SERVIDOR << std::endl;
    return true;
}

bool SocketClientManager::inicializar() {
    WSADATA wsaData;
    int resultado = WSAStartup(MAKEWORD(2, 2), &wsaData);
    return resultado == 0;
}

void SocketClientManager::limpiar() {
    WSACleanup();
}

bool SocketClientManager::configurar() {
    socketCliente = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socketCliente == INVALID_SOCKET) {
        std::cerr << "Error creando socket cliente: " << WSAGetLastError() << std::endl;
        return false;
    }

    sockaddr_in direccionServ{};
    direccionServ.sin_family = AF_INET;
    direccionServ.sin_addr.s_addr = inet_addr(direccionServidor.c_str());

    if (direccionServ.sin_addr.s_addr == INADDR_NONE) {
        std::cerr << "Error: Dirección IP inválida: " << direccionServidor << std::endl;
        closesocket(socketCliente);
        socketCliente = INVALID_SOCKET;
        return false;
    }

    direccionServ.sin_port = htons(PUERTO_SERVIDOR);

    std::cout << "Intentando conectar al servidor..." << std::endl;
    if (connect(socketCliente, reinterpret_cast<sockaddr*>(&direccionServ), sizeof(direccionServ)) == SOCKET_ERROR) {
        std::cerr << "Error conectando al servidor: " << WSAGetLastError() << std::endl;
        closesocket(socketCliente);
        socketCliente = INVALID_SOCKET;
        return false;
    }

    std::cout << "Conectado al servidor " << direccionServidor << ":" << PUERTO_SERVIDOR << std::endl;
    return true;
}

void MessageProcessor::procesarMensaje(const std::string& mensaje) {
    std::string mensajeConTimestamp = timestampGenerator() + " " + mensaje;
    agregarMensajeAlHistorial(mensajeConTimestamp);
}

void MessageProcessor::agregarMensajeAlHistorial(const std::string& mensaje) {
    std::lock_guard<std::mutex> lock(mtxMensajes);
    historial.push_back(mensaje);
    nuevosMessages = true;
}

void ConsoleInterface::actualizarPantalla() {
    while (chatActivo) {
        if (nuevosMessages) {
            std::cout << "\r" << std::string(80, ' ') << "\r";

            {
                std::lock_guard<std::mutex> lock(mtxMensajes);
                if (historial.size() > mensajesMostrados) {
                    // Crear vista de los mensajes nuevos y procesarlos directamente
                    auto mensajesNuevos = std::vector<std::string>(
                        historial.begin() + mensajesMostrados,
                        historial.end()
                    );

                    // Usar for_each sin if interno
                    std::for_each(mensajesNuevos.begin(), mensajesNuevos.end(),
                        [](const std::string& mensaje) {
                            std::cout << mensaje << std::endl;
                        });

                    mensajesMostrados = historial.size();
                }
                nuevosMessages = false;
            }

            std::cout << nombreUsuario << ": " << inputBuffer << std::flush;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

std::string ConsoleInterface::leerEntrada() {
    std::string resultado;

    while (chatActivo) {
        if (_kbhit()) {
            char ch = _getch();

            // Separar lógica de teclas especiales y normales
            const std::unordered_set<char> teclasEnter = { '\r', '\n' };
            const std::unordered_set<char> teclasBackspace = { '\b', 127 };

            if (teclasEnter.count(ch)) {
                resultado = inputBuffer;
                inputBuffer.clear();
                cursorPos = 0;
                std::cout << std::endl;
                break;
            }
            else if (teclasBackspace.count(ch)) {
                if (!inputBuffer.empty() && cursorPos > 0) {
                    inputBuffer.erase(cursorPos - 1, 1);
                    cursorPos--;
                    std::cout << "\r" << std::string(80, ' ') << "\r";
                    std::cout << nombreUsuario << ": " << inputBuffer << std::flush;
                }
            }
            else if (ch >= 32 && ch <= 126) {
                inputBuffer.insert(cursorPos, 1, ch);
                cursorPos++;
                std::cout << "\r" << std::string(80, ' ') << "\r";
                std::cout << nombreUsuario << ": " << inputBuffer << std::flush;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    return resultado;
}

void ConsoleInterface::mostrarMensaje(const std::string& mensaje) {
    std::cout << mensaje << std::endl;
}

// ========================= CLASE PRINCIPAL =========================

AdministradorChatSocket::AdministradorChatSocket()
    : socketServidor(INVALID_SOCKET), socketCliente(INVALID_SOCKET),
    chatActivo(false), servidorEnEjecucion(false), nuevosMessages(false),
    inputBuffer(""), cursorPos(0), mensajesMostrados(0) {

    inicializarComandos();
}

AdministradorChatSocket::~AdministradorChatSocket() {
    detenerChat();
}

void AdministradorChatSocket::inicializarComandos() {
    comandosDisponibles = {
        {"/exit", [this]() { chatActivo = false; }},
        {"/salir", [this]() { chatActivo = false; }},
        {"/estado", [this]() { mostrarEstadoConexion(); }}
    };
}

std::unique_ptr<ISocketManager> AdministradorChatSocket::crearSocketManager() {
    if (ConexionMongo::getModoConexion() == ConexionMongo::SERVIDOR) {
        return std::make_unique<SocketServerManager>(socketServidor);
    }
    return std::make_unique<SocketClientManager>(socketCliente, obtenerDireccionServidor());
}

std::unique_ptr<IMessageHandler> AdministradorChatSocket::crearMessageHandler() {
    return std::make_unique<MessageProcessor>(
        historialMensajes,
        mtxMensajes,
        nuevosMessages,
        [this]() { return obtenerTimestamp(); }
    );
}

std::unique_ptr<IUserInterface> AdministradorChatSocket::crearUserInterface() {
    return std::make_unique<ConsoleInterface>(
        inputBuffer, cursorPos, nombreUsuario, historialMensajes,
        mensajesMostrados, mtxMensajes, chatActivo, nuevosMessages
    );
}

std::string AdministradorChatSocket::obtenerDireccionServidor() const {
    const std::unordered_map<ConexionMongo::ModoConexion, std::string> direcciones = {
        {ConexionMongo::SERVIDOR, "127.0.0.1"},
        {ConexionMongo::CLIENTE, "192.168.1.10"}
    };

    auto it = direcciones.find(ConexionMongo::getModoConexion());
    return (it != direcciones.end()) ? it->second : "127.0.0.1";
}

void AdministradorChatSocket::procesarActividadSocket(int actividad, SOCKET socket,
    std::function<void()> onActivity,
    std::function<void()> onError) {
    if (actividad > 0) {
        onActivity();
    }
    else if (actividad == SOCKET_ERROR) {
        onError();
    }
}

bool AdministradorChatSocket::procesarComandos(const std::string& mensaje) {
    auto it = comandosDisponibles.find(mensaje);
    if (it != comandosDisponibles.end()) {
        it->second();
        return true;
    }
    return false;
}

void AdministradorChatSocket::mostrarEstadoConexion() {
    std::lock_guard<std::mutex> lock(mtxClientes);
    messageHandler->agregarMensajeAlHistorial(
        "[SISTEMA]: Conectados: " + std::to_string(clientesConectados.size()) + " clientes"
    );
}

void AdministradorChatSocket::manejarDesconexionCliente(SOCKET clienteSocket) {
    {
        std::lock_guard<std::mutex> lock(mtxClientes);
        clientesConectados.erase(
            std::remove(clientesConectados.begin(), clientesConectados.end(), clienteSocket),
            clientesConectados.end()
        );
    }
    closesocket(clienteSocket);
    std::cout << "[SERVIDOR]: Cliente desconectado. Total: " << clientesConectados.size() << std::endl;
}

bool AdministradorChatSocket::hayClientesConectados() const {
    if (ConexionMongo::getModoConexion() == ConexionMongo::SERVIDOR) {
        std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(mtxClientes));
        return !clientesConectados.empty();
    }
    return socketCliente != INVALID_SOCKET;
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

void AdministradorChatSocket::configurarSegunModoMongoDB() {
    ConexionMongo::ModoConexion modo = ConexionMongo::getModoConexion();
    std::string direccionServidor = obtenerDireccionServidor();

    std::cout << "\n=== CONFIGURACIÓN DEL CHAT CON SOCKETS ===" << std::endl;
    if (modo == ConexionMongo::SERVIDOR) {
        std::cout << "• Modo: SERVIDOR (Aceptará conexiones de clientes)" << std::endl;
        std::cout << "• Puerto: 8888" << std::endl;
        std::cout << "• Máximo clientes: 5" << std::endl;
    }
    else {
        std::cout << "• Modo: CLIENTE (Se conectará al servidor)" << std::endl;
        std::cout << "• Servidor: " << direccionServidor << ":8888" << std::endl;
    }
    std::cout << "• Protocolo: TCP/IP" << std::endl;
    std::cout << std::endl;
}

void AdministradorChatSocket::iniciarChat() {
    configurarSegunModoMongoDB();

    // Crear dependencias usando Factory Methods (Dependency Injection)
    socketManager = crearSocketManager();
    messageHandler = crearMessageHandler();
    userInterface = crearUserInterface();

    if (!socketManager->inicializar()) {
        std::cout << "Error: No se pudo inicializar Winsock." << std::endl;
        system("pause");
        return;
    }

    std::cout << "Ingrese su nombre de usuario: ";
    std::getline(std::cin, nombreUsuario);
    if (nombreUsuario.empty()) {
        nombreUsuario = "Usuario" + std::to_string(
            std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()
            ).count() % 1000
        );
    }

    chatActivo = true;

    if (!socketManager->configurar()) {
        std::cout << "Error: No se pudo establecer la conexión." << std::endl;
        detenerChat();
        system("pause");
        return;
    }

    std::cout << "Chat iniciado correctamente." << std::endl;
    std::cout << "Estado: " << (hayClientesConectados() ? "CONECTADO" : "ESPERANDO CONEXIONES") << std::endl;

    // Bucle principal simplificado
    while (chatActivo) {
        std::cout << nombreUsuario << ": " << std::flush;
        std::string mensaje = userInterface->leerEntrada();

        if (mensaje.empty()) continue;

        // Procesar comandos usando Command Pattern
        if (procesarComandos(mensaje)) continue;

        // Verificar conexión para clientes
        if (ConexionMongo::getModoConexion() == ConexionMongo::CLIENTE && !hayClientesConectados()) {
            messageHandler->agregarMensajeAlHistorial(
                "[SISTEMA]: No hay conexión con el servidor. Use '/estado' para verificar."
            );
            continue;
        }

        // Enviar mensaje
        std::string mensajeCompleto = "[" + nombreUsuario + ":" + obtenerModoTexto() + "]: " + mensaje;

        // Solo el servidor muestra sus propios mensajes inmediatamente
        if (ConexionMongo::getModoConexion() == ConexionMongo::SERVIDOR) {
            messageHandler->procesarMensaje(mensajeCompleto);
        }
    }

    detenerChat();
}

void AdministradorChatSocket::detenerChat() {
    chatActivo = false;
    servidorEnEjecucion = false;

    // Crear predicado para filtrar sockets válidos
    auto esSocketValido = [](SOCKET socket) {
        return socket != INVALID_SOCKET;
        };

    // Filtrar y cerrar solo sockets válidos
    std::vector<SOCKET> socketsParaCerrar = { socketServidor, socketCliente };

    // Filtrar sockets válidos y procesarlos
    std::vector<SOCKET> socketsValidos;
    std::copy_if(socketsParaCerrar.begin(), socketsParaCerrar.end(),
        std::back_inserter(socketsValidos), esSocketValido);

    // Procesar solo sockets válidos
    std::for_each(socketsValidos.begin(), socketsValidos.end(),
        [this](SOCKET socket) {
            closesocket(socket);
            if (socket == socketServidor) socketServidor = INVALID_SOCKET;
            if (socket == socketCliente) socketCliente = INVALID_SOCKET;
        });

    // Cerrar conexiones de clientes usando range-based for sin if interno
    {
        std::lock_guard<std::mutex> lock(mtxClientes);
        // Usar for_each directo para cerrar todos los clientes
        std::for_each(clientesConectados.begin(), clientesConectados.end(),
            [](SOCKET cliente) {
                closesocket(cliente);
            });
        clientesConectados.clear();
    }

    // Crear predicado para hilos que se pueden joinear
    auto esHiloJoineable = [](const std::thread& hilo) {
        return hilo.joinable();
        };

    // Procesar hilos: filtrar los que se pueden joinear y procesarlos
    std::vector<std::reference_wrapper<std::thread>> todosLosHilos = {
        std::ref(hiloServidor), std::ref(hiloCliente), std::ref(hiloEscucha)
    };

    // Filtrar hilos joineables
    std::vector<std::reference_wrapper<std::thread>> hilosJoineables;
    std::copy_if(todosLosHilos.begin(), todosLosHilos.end(),
        std::back_inserter(hilosJoineables),
        [](const std::reference_wrapper<std::thread>& hilo) {
            return hilo.get().joinable();
        });

    // Joinear solo los hilos válidos
    std::for_each(hilosJoineables.begin(), hilosJoineables.end(),
        [](std::reference_wrapper<std::thread> hilo) {
            hilo.get().join();
        });

    if (socketManager) {
        socketManager->limpiar();
    }
}