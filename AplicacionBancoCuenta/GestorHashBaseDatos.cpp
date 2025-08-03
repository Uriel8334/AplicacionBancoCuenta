#include "GestorHashBaseDatos.h"
#include "Utilidades.h"
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/types.hpp>
#include <filesystem>
#include <chrono>
#include <shlobj.h>
#include <iomanip>
#include <algorithm>
#include <limits>
#include <iostream>
#include <fstream>
#include <sstream>

// === IMPLEMENTACIÓN DE CalculadorHashSHA1 ===

std::string CalculadorHashSHA1::calcularHashDeCadena(const std::string& datos) {
    try {
        std::string archivoTemporal = obtenerRutaArchivoTemporal();

        std::ofstream temp(archivoTemporal);
        if (!temp.is_open()) {
            throw std::runtime_error("No se pudo crear archivo temporal para hash");
        }

        temp << datos;
        temp.close();

        std::string hash = Utilidades::calcularSHA1(archivoTemporal);

        // Limpiar archivo temporal
        std::filesystem::remove(archivoTemporal);

        return hash;
    }
    catch (const std::exception& e) {
        std::cerr << "Error al calcular hash SHA1: " << e.what() << std::endl;
        return "";
    }
}

std::string CalculadorHashSHA1::obtenerRutaArchivoTemporal() {
    return std::filesystem::temp_directory_path().string() + "\\banco_temp_hash.tmp";
}

// === IMPLEMENTACIÓN DE ProveedorDatosMongoDB ===

std::string ProveedorDatosMongoDB::obtenerDatosCompletosBaseDatos() {
    std::ostringstream datosCompletos;

    // Recopilar datos de todas las colecciones usando programación funcional
    auto colecciones = std::vector<std::string>{ "personas", "registros", "secuenciales" };

    std::for_each(colecciones.begin(), colecciones.end(), [&](const std::string& coleccion) {
        datosCompletos << obtenerDatosColeccion(coleccion);
        });

    return datosCompletos.str();
}

std::string ProveedorDatosMongoDB::obtenerDatosColeccion(const std::string& nombreColeccion) {
    try {
        auto& cliente = conexion.obtenerClienteBaseDatos();
        auto db = cliente[NOMBRE_DB];
        auto coleccion = db[nombreColeccion];

        std::ostringstream datos;

        // Usar iterador funcional para procesar documentos
        auto cursor = coleccion.find({});
        std::for_each(cursor.begin(), cursor.end(), [&](const bsoncxx::document::view& doc) {
            datos << bsoncxx::to_json(doc);
            });

        return datos.str();
    }
    catch (const std::exception& e) {
        std::cerr << "Error al obtener datos de colección " << nombreColeccion << ": " << e.what() << std::endl;
        return "";
    }
}

// === IMPLEMENTACIÓN DE PersistenciaHashImpl ===

std::string PersistenciaHashImpl::guardarHashEnArchivo(const std::string& hash) {
    try {
        std::string rutaArchivo = obtenerRutaArchivoHash();
        std::ofstream archivo(rutaArchivo);

        if (!archivo.is_open()) {
            throw std::runtime_error("No se pudo crear el archivo de hash");
        }

        archivo << hash;
        archivo.close();

        std::cout << "Hash guardado exitosamente en: " << rutaArchivo << std::endl;
        return rutaArchivo;
    }
    catch (const std::exception& e) {
        std::cerr << "Error al guardar hash en archivo: " << e.what() << std::endl;
        return "";
    }
}

std::string PersistenciaHashImpl::leerHashDeArchivo(const std::string& rutaArchivo) {
    return Utilidades::leerHashArchivo(rutaArchivo);
}

bool PersistenciaHashImpl::enviarHashABaseDatos(const std::string& hashCalculado, const std::string& hashRecibido) {
    try {
        auto& cliente = conexion.obtenerClienteBaseDatos();
        auto db = cliente[NOMBRE_DB];
        auto coleccion = db[COLECCION_CIFRADO];

        auto fechaActual = std::chrono::system_clock::now();

        ValidadorHashImpl validador;
        std::string validez = validador.determinarValidez(hashCalculado, hashRecibido);

        auto documento = bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("hashCalculado", hashCalculado),
            bsoncxx::builder::basic::kvp("hashRecibido", hashRecibido.empty() ? hashCalculado : hashRecibido),
            bsoncxx::builder::basic::kvp("validez", validez),
            bsoncxx::builder::basic::kvp("fechaIngreso", bsoncxx::types::b_date{ fechaActual })
        );

        auto resultado = coleccion.insert_one(documento.view());

        if (resultado) {
            std::cout << "Hash enviado exitosamente a la base de datos." << std::endl;
            std::cout << "Estado de validez: " << validez << std::endl;
            return true;
        }

        return false;
    }
    catch (const std::exception& e) {
        std::cerr << "Error al enviar hash a base de datos: " << e.what() << std::endl;
        return false;
    }
}

std::string PersistenciaHashImpl::obtenerRutaArchivoHash() {
    char pathEscritorio[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_DESKTOPDIRECTORY, NULL, 0, pathEscritorio))) {
        std::string rutaCarpeta = std::string(pathEscritorio) + "\\BancoApp";
        std::filesystem::create_directories(rutaCarpeta);

        auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);
        std::tm tm;
        localtime_s(&tm, &t);

        std::ostringstream nombreArchivo;
        nombreArchivo << rutaCarpeta << "\\Hash_BaseDatos_Banco_"
            << std::setfill('0') << std::setw(2) << tm.tm_mday << "_"
            << std::setfill('0') << std::setw(2) << (tm.tm_mon + 1) << "_"
            << (tm.tm_year + 1900) << "_"
            << std::setfill('0') << std::setw(2) << tm.tm_hour << "_"
            << std::setfill('0') << std::setw(2) << tm.tm_min << "_"
            << std::setfill('0') << std::setw(2) << tm.tm_sec << ".hash";

        return nombreArchivo.str();
    }

    throw std::runtime_error("No se pudo obtener la ruta del escritorio");
}

// === IMPLEMENTACIÓN DE ValidadorHashImpl ===

bool ValidadorHashImpl::validarHash(const std::string& hashRecibido, const std::string& hashCalculado) {
    if (hashCalculado.empty()) {
        std::cerr << "Error: No se pudo calcular el hash actual de la base de datos." << std::endl;
        return false;
    }

    bool esValido = (hashCalculado == hashRecibido);

    // Reportar resultado en consola
    if (esValido) {
        std::cout << "✓ Hash verificado exitosamente." << std::endl;
        std::cout << "  • Hash esperado/recibido: " << hashRecibido << std::endl;
        std::cout << "  • Hash actual/calculado: " << hashCalculado << std::endl;
    }
    else {
        std::cout << "¡ADVERTENCIA! Hash no coincide." << std::endl;
        std::cout << "  • Hash esperado/recibido: " << hashRecibido << std::endl;
        std::cout << "  • Hash actual/calculado: " << hashCalculado << std::endl;
    }

    return esValido;
}

std::string ValidadorHashImpl::determinarValidez(const std::string& hashCalculado, const std::string& hashRecibido) {
    if (hashRecibido.empty() || hashCalculado == hashRecibido) {
        return "Válido";
    }
    return "No es acorde a lo calculado. Violación de seguridad!";
}

// === IMPLEMENTACIÓN DE ConsultorHistorialHash ===

std::vector<bsoncxx::document::value> ConsultorHistorialHash::obtenerHistorialHash() {
    std::vector<bsoncxx::document::value> historial;

    try {
        auto& cliente = conexion.obtenerClienteBaseDatos();
        auto db = cliente[NOMBRE_DB];
        auto coleccion = db[COLECCION_CIFRADO];

        // Obtener documentos ordenados por fecha (más recientes primero)
        mongocxx::options::find opciones;
        opciones.sort(bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("fechaIngreso", -1)
        ));

        auto cursor = coleccion.find({}, opciones);

        std::for_each(cursor.begin(), cursor.end(), [&](const bsoncxx::document::view& doc) {
            historial.emplace_back(bsoncxx::document::value(doc));
            });

    }
    catch (const std::exception& e) {
        std::cerr << "Error al obtener historial: " << e.what() << std::endl;
    }

    return historial;
}

void ConsultorHistorialHash::mostrarHistorial() {
    std::cout << "=== Historial de Hashes ===" << std::endl;
    std::cout << std::endl;

    auto historial = obtenerHistorialHash();

    if (historial.empty()) {
        std::cout << "No hay registros de hash en la base de datos." << std::endl;
        return;
    }

    int contador = 1;
    std::for_each(historial.begin(), historial.end(), [&](const bsoncxx::document::value& docValue) {
        auto view = docValue.view();

        std::cout << "Registro #" << contador++ << ":" << std::endl;
        std::cout << "  Hash Calculado: " << view["hashCalculado"].get_string().value << std::endl;
        std::cout << "  Hash Recibido:  " << view["hashRecibido"].get_string().value << std::endl;
        std::cout << "  Validez:        " << view["validez"].get_string().value << std::endl;

        if (view["fechaIngreso"]) {
            auto fecha = view["fechaIngreso"].get_date();
            auto time_t_fecha = std::chrono::system_clock::to_time_t(std::chrono::system_clock::time_point{ std::chrono::milliseconds{fecha.value.count()} });
            std::tm tm;
            if (localtime_s(&tm, &time_t_fecha) == 0) {
                std::cout << "  Fecha:          " << std::put_time(&tm, "%d/%m/%Y %H:%M:%S") << std::endl;
            }
            else {
                std::cerr << "Error al convertir la fecha." << std::endl;
            }
        }

        std::cout << std::endl;
        });
}

// === IMPLEMENTACIÓN DE GestorHashBaseDatos ===

GestorHashBaseDatos::GestorHashBaseDatos(
    std::unique_ptr<ICalculadorHash> calculador,
    std::unique_ptr<IProveedorDatos> proveedor,
    std::unique_ptr<IPersistenciaHash> persistencia,
    std::unique_ptr<IValidadorHash> validador,
    std::unique_ptr<ConsultorHistorialHash> consultor
) : calculadorHash(std::move(calculador)),
proveedorDatos(std::move(proveedor)),
persistenciaHash(std::move(persistencia)),
validadorHash(std::move(validador)),
consultorHistorial(std::move(consultor)) {
}

std::unique_ptr<GestorHashBaseDatos> GestorHashBaseDatos::crear(ConexionMongo& conexion) {
    return std::make_unique<GestorHashBaseDatos>(
        std::make_unique<CalculadorHashSHA1>(),
        std::make_unique<ProveedorDatosMongoDB>(conexion),
        std::make_unique<PersistenciaHashImpl>(conexion),
        std::make_unique<ValidadorHashImpl>(),
        std::make_unique<ConsultorHistorialHash>(conexion)
    );
}

std::string GestorHashBaseDatos::calcularHashBaseDatos() {
    try {
        std::string datosCompletos = proveedorDatos->obtenerDatosCompletosBaseDatos();
        return calculadorHash->calcularHashDeCadena(datosCompletos);
    }
    catch (const std::exception& e) {
        std::cerr << "Error al calcular hash de base de datos: " << e.what() << std::endl;
        return "";
    }
}

std::string GestorHashBaseDatos::guardarHashEnArchivo(const std::string& hash) {
    return persistenciaHash->guardarHashEnArchivo(hash);
}

bool GestorHashBaseDatos::enviarHashABaseDatos(const std::string& hashCalculado, const std::string& hashRecibido) {
    return persistenciaHash->enviarHashABaseDatos(hashCalculado, hashRecibido);
}

bool GestorHashBaseDatos::validarHashBaseDatos(const std::string& hashRecibido) {
    std::string hashActual = calcularHashBaseDatos();
    bool esValido = validadorHash->validarHash(hashRecibido, hashActual);

    // Actualizar estado en base de datos
    enviarHashABaseDatos(hashActual, hashRecibido);

    return esValido;
}

void GestorHashBaseDatos::mostrarHistorialHash() {
    consultorHistorial->mostrarHistorial();
}

// === IMPLEMENTACIÓN DE InterfazGestionHash ===

InterfazGestionHash::InterfazGestionHash(std::unique_ptr<GestorHashBaseDatos> gestor)
    : gestorHash(std::move(gestor)) {
}

void InterfazGestionHash::mostrarMenuPrincipal() {
    std::vector<std::string> opciones = {
        "Generar Hash de Base de Datos",
        "Validar Hash Existente",
        "Importar y Validar Hash",
        "Ver Historial de Hashes",
        "Volver al Menú Principal"
    };

    while (true) {
        Utilidades::limpiarPantallaPreservandoMarquesina(0);
        int seleccion = Utilidades::menuInteractivo("=== Gestión de Hash de Base de Datos ===", opciones, 0, 0);

        switch (seleccion) {
        case 0:
            ejecutarGenerarHash();
            break;
        case 1:
            ejecutarValidarHash();
            break;
        case 2:
            ejecutarImportarValidarHash();
            break;
        case 3:
            ejecutarVerHistorial();
            break;
        case 4:
        case -1:
            return;
        default:
            break;
        }
    }
}

void InterfazGestionHash::ejecutarGenerarHash() {
    Utilidades::limpiarPantallaPreservandoMarquesina(0);
    std::cout << "=== Generando Hash de Base de Datos ===" << std::endl;
    std::cout << std::endl;

    std::cout << "Calculando hash de las colecciones: personas, registros, secuenciales..." << std::endl;

    std::string hash = gestorHash->calcularHashBaseDatos();

    if (hash.empty()) {
        std::cout << "Error: No se pudo calcular el hash de la base de datos." << std::endl;
        pausarPantalla();
        return;
    }

    std::cout << "Hash calculado exitosamente: " << hash << std::endl;
    std::cout << std::endl;

    // Guardar en archivo
    std::string rutaArchivo = gestorHash->guardarHashEnArchivo(hash);

    if (!rutaArchivo.empty()) {
        std::cout << "Archivo hash creado en: " << rutaArchivo << std::endl;
    }

    // Enviar a base de datos
    if (gestorHash->enviarHashABaseDatos(hash)) {
        std::cout << "Hash registrado en la base de datos (colección: cifrado)." << std::endl;
    }

    pausarPantalla();
}

void InterfazGestionHash::ejecutarValidarHash() {
    Utilidades::limpiarPantallaPreservandoMarquesina(0);
    std::cout << "=== Validar Hash de Base de Datos ===" << std::endl;
    std::cout << std::endl;

    std::cout << "Ingrese el hash a validar: ";
    std::string hashAValidar;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::getline(std::cin, hashAValidar);

    if (hashAValidar.empty()) {
        std::cout << "Error: Hash vacío proporcionado." << std::endl;
        pausarPantalla();
        return;
    }

    std::cout << std::endl << "Validando hash contra base de datos actual..." << std::endl;

    bool esValido = gestorHash->validarHashBaseDatos(hashAValidar);

    std::cout << std::endl;
    std::cout << "Resultado: " << (esValido ? "VÁLIDO" : "INVÁLIDO") << std::endl;

    pausarPantalla();
}

void InterfazGestionHash::ejecutarImportarValidarHash() {
    Utilidades::limpiarPantallaPreservandoMarquesina(0);
    std::cout << "=== Importar y Validar Archivo Hash ===" << std::endl;
    std::cout << std::endl;

    std::cout << "Ingrese la ruta completa del archivo .hash: ";
    std::string rutaArchivo;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::getline(std::cin, rutaArchivo);

    if (rutaArchivo.empty()) {
        std::cout << "Error: Ruta de archivo vacía." << std::endl;
        pausarPantalla();
        return;
    }

    std::string hashImportado = Utilidades::leerHashArchivo(rutaArchivo);

    if (hashImportado.empty()) {
        std::cout << "Error: No se pudo leer el archivo hash o está vacío." << std::endl;
        pausarPantalla();
        return;
    }

    std::cout << "Hash importado exitosamente: " << hashImportado << std::endl;
    std::cout << "Validando contra base de datos actual..." << std::endl;

    bool esValido = gestorHash->validarHashBaseDatos(hashImportado);

    std::cout << std::endl;
    std::cout << "Resultado de validación: " << (esValido ? "VÁLIDO" : "INVÁLIDO") << std::endl;

    pausarPantalla();
}

void InterfazGestionHash::ejecutarVerHistorial() {
    Utilidades::limpiarPantallaPreservandoMarquesina(0);
    gestorHash->mostrarHistorialHash();
    pausarPantalla();
}

void InterfazGestionHash::pausarPantalla() {
    std::cout << std::endl << "Presione Enter para continuar...";
    std::cin.get();
}