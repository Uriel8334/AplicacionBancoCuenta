// @file ConexionMongo.cpp

#include "ConexionMongo.h"
#include "Utilidades.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

// Inicialización de variables estáticas
ConexionMongo::ModoConexion ConexionMongo::modoActual = ConexionMongo::CLIENTE; // Por defecto cliente
std::string ConexionMongo::uriServidor = "";
std::string ConexionMongo::uriCliente = "";
std::string ConexionMongo::uriInternet = "";

/**
 * @brief Obtiene la URI sin imprimir mensajes de diagnóstico
 * @return URI de conexión apropiada sin logging
 */
std::string ConexionMongo::obtenerURISilenciosa() {
    // Configurar las URIs por defecto si no se han configurado
    if (uriServidor.empty() || uriCliente.empty() || uriInternet.empty()) {
        configurarURIs();
    }

    // Retornar la URI según el modo seleccionado sin imprimir mensajes
    switch (modoActual) {
    case SERVIDOR:
        return uriServidor;
    case CLIENTE:
        return uriCliente;
    case INTERNET:
        return uriInternet;
    default:
        return uriCliente;
    }
}

/**
 * @brief Verifica que el cliente MongoDB funcione correctamente con la base de datos "Banco"
 */
bool ConexionMongo::verificarCliente(mongocxx::client& cliente) {
    try {
        // Probar primero con la base de datos "Banco" (la real)
        auto banco_db = cliente["Banco"];
        auto doc = bsoncxx::builder::basic::document{};
        doc.append(bsoncxx::builder::basic::kvp("ping", 1));
        auto result = banco_db.run_command(doc.extract());

        // Verificar que la colección "personas" existe
        auto personas_collection = banco_db["personas"];
        auto count = personas_collection.estimated_document_count();

        std::cout << "✓ Cliente MongoDB verificado correctamente" << std::endl;
        std::cout << "✓ Base de datos 'Banco' accesible" << std::endl;
        std::cout << "✓ Colección 'personas' encontrada con " << count << " documentos" << std::endl;

        std::string modoTexto;
        switch (modoActual) {
        case SERVIDOR:
            modoTexto = "SERVIDOR";
            break;
        case CLIENTE:
            modoTexto = "CLIENTE";
            break;
        case INTERNET:
            modoTexto = "INTERNET";
            break;
        }
        std::cout << "✓ Conectado en modo: " << modoTexto << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "✗ Error al verificar cliente MongoDB: " << e.what() << std::endl;

        // Intentar fallback con admin si Banco falla
        try {
            auto admin_db = cliente["admin"];
            auto doc = bsoncxx::builder::basic::document{};
            doc.append(bsoncxx::builder::basic::kvp("ping", 1));
            auto result = admin_db.run_command(doc.extract());
            std::cout << "✓ Fallback: Conexión básica a 'admin' funciona" << std::endl;
            std::cout << "⚠ Pero la base de datos 'Banco' no está disponible" << std::endl;

            std::string modoTexto;
            switch (modoActual) {
            case SERVIDOR:
                modoTexto = "SERVIDOR";
                break;
            case CLIENTE:
                modoTexto = "CLIENTE";
                break;
            case INTERNET:
                modoTexto = "INTERNET";
                break;
            }
            std::cout << "✓ Conectado en modo: " << modoTexto << std::endl;
            system("pause");
            system("cls"); // Limpiar pantalla para evitar confusión
            return true; // Conexión básica funciona
        }
        catch (const std::exception& e2) {
            std::cerr << "✗ Error completo de conexión: " << e2.what() << std::endl;
            return false;
        }
    }
}

/**
 * @brief Prueba la conexión a MongoDB con timeout y mejor diagnóstico
 * @param uri URI de conexión a probar
 * @param timeoutSegundos Timeout en segundos (por defecto 10)
 * @return true si la conexión es exitosa, false en caso contrario
 */
bool ConexionMongo::probarConexion(const std::string& uri, int timeoutSegundos) {
    std::cout << "Probando conexión con URI: " << uri << std::endl;

    try {
        mongocxx::uri mongoUri{ uri };
        std::cout << "URI parseada correctamente" << std::endl;

        mongocxx::client cliente{ mongoUri };
        std::cout << "Cliente creado correctamente" << std::endl;

        auto start = std::chrono::steady_clock::now();

        // Probar primero con la base de datos real "Banco"
        std::vector<std::string> databases = { "Banco", "admin", "test" };
        bool exito = false;

        for (const auto& dbName : databases) {
            try {
                std::cout << "Probando base de datos: " << dbName << std::endl;
                auto db = cliente[dbName];

                // Realizar ping al servidor
                auto doc = bsoncxx::builder::basic::document{};
                doc.append(bsoncxx::builder::basic::kvp("ping", 1));

                auto result = db.run_command(doc.extract());

                std::cout << "✓ Ping exitoso a base de datos: " << dbName << std::endl;

                // Si es la base de datos Banco, verificar la colección personas
                if (dbName == "Banco") {
                    try {
                        auto personas_collection = db["personas"];
                        auto count = personas_collection.estimated_document_count();
                        std::cout << "✓ Colección 'personas' verificada con " << count << " documentos" << std::endl;
                    }
                    catch (const std::exception& e) {
                        std::cout << "⚠ Base de datos 'Banco' accesible pero problema con colección 'personas': " << e.what() << std::endl;
                    }
                }

                exito = true;
                break;
            }
            catch (const mongocxx::operation_exception& e) {
                std::cout << "✗ Error de operación en " << dbName << ": " << e.what() << std::endl;
                continue;
            }
            catch (const std::exception& e) {
                std::cout << "✗ Error general en base de datos " << dbName << ": " << e.what() << std::endl;
                continue;
            }
        }

        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);

        if (duration.count() > timeoutSegundos) {
            std::cerr << "Timeout en la conexión (" << duration.count() << "s)" << std::endl;
            return false;
        }

        if (exito) {
            std::cout << "✓ Conexión exitosa a MongoDB en: " << uri << std::endl;
            return true;
        }
        else {
            std::cerr << "✗ No se pudo conectar a ninguna base de datos" << std::endl;
            return false;
        }
    }
    catch (const mongocxx::operation_exception& e) {
        std::cerr << "✗ Error de operación MongoDB: " << e.what() << std::endl;
        try {
            auto& code = e.code();
            std::cerr << "   Código de error: " << code.value() << std::endl;
            std::cerr << "   Categoría: " << code.category().name() << std::endl;
        }
        catch (...) {
            // Ignorar si no se puede obtener información del código
        }
        return false;
    }
    catch (const std::exception& e) {
        std::cerr << "✗ Error general al conectar con MongoDB: " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief Ejecuta un diagnóstico completo de la conexión
 */
void ConexionMongo::ejecutarDiagnosticoCompleto() {
    std::cout << "\n=== DIAGNÓSTICO COMPLETO MONGODB ===" << std::endl;

    std::string modoTexto;
    switch (modoActual) {
    case SERVIDOR:
        modoTexto = "SERVIDOR (Local)";
        break;
    case CLIENTE:
        modoTexto = "CLIENTE (Remoto)";
        break;
    case INTERNET:
        modoTexto = "INTERNET (MongoDB Atlas)";
        break;
    }
    std::cout << "Modo de conexión: " << modoTexto << std::endl;

    // Información del driver
    std::cout << "\n1. Información del driver:" << std::endl;
    std::cout << "✓ Driver MongoDB C++ inicializado correctamente" << std::endl;

    // Probar diferentes configuraciones de URI según el modo
    std::cout << "\n2. Probando diferentes configuraciones de URI:" << std::endl;

    std::vector<std::string> uris;
    if (modoActual == SERVIDOR) {
        uris = {
            "mongodb://localhost:27017",
            "mongodb://localhost:27017/Banco",
            "mongodb://localhost:27017/?connectTimeoutMS=5000",
            "mongodb://localhost:27017/?serverSelectionTimeoutMS=3000",
            "mongodb://localhost:27017/?connectTimeoutMS=5000&serverSelectionTimeoutMS=3000&socketTimeoutMS=5000",
            "mongodb://localhost:27017/?authSource=admin"
        };
    }
    else if (modoActual == CLIENTE) {
        std::string ipDetectada = obtenerIPLocal();
        std::string redBase = ipDetectada.substr(0, ipDetectada.find_last_of('.'));

        uris = {
            uriCliente,
            uriCliente + "/Banco",
            uriCliente + "/?connectTimeoutMS=5000",
            uriCliente + "/?serverSelectionTimeoutMS=3000",
            uriCliente + "/?connectTimeoutMS=5000&serverSelectionTimeoutMS=3000&socketTimeoutMS=5000"
        };
    }
    else { // INTERNET
        uris = {
            uriInternet,
            uriInternet + "Banco",
            uriInternet + "?retryWrites=true&w=majority",
            uriInternet + "?ssl=true&authSource=admin"
        };
    }

    std::string uriExitosa;
    for (const auto& uri : uris) {
        std::cout << "\nProbando: " << uri << std::endl;
        if (probarConexion(uri, 10)) {
            std::cout << "✓ ÉXITO con esta configuración!" << std::endl;
            uriExitosa = uri;
            break;
        }
        std::cout << "✗ Falló" << std::endl;
    }

    if (!uriExitosa.empty()) {
        std::cout << "\n✓ URI recomendada: " << uriExitosa << std::endl;
    }
    else {
        std::cout << "\n=== SOLUCIONES RECOMENDADAS ===" << std::endl;
        if (modoActual == SERVIDOR) {
            Utilidades::limpiarPantallaPreservandoMarquesina(0);
            std::cout << "1. Verificar que MongoDB esté ejecutándose localmente" << std::endl;
            std::cout << "2. Ejecutar: net start MongoDB" << std::endl;
            std::cout << "3. Verificar el puerto 27017 esté disponible" << std::endl;
        }
        else if (modoActual == CLIENTE) {
            Utilidades::limpiarPantallaPreservandoMarquesina(0);
            std::cout << "1. Verificar que MongoDB esté ejecutándose en el servidor remoto" << std::endl;
            std::cout << "2. Revisar la configuración mongod.conf en el servidor:" << std::endl;
            std::cout << "   net:" << std::endl;
            std::cout << "     port: 27017" << std::endl;
            std::cout << "     bindIp: 0.0.0.0" << std::endl;
            std::cout << "3. Verificar el firewall de Windows en el servidor" << std::endl;
            std::cout << "4. Reiniciar el servicio MongoDB después de cambios" << std::endl;
        }
        else { // INTERNET
            Utilidades::limpiarPantallaPreservandoMarquesina(0);
            std::cout << "1. Verificar conexión a Internet" << std::endl;
            std::cout << "2. Comprobar credenciales de MongoDB Atlas" << std::endl;
            std::cout << "3. Verificar whitelist de IPs en MongoDB Atlas" << std::endl;
            std::cout << "4. Comprobar que el cluster esté activo" << std::endl;
        }
    }
	system("pause");
    std::cout << "\n=== FIN DEL DIAGNÓSTICO ===" << std::endl;
}

/**
 * @brief Detecta automáticamente las IPs de red local para configurar URI de cliente
 * @return URI configurada automáticamente para red local
 */
std::string ConexionMongo::detectarURIClienteAutomatica() {
    std::string ipLocal = obtenerIPLocal();
    std::string redBase = ipLocal.substr(0, ipLocal.find_last_of('.'));

    std::cout << "Detectando configuración automática de red..." << std::endl;
    std::cout << "IP local detectada: " << ipLocal << std::endl;
    std::cout << "Red base: " << redBase << std::endl;

    // Escanear la red para encontrar servidores MongoDB
    std::vector<std::string> servidoresEncontrados = escanearRedMongoDB(redBase);

    if (!servidoresEncontrados.empty()) {
        std::string ipServidor = servidoresEncontrados[0]; // Usar el primero encontrado
        std::string uri = "mongodb://" + ipServidor + ":27017/?connectTimeoutMS=5000&serverSelectionTimeoutMS=3000";
        std::cout << "Servidor MongoDB detectado automáticamente en: " << ipServidor << std::endl;
        return uri;
    }

    // Fallback: usar configuración por defecto basada en la red detectada
    std::string uri = "mongodb://" + redBase + ".10:27017/?connectTimeoutMS=5000&serverSelectionTimeoutMS=3000";
    std::cout << "No se detectó servidor MongoDB, usando configuración por defecto: " << uri << std::endl;
    return uri;
}

/**
 * @brief Obtiene la IP local del adaptador de red principal
 * @return IP local detectada
 */
std::string ConexionMongo::obtenerIPLocal() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return "192.168.1.2"; // Fallback
    }

    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR) {
        WSACleanup();
        return "192.168.1.2"; // Fallback
    }

    struct addrinfo hints, * result;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(hostname, NULL, &hints, &result) != 0) {
        WSACleanup();
        return "192.168.1.2"; // Fallback
    }

    struct sockaddr_in* addr_in = (struct sockaddr_in*)result->ai_addr;
    std::string ipLocal = inet_ntoa(addr_in->sin_addr);

    freeaddrinfo(result);
    WSACleanup();

    // Filtrar IPs de loopback y usar solo IPs de red local
    if (ipLocal.find("127.") == 0 || ipLocal.find("169.254.") == 0) {
        return "192.168.1.2"; // Fallback para red típica
    }

    return ipLocal;
}

/**
 * @brief Escanea la red local para encontrar servidores MongoDB
 * @param redBase Red base para escanear (ej: "192.168.1")
 * @return Vector de IPs que tienen MongoDB disponible
 */
std::vector<std::string> ConexionMongo::escanearRedMongoDB(const std::string& redBase) {
    std::vector<std::string> servidoresEncontrados;

    std::cout << "Escaneando red " << redBase << ".x para servidores MongoDB..." << std::endl;

    // Escanear IPs comunes para servidores (1-20)
    for (int i = 1; i <= 20; i++) {
        std::string ip = redBase + "." + std::to_string(i);
        std::string uri = "mongodb://" + ip + ":27017";

        std::cout << "Probando " << ip << "...";

        try {
            // Intentar conexión rápida con timeout corto
            mongocxx::uri mongoUri{ uri + "/?connectTimeoutMS=100&serverSelectionTimeoutMS=100" };
            mongocxx::client cliente{ mongoUri };

            auto db = cliente["Banco"];
            auto doc = bsoncxx::builder::basic::document{};
            doc.append(bsoncxx::builder::basic::kvp("ping", 1));
            auto result = db.run_command(doc.extract());

            std::cout << " ✓ MongoDB encontrado!" << std::endl;
            servidoresEncontrados.push_back(ip);
        }
        catch (const std::exception&) {
            std::cout << " ✗" << std::endl;
        }

        // No escanear demasiado tiempo
        if (servidoresEncontrados.size() >= 3) break;
    }

    return servidoresEncontrados;
}

/**
 * @brief Obtiene la URI según el modo de conexión configurado
 * @return URI de conexión apropiada
 */
std::string ConexionMongo::obtenerURI() {
    // Configurar las URIs por defecto si no se han configurado
    if (uriServidor.empty() || uriCliente.empty() || uriInternet.empty()) {
        configurarURIs();
    }

    // Retornar la URI según el modo seleccionado
    switch (modoActual) {
    case SERVIDOR:
        std::cout << "Usando URI de SERVIDOR (Local): " << uriServidor << std::endl;
        return uriServidor;
    case CLIENTE:
        std::cout << "Usando URI de CLIENTE (Remoto): " << uriCliente << std::endl;
        return uriCliente;
    case INTERNET:
        std::cout << "Usando URI de INTERNET (MongoDB Atlas): " << uriInternet << std::endl;
        return uriInternet;
    default:
        std::cout << "Modo desconocido, usando CLIENTE por defecto: " << uriCliente << std::endl;
        return uriCliente;
    }
}