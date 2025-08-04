#pragma once
#ifndef PERSONADATAPROCESSOR_H
#define PERSONADATAPROCESSOR_H

#include <string>
#include <functional>
#include <vector>
#include <memory>

class Persona;
class CuentaAhorros;
class CuentaCorriente;

/**
 * @brief Interfaz para procesadores de datos específicos
 * Aplicando Strategy Pattern
 */
class IDataProcessor {
public:
    virtual ~IDataProcessor() = default;
    virtual void procesar(Persona* persona, const std::string& parametro = "") const = 0;
};

/**
 * @brief Procesador para búsquedas en cuentas
 */
class BuscadorCuentasProcessor : public IDataProcessor {
public:
    void procesar(Persona* persona, const std::string& numeroCuenta = "") const override;
};

/**
 * @brief Procesador para mostrar cuentas
 */
class MostradorCuentasProcessor : public IDataProcessor {
public:
    void procesar(Persona* persona, const std::string& tipoCuenta = "") const override;
};

/**
 * @brief Clase responsable de procesar datos de persona
 * Aplicando Single Responsibility Principle y Command Pattern
 */
class PersonaDataProcessor {
private:
    std::vector<std::unique_ptr<IDataProcessor>> procesadores;

    // Funciones recursivas para manejo de listas enlazadas
    template<typename T>
    void procesarListaRecursivo(T* nodo, const std::function<void(T*)>& accion) const;

    template<typename T>
    int contarElementosRecursivo(T* nodo) const;

public:
    PersonaDataProcessor();
    void ejecutarProcesamiento(const std::string& tipo, Persona* persona, const std::string& parametro = "") const;

    // Métodos recursivos específicos
    void procesarCuentasAhorros(Persona* persona, const std::function<void(CuentaAhorros*)>& accion) const;
    void procesarCuentasCorriente(Persona* persona, const std::function<void(CuentaCorriente*)>& accion) const;

    int contarCuentasAhorros(Persona* persona) const;
    int contarCuentasCorriente(Persona* persona) const;
};

#endif // PERSONADATAPROCESSOR_H