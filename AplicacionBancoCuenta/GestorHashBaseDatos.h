#pragma once
#ifndef GESTORHASHBASEDATOS_H
#define GESTORHASHBASEDATOS_H

#include "ConexionMongo.h"
#include <mongocxx/client.hpp>
#include <bsoncxx/document/value.hpp>
#include <string>
#include <vector>

// === INTERFACES (ISP - Interface Segregation Principle) ===

/**
 * @brief Interfaz para el cálculo de hash
 *
 * Separación de responsabilidades: Solo se encarga del cálculo de hash
 */
class ICalculadorHash {
public:
    virtual ~ICalculadorHash() = default;
    virtual std::string calcularHashDeCadena(const std::string& datos) = 0;
};

/**
 * @brief Interfaz para acceso a datos de la base de datos
 *
 * Separación de responsabilidades: Solo se encarga de obtener datos
 */
class IProveedorDatos {
public:
    virtual ~IProveedorDatos() = default;
    virtual std::string obtenerDatosCompletosBaseDatos() = 0;
    virtual std::string obtenerDatosColeccion(const std::string& nombreColeccion) = 0;
};

/**
 * @brief Interfaz para persistencia de hash
 *
 * Separación de responsabilidades: Solo se encarga de guardar/leer hash
 */
class IPersistenciaHash {
public:
    virtual ~IPersistenciaHash() = default;
    virtual std::string guardarHashEnArchivo(const std::string& hash) = 0;
    virtual std::string leerHashDeArchivo(const std::string& rutaArchivo) = 0;
    virtual bool enviarHashABaseDatos(const std::string& hashCalculado, const std::string& hashRecibido = "") = 0;
};

/**
 * @brief Interfaz para validación de hash
 *
 * Separación de responsabilidades: Solo se encarga de validar hash
 */
class IValidadorHash {
public:
    virtual ~IValidadorHash() = default;
    virtual bool validarHash(const std::string& hashRecibido, const std::string& hashCalculado) = 0;
    virtual std::string determinarValidez(const std::string& hashCalculado, const std::string& hashRecibido) = 0;
};

// === IMPLEMENTACIONES CONCRETAS ===

/**
 * @brief Calculadora de hash SHA1 usando utilidades existentes
 *
 * SRP: Una sola responsabilidad - calcular hash
 */
class CalculadorHashSHA1 : public ICalculadorHash {
public:
    std::string calcularHashDeCadena(const std::string& datos) override;

private:
    std::string obtenerRutaArchivoTemporal();
};

/**
 * @brief Proveedor de datos desde MongoDB
 *
 * SRP: Una sola responsabilidad - obtener datos de MongoDB
 * DIP: Depende de la abstracción ConexionMongo
 */
class ProveedorDatosMongoDB : public IProveedorDatos {
private:
    ConexionMongo& conexion;
    static constexpr const char* NOMBRE_DB = "Banco";

public:
    explicit ProveedorDatosMongoDB(ConexionMongo& conn) : conexion(conn) {}

    std::string obtenerDatosCompletosBaseDatos() override;
    std::string obtenerDatosColeccion(const std::string& nombreColeccion) override;
};

/**
 * @brief Gestor de persistencia de hash (archivos y MongoDB)
 *
 * SRP: Una sola responsabilidad - persistir hash
 * DIP: Depende de abstracciones
 */
class PersistenciaHashImpl : public IPersistenciaHash {
private:
    ConexionMongo& conexion;
    static constexpr const char* NOMBRE_DB = "Banco";
    static constexpr const char* COLECCION_CIFRADO = "cifrado";

public:
    explicit PersistenciaHashImpl(ConexionMongo& conn) : conexion(conn) {}

    std::string guardarHashEnArchivo(const std::string& hash) override;
    std::string leerHashDeArchivo(const std::string& rutaArchivo) override;
    bool enviarHashABaseDatos(const std::string& hashCalculado, const std::string& hashRecibido = "") override;

private:
    std::string obtenerRutaArchivoHash();
};

/**
 * @brief Validador de hash con lógica de negocio
 *
 * SRP: Una sola responsabilidad - validar hash
 */
class ValidadorHashImpl : public IValidadorHash {
public:
    bool validarHash(const std::string& hashRecibido, const std::string& hashCalculado) override;
    std::string determinarValidez(const std::string& hashCalculado, const std::string& hashRecibido) override;
};

/**
 * @brief Consultor de historial de hash
 *
 * SRP: Una sola responsabilidad - consultar historial
 */
class ConsultorHistorialHash {
private:
    ConexionMongo& conexion;
    static constexpr const char* NOMBRE_DB = "Banco";
    static constexpr const char* COLECCION_CIFRADO = "cifrado";

public:
    explicit ConsultorHistorialHash(ConexionMongo& conn) : conexion(conn) {}

    std::vector<bsoncxx::document::value> obtenerHistorialHash();
    void mostrarHistorial();
};

/**
 * @brief Gestor principal de hash aplicando principios SOLID
 *
 * SRP: Coordina las operaciones de hash
 * OCP: Extensible sin modificar código existente
 * LSP: Utiliza interfaces consistentes
 * ISP: Interfaces específicas y segregadas
 * DIP: Depende de abstracciones, no de concreciones
 */
class GestorHashBaseDatos {
private:
    std::unique_ptr<ICalculadorHash> calculadorHash;
    std::unique_ptr<IProveedorDatos> proveedorDatos;
    std::unique_ptr<IPersistenciaHash> persistenciaHash;
    std::unique_ptr<IValidadorHash> validadorHash;
    std::unique_ptr<ConsultorHistorialHash> consultorHistorial;

public:
    // Constructor con inyección de dependencias (DIP)
    GestorHashBaseDatos(
        std::unique_ptr<ICalculadorHash> calculador,
        std::unique_ptr<IProveedorDatos> proveedor,
        std::unique_ptr<IPersistenciaHash> persistencia,
        std::unique_ptr<IValidadorHash> validador,
        std::unique_ptr<ConsultorHistorialHash> consultor
    );

    // Factory method para crear instancia con dependencias por defecto
    static std::unique_ptr<GestorHashBaseDatos> crear(ConexionMongo& conexion);

    // Operaciones principales
    std::string calcularHashBaseDatos();
    std::string guardarHashEnArchivo(const std::string& hash);
    bool enviarHashABaseDatos(const std::string& hashCalculado, const std::string& hashRecibido = "");
    bool validarHashBaseDatos(const std::string& hashRecibido);
    void mostrarHistorialHash();
};

/**
 * @brief Interfaz de usuario para gestión de hash
 *
 * SRP: Una sola responsabilidad - manejar interfaz de usuario
 * DIP: Depende de la abstracción GestorHashBaseDatos
 */
class InterfazGestionHash {
private:
    std::unique_ptr<GestorHashBaseDatos> gestorHash;

public:
    explicit InterfazGestionHash(std::unique_ptr<GestorHashBaseDatos> gestor);

    void mostrarMenuPrincipal();

private:
    void ejecutarGenerarHash();
    void ejecutarValidarHash();
    void ejecutarImportarValidarHash();
    void ejecutarVerHistorial();
    void pausarPantalla();
};

#endif // GESTORHASHBASEDATOS_H