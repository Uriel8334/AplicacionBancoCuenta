#pragma once
#ifndef CUENTA_H
#define CUENTA_H

#include <iostream>
#include <string>
#include <fstream>
#include <regex> // Incluye la libreria regex para validacion de fecha
#include "Validar.h" // Incluye la clase de validacion
#include "Fecha.h" // Incluye la clase Fecha para manejar fechas

/**
 * @class Cuenta
 * @brief Clase abstracta que representa una cuenta bancaria genérica
 *
 * Esta clase implementa una estructura de lista doblemente enlazada y utiliza
 * templates para permitir diferentes tipos de datos para el saldo (T).
 * Sirve como base para diferentes tipos de cuentas bancarias.
 *
 * @tparam T Tipo de dato para almacenar el saldo (típicamente double o float)
 */
template <typename T>
class Cuenta {
protected:
    /** @brief Número identificador único de la cuenta */
    std::string numeroCuenta;

    /** @brief Saldo actual disponible en la cuenta */
    T saldo;

    /** @brief Fecha en que se abrió la cuenta */
    Fecha fechaApertura;

    /** @brief Estado actual de la cuenta (activa, inactiva, bloqueada, etc.) */
    std::string estadoCuenta;

    /** @brief Puntero al siguiente nodo en la lista doblemente enlazada */
    Cuenta<T>* siguiente;

    /** @brief Puntero al nodo anterior en la lista doblemente enlazada */
    Cuenta<T>* anterior;

    /**
     * @brief Constructor por defecto
     *
     * Inicializa una cuenta con valores vacíos y punteros nulos
     */
    Cuenta()
        : numeroCuenta(""), saldo(0), fechaApertura(), estadoCuenta(""), siguiente(nullptr), anterior(nullptr) {
    }

    /**
     * @brief Constructor con parámetros
     *
     * @param numeroCuenta Número identificador de la cuenta
     * @param saldo Saldo inicial de la cuenta
     * @param fechaStr Fecha de apertura en formato de cadena
     * @param estadoCuenta Estado inicial de la cuenta
     */
    Cuenta(std::string numeroCuenta, T saldo, const std::string& fechaStr, std::string estadoCuenta)
        : numeroCuenta(numeroCuenta), saldo(saldo), fechaApertura(fechaStr),
        estadoCuenta(estadoCuenta), siguiente(nullptr), anterior(nullptr) {
    }

    /**
     * @brief Destructor virtual
     *
     * Permite la correcta liberación de recursos en clases derivadas
     */
    virtual ~Cuenta() {}

public:
    /**
     * @brief Obtiene el número de cuenta
     * @return Número de cuenta como cadena
     */
    std::string getNumeroCuenta() const { return numeroCuenta; }

    /**
     * @brief Obtiene el saldo actual
     * @return Saldo de la cuenta
     */
    T getSaldo() const { return saldo; }

    /**
     * @brief Obtiene la fecha de apertura
     * @return Objeto Fecha con la fecha de apertura
     */
    Fecha getFechaApertura() const { return fechaApertura; }

    /**
     * @brief Obtiene el estado actual de la cuenta
     * @return Estado de la cuenta como cadena
     */
    std::string getEstadoCuenta() const { return estadoCuenta; }

    /**
     * @brief Obtiene el puntero al siguiente nodo
     * @return Puntero al siguiente nodo en la lista
     */
    Cuenta<T>* getSiguiente() const { return siguiente; }

    /**
     * @brief Obtiene el puntero al nodo anterior
     * @return Puntero al nodo anterior en la lista
     */
    Cuenta<T>* getAnterior() const { return anterior; }

    /**
     * @brief Establece el número de cuenta
     * @param numero Nuevo número de cuenta
     * @return Número de cuenta actualizado
     */
    std::string setNumeroCuenta(const std::string& numero) { numeroCuenta = numero; return numeroCuenta; }

    /**
     * @brief Establece el saldo de la cuenta
     * @param nuevoSaldo Nuevo saldo a establecer
     * @return Saldo actualizado
     */
    T setSaldo(T nuevoSaldo) { saldo = nuevoSaldo; return saldo; }

    /**
     * @brief Establece la fecha de apertura
     * @param fechaStr Fecha en formato de cadena
     * @return Objeto Fecha actualizado
     */
    Fecha setFechaApertura(const std::string& fechaStr) { fechaApertura = Fecha(fechaStr); return fechaApertura; }

    /**
     * @brief Establece el estado de la cuenta
     * @param estado Nuevo estado de la cuenta
     * @return Estado actualizado
     */
    std::string setEstadoCuenta(const std::string& estado) { estadoCuenta = estado; return estadoCuenta; }

    /**
     * @brief Establece el puntero al siguiente nodo
     * @param sig Puntero al siguiente nodo
     */
    void setSiguiente(Cuenta<T>* sig) { siguiente = sig; }

    /**
     * @brief Establece el puntero al nodo anterior
     * @param ant Puntero al nodo anterior
     */
    void setAnterior(Cuenta<T>* ant) { anterior = ant; }

    /**
     * @brief Realiza un depósito en la cuenta
     * @param cantidad Monto a depositar
     */
    virtual void depositar(T cantidad) = 0;

    /**
     * @brief Realiza un retiro de la cuenta
     * @param cantidad Monto a retirar
     */
    virtual void retirar(T cantidad) = 0;

    /**
     * @brief Consulta el saldo disponible
     * @return Saldo actual de la cuenta
     */
    virtual double consultarSaldo() const = 0;

    /**
     * @brief Consulta el estado actual de la cuenta
     * @return Estado de la cuenta como cadena
     */
    virtual std::string consultarEstado() const = 0;

    /**
     * @brief Guarda los datos de la cuenta en un archivo
     * @param nombreArchivo Nombre del archivo donde guardar los datos
     */
    virtual void guardarEnArchivo(const std::string& nombreArchivo) const = 0;

    /**
     * @brief Carga los datos de la cuenta desde un archivo
     * @param nombreArchivo Nombre del archivo desde donde cargar los datos
     */
    virtual void cargarDesdeArchivo(const std::string& nombreArchivo) = 0;

    /**
     * @brief Muestra la información de la cuenta
     * @param cedula Cédula del titular (opcional)
     * @param limpiarPantalla Indica si se debe limpiar la pantalla antes de mostrar (por defecto es true)
     */
    virtual void mostrarInformacion(const std::string& cedula = "", bool limpiarPantalla = true) const = 0;
};
#endif // CUENTA_H