#pragma once
#ifndef CUENTACORRIENTE_H
#define CUENTACORRIENTE_H

#include "Cuenta.h"
#include "Validar.h"
#include "Cifrado.h"
#include "Fecha.h"

/**
 * @class CuentaCorriente
 * @brief Clase que representa una cuenta corriente bancaria
 *
 * Esta clase hereda de la clase abstracta Cuenta<double> e implementa
 * las funcionalidades específicas de una cuenta corriente, incluyendo
 * la verificación de monto mínimo y operaciones bancarias básicas.
 */
class CuentaCorriente : public Cuenta<double> {
private:
    /** @brief Monto mínimo requerido para mantener la cuenta (en dólares) */
    double montoMinimo; // monto minimo sino es $250.00, no se puede tener una CuentaCorriente / monto minimo en dolares

    /** @brief Fecha de apertura de la cuenta */
    Fecha fechaApertura;

public:
    /**
     * @brief Constructor por defecto
     *
     * Inicializa una cuenta corriente con valores predeterminados
     */
    CuentaCorriente() : Cuenta<double>(), montoMinimo(0.00) {}

    /**
     * @brief Constructor con parámetros
     *
     * @param numeroCuenta Número identificador de la cuenta
     * @param saldo Saldo inicial de la cuenta
     * @param fechaApertura Fecha de apertura en formato de cadena
     * @param estadoCuenta Estado inicial de la cuenta
     * @param montoMinimo Monto mínimo requerido para la cuenta
     */
    CuentaCorriente(std::string numeroCuenta, double saldo, const std::string& fechaApertura, const std::string estadoCuenta, double montoMinimo)
        : Cuenta<double>(numeroCuenta, saldo, fechaApertura, estadoCuenta), montoMinimo(montoMinimo) {}

    /**
     * @brief Establece el monto mínimo de la cuenta
     * @param montoMinimo Nuevo monto mínimo
     */
    void setMontoMinimo(double montoMinimo) { this->montoMinimo = montoMinimo; }

    /**
     * @brief Establece el número de cuenta
     * @param numeroCuenta Nuevo número de cuenta
     */
    void setNumeroCuenta(std::string numeroCuenta) { this->numeroCuenta = numeroCuenta; }

    /**
     * @brief Establece el saldo de la cuenta
     * @param saldo Nuevo saldo
     */
    void setSaldo(double saldo) { this->saldo = saldo; }

    /**
     * @brief Establece la fecha de apertura de la cuenta
     * @param fecha Nueva fecha en formato de cadena
     */
    void setFechaApertura(const std::string& fecha) { this->fechaApertura = fecha; }

    /**
     * @brief Establece el estado de la cuenta
     * @param estadoCuenta Nuevo estado
     */
    void setEstadoCuenta(const std::string& estadoCuenta) { this->estadoCuenta = estadoCuenta; }

    /**
     * @brief Devuelve una referencia a esta cuenta
     * @param cuentaCorriente Puntero a una cuenta corriente (no utilizado)
     * @return Puntero a esta cuenta
     */
    CuentaCorriente* setCuentaCorriente(CuentaCorriente* cuentaCorriente) { return this; }

    /**
     * @brief Establece la siguiente cuenta en la lista enlazada
     * @param cuentaSiguiente Puntero a la siguiente cuenta
     * @return Puntero a esta cuenta
     */
    CuentaCorriente* setSiguiente(CuentaCorriente* cuentaSiguiente) { this->siguiente = siguiente; return this; }

    /**
     * @brief Establece la cuenta anterior en la lista enlazada
     * @param cuentaAnterior Puntero a la cuenta anterior
     * @return Puntero a esta cuenta
     */
    CuentaCorriente* setAnterior(CuentaCorriente* cuentaAnterior) { this->anterior = anterior; return this; }

    /**
     * @brief Obtiene el monto mínimo de la cuenta
     * @return Monto mínimo actual
     */
    double getMontoMinimo() const { return this->montoMinimo; }

    /**
     * @brief Obtiene el número de cuenta
     * @return Número de cuenta como cadena
     */
    std::string getNumeroCuenta() const { return this->numeroCuenta; }

    /**
     * @brief Obtiene el saldo actual de la cuenta
     * @return Saldo de la cuenta
     */
    double getSaldo() const { return this->saldo; }

    /**
     * @brief Obtiene la fecha de apertura
     * @return Fecha de apertura como cadena formateada
     */
    std::string getFechaApertura() const { return this->fechaApertura.toString(); }

    /**
     * @brief Obtiene el estado actual de la cuenta
     * @return Estado de la cuenta como cadena
     */
    std::string getEstadoCuenta() const { return this->estadoCuenta; }

    /**
     * @brief Obtiene una referencia a esta cuenta
     * @return Puntero a esta cuenta
     */
    CuentaCorriente* getCuentaCorriente() { return this; }

    /**
     * @brief Obtiene la siguiente cuenta en la lista enlazada
     * @return Puntero a la siguiente cuenta, o nullptr si no existe
     */
    CuentaCorriente* getSiguiente() const {
        Cuenta<double>* sig = this->siguiente;
        return sig ? dynamic_cast<CuentaCorriente*>(sig) : nullptr;
    }

    /**
     * @brief Obtiene la cuenta anterior en la lista enlazada
     * @return Puntero a la cuenta anterior, o nullptr si no existe
     */
    CuentaCorriente* getAnterior() const { return dynamic_cast<CuentaCorriente*>(this->anterior); }

    /**
     * @brief Realiza un depósito en la cuenta
     * @param cantidad Monto a depositar
     */
    void depositar(double cantidad);

    /**
     * @brief Realiza un retiro de la cuenta
     * @param cantidad Monto a retirar
     */
    void retirar(double cantidad);

    /**
     * @brief Consulta el saldo disponible
     * @return Saldo actual de la cuenta
     */
    double consultarSaldo() const;

    /**
     * @brief Consulta el estado actual de la cuenta
     * @return Estado de la cuenta como cadena
     */
    std::string consultarEstado() const;

    /**
     * @brief Formatea el saldo para presentación
     * @return Saldo formateado como cadena
     */
    std::string formatearSaldo() const;

    /**
     * @brief Formatea un número con separador de miles (comas)
     * @param saldo Valor a formatear
     * @return Cadena con el valor formateado
     */
    std::string formatearConComas(double saldo) const;

    /**
     * @brief Muestra la información detallada de la cuenta
     * @param cedula Cédula del titular
     * @param limpiarPantalla Indica si debe limpiarse la pantalla antes de mostrar la información
     */
    void mostrarInformacion(const std::string& cedula, bool limpiarPantalla) const;

    /**
     * @brief Guarda los datos de la cuenta en un archivo
     * @param nombreArchivo Nombre del archivo donde guardar los datos
     */
    void guardarEnArchivo(const std::string& nombreArchivo) const override;

    /**
     * @brief Carga los datos de la cuenta desde un archivo
     * @param nombreArchivo Nombre del archivo desde donde cargar los datos
     */
    void cargarDesdeArchivo(const std::string& nombreArchivo) override;

    /**
     * @brief Verifica si el saldo cumple con el requisito de monto mínimo
     * @param montoMinimo Valor del monto mínimo a verificar
     */
    void esMontoMinimo(double montoMinimo);

};
#endif // CUENTACORRIENTE_H