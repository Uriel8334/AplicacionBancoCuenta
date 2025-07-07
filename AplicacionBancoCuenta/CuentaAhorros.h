#pragma once
#ifndef CUENTAAHORROS_H
#define CUENTAAHORROS_H

#include "Cuenta.h"
#include "Validar.h" // Include validation class
#include "Cifrado.h" // Include encryption class
#include "Fecha.h"   // Include date class
#include "Utilidades.h" // Include utilities class

/**
 * @class CuentaAhorros
 * @brief Clase que representa una cuenta de ahorros bancaria
 *
 * Esta clase hereda de la clase abstracta Cuenta<double> e implementa
 * las funcionalidades específicas de una cuenta de ahorros, incluyendo
 * el cálculo de intereses y operaciones básicas bancarias.
 */
class CuentaAhorros : public Cuenta<double> {
private:
    /** @brief Tasa de interés anual de la cuenta de ahorros */
    double tasaInteres;

    /** @brief Fecha de apertura de la cuenta */
    Fecha fechaApertura;

public:
    /**
     * @brief Constructor por defecto
     *
     * Inicializa una cuenta de ahorros con valores predeterminados
     */
    CuentaAhorros() : Cuenta<double>(), tasaInteres(0.0) {}

    /**
     * @brief Constructor con parámetros
     *
     * @param numCuenta Número identificador de la cuenta
     * @param saldo Saldo inicial de la cuenta
     * @param fecha Fecha de apertura en formato de cadena
     * @param estado Estado inicial de la cuenta
     * @param tasa Tasa de interés anual aplicable a la cuenta
     */
    CuentaAhorros(std::string numCuenta, double saldo, const std::string& fecha, const std::string& estado, double tasa)
        : Cuenta<double>(numCuenta, saldo, fecha, estado), tasaInteres(tasa) {}

    /**
     * @brief Destructor
     */
    ~CuentaAhorros() {}

    //CuentaAhorros() : Cuenta<int>(), tasaInteres(0) {}
    //CuentaAhorros(int numCuenta, int sal, std::string fecha, std::string estado, double tasa)
    //	: Cuenta<int>(numCuenta, sal, fecha, estado), tasaInteres(tasa) {}
    //CuentaAhorros(int numCuenta, int sal, std::string fecha,
    //	std::string estado, double tasa);

    /**
     * @brief Establece la tasa de interés de la cuenta
     * @param tast Nueva tasa de interés
     */
    void setTasaInteres(double tast) { this->tasaInteres = tast; }

    /**
     * @brief Establece el número de cuenta
     * @param numCuenta Nuevo número de cuenta
     */
    void setNumeroCuenta(std::string numCuenta) { this->numeroCuenta = numCuenta; }

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
     * @param estado Nuevo estado
     */
    void setEstadoCuenta(const std::string& estado) { this->estadoCuenta = estado; }

    /**
     * @brief Devuelve una referencia a esta cuenta
     * @param cuenta Puntero a una cuenta de ahorros (no utilizado)
     * @return Puntero a esta cuenta
     */
    CuentaAhorros* setCuentaAhorros(CuentaAhorros* cuenta) { return this; }

    /**
     * @brief Establece la siguiente cuenta en la lista enlazada
     * @param siguiente Puntero a la siguiente cuenta
     * @return Puntero a esta cuenta
     */
    CuentaAhorros* setSiguiente(CuentaAhorros* siguiente) { this->siguiente = siguiente; return this; }

    /**
     * @brief Establece la cuenta anterior en la lista enlazada
     * @param anterior Puntero a la cuenta anterior
     * @return Puntero a esta cuenta
     */
    CuentaAhorros* setAnterior(CuentaAhorros* anterior) { this->anterior = anterior; return this; }

    /**
     * @brief Establece la tasa de interés (sobrecargado)
     * @param tasa Nueva tasa de interés
     */
    void getTasaInteres(double tasa) { this->tasaInteres = tasa; }

    /**
     * @brief Obtiene el número de cuenta
     * @return Número de cuenta como cadena
     */
    std::string getNumeroCuenta() const { return this->numeroCuenta; }

    /**
     * @brief Obtiene la fecha de apertura
     * @return Fecha de apertura como cadena formateada
     */
    std::string getFechaApertura() const { return this->fechaApertura.toString(); }

    /**
     * @brief Obtiene el saldo actual de la cuenta
     * @return Saldo de la cuenta
     */
    double getSaldo() const { return this->saldo; }

    /**
     * @brief Obtiene el estado actual de la cuenta
     * @return Estado de la cuenta como cadena
     */
    std::string getEstadoCuenta() const { return this->estadoCuenta; }

    /**
     * @brief Obtiene una referencia a esta cuenta
     * @return Puntero a esta cuenta
     */
    CuentaAhorros* getCuentaAhorros() { return this; }

    /**
     * @brief Obtiene la siguiente cuenta en la lista enlazada
     * @return Puntero a la siguiente cuenta, o nullptr si no existe
     */
    CuentaAhorros* getSiguiente() const {
        Cuenta<double>* sig = this->siguiente;
        return sig ? dynamic_cast<CuentaAhorros*>(sig) : nullptr;
    }

    /**
     * @brief Obtiene la cuenta anterior en la lista enlazada
     * @return Puntero a la cuenta anterior, o nullptr si no existe
     */
    CuentaAhorros* getAnterior() const { return dynamic_cast<CuentaAhorros*>(this->anterior); }

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
     * @brief Calcula el interés generado por la cuenta
     * @return Interés calculado en base a la tasa y saldo actual
     */
    int calcularInteres() const;
};

#endif // CUENTAAHORROS_H