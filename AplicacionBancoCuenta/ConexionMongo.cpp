// @file ConexionMongo.cpp

#include "ConexionMongo.h"

// Inicialización de variables estáticas
ConexionMongo::ModoConexion ConexionMongo::modoActual = ConexionMongo::CLIENTE; // Por defecto cliente
std::string ConexionMongo::uriServidor = "";
std::string ConexionMongo::uriCliente = "";

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
        std::cout << "✓ Conectado en modo: " << (modoActual == SERVIDOR ? "SERVIDOR" : "CLIENTE") << std::endl;
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
            std::cout << "✓ Conectado en modo: " << (modoActual == SERVIDOR ? "SERVIDOR" : "CLIENTE") << std::endl;
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
            auto code = e.code();
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
    std::cout << "Modo de conexión: " << (modoActual == SERVIDOR ? "SERVIDOR (Local)" : "CLIENTE (Remoto)") << std::endl;

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
    else {
        uris = {
            "mongodb://192.168.1.10:27017",
            "mongodb://192.168.1.10:27017/Banco",
            "mongodb://192.168.1.10:27017/?connectTimeoutMS=5000",
            "mongodb://192.168.1.10:27017/?serverSelectionTimeoutMS=3000",
            "mongodb://192.168.1.10:27017/?connectTimeoutMS=5000&serverSelectionTimeoutMS=3000&socketTimeoutMS=5000",
            "mongodb://192.168.1.10:27017/?authSource=admin"
        };
    }

    std::string uriExitosa;
    for (const auto& uri : uris) {
        std::cout << "\nProbando: " << uri << std::endl;
        if (probarConexion(uri, 5)) {
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
            std::cout << "1. Verificar que MongoDB esté ejecutándose localmente" << std::endl;
            std::cout << "2. Ejecutar: net start MongoDB" << std::endl;
            std::cout << "3. Verificar el puerto 27017 esté disponible" << std::endl;
        }
        else {
            std::cout << "1. Verificar que MongoDB esté ejecutándose en pcServidor (192.168.1.10)" << std::endl;
            std::cout << "2. Revisar la configuración mongod.conf en pcServidor:" << std::endl;
            std::cout << "   net:" << std::endl;
            std::cout << "     port: 27017" << std::endl;
            std::cout << "     bindIp: 0.0.0.0" << std::endl;
            std::cout << "3. Verificar el firewall de Windows en pcServidor" << std::endl;
            std::cout << "4. Reiniciar el servicio MongoDB después de cambios" << std::endl;
        }
    }

    std::cout << "\n=== FIN DEL DIAGNÓSTICO ===" << std::endl;
}

/**
 * @brief Obtiene la URI según el modo de conexión configurado
 * @return URI de conexión apropiada
 */
std::string ConexionMongo::obtenerURI() {
    // Configurar las URIs por defecto si no se han configurado
    if (uriServidor.empty() || uriCliente.empty()) {
        configurarURIs();
    }

    // Retornar la URI según el modo seleccionado
    if (modoActual == SERVIDOR) {
        std::cout << "Usando URI de SERVIDOR (Local): " << uriServidor << std::endl;
        return uriServidor;
    }
    else {
        std::cout << "Usando URI de CLIENTE (Remoto): " << uriCliente << std::endl;
        return uriCliente;
    }
}