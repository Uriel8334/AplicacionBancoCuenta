#pragma once

#include "CuentaAhorros.h"
#include "CuentaCorriente.h"
#include "BancoManejoPersona.h"
#include "_BaseDatosPersona.h"
#include <string>
#include <vector>

/**
 * @class BancoManejoCuenta
 * @brief Responsable únicamente del manejo de cuentas bancarias
 *
 * Aplicando SRP: Una sola responsabilidad - gestionar operaciones de cuentas
 */
class BancoManejoCuenta {
private:
    BancoManejoPersona& manejoPersonas;

    // Métodos recursivos para búsquedas en cuentas
    template<typename TipoCuenta>
    TipoCuenta* buscarCuentaRecursivo(TipoCuenta* cuenta, const std::string& numeroCuenta);

    void buscarCuentasPorCriterioRecursivo(NodoPersona* nodo,
        const std::string& criterio,
        const std::string& valor,
        std::vector<std::pair<Persona*, void*>>& resultados);

public:
    explicit BancoManejoCuenta(BancoManejoPersona& manejadorPersonas);

    // Operaciones de búsqueda de cuentas
    std::pair<CuentaAhorros*, Persona*> buscarCuentaAhorros(const std::string& numeroCuenta);
    std::pair<CuentaCorriente*, Persona*> buscarCuentaCorriente(const std::string& numeroCuenta);

    // Búsquedas por criterios múltiples
    std::vector<std::pair<Persona*, void*>> buscarCuentasPorNumero(const std::string& numero);
    std::vector<std::pair<Persona*, void*>> buscarCuentasPorFecha(const std::string& fecha);
    std::vector<std::pair<Persona*, void*>> buscarCuentasPorSaldo(double saldoMinimo);
    std::vector<std::pair<Persona*, void*>> buscarCuentasPorCedula(const std::string& cedula);

    // Validaciones
    bool existeCuenta(const std::string& numeroCuenta);
    bool validarFondosSuficientes(const std::string& numeroCuenta, double monto);

    // Creación de cuentas
    bool crearCuentaAhorros(const std::string& cedula, CuentaAhorros* cuenta);
    bool crearCuentaCorriente(const std::string& cedula, CuentaCorriente* cuenta);

    // === MÉTODOS OPERACIONALES BANCARIOS ===

    /**
     * @brief Realiza un depósito en una cuenta específica
     * @param numeroCuenta Número de cuenta destino
     * @param monto Monto a depositar
     * @return true si el depósito fue exitoso, false en caso contrario
     */
    bool depositar(const std::string& numeroCuenta, double monto);

    /**
     * @brief Realiza un retiro de una cuenta específica
     * @param numeroCuenta Número de cuenta origen
     * @param monto Monto a retirar
     * @return true si el retiro fue exitoso, false en caso contrario
     */
    bool retirar(const std::string& numeroCuenta, double monto);

    /**
     * @brief Obtiene el saldo actual de una cuenta
     * @param numeroCuenta Número de cuenta a consultar
     * @return Saldo actual como double, -1.0 si la cuenta no existe
     */
    double consultarSaldo(const std::string& numeroCuenta);

    /**
     * @brief Realiza una transferencia entre dos cuentas
     * @param cuentaOrigen Número de cuenta origen
     * @param cuentaDestino Número de cuenta destino
     * @param monto Monto a transferir
     * @return true si la transferencia fue exitosa, false en caso contrario
     */
    bool transferir(const std::string& cuentaOrigen, const std::string& cuentaDestino, double monto);

    /**
     * @brief Obtiene información completa de una cuenta desde la base de datos
     * @param numeroCuenta Número de cuenta a consultar
     * @return Información de la cuenta en formato JSON
     */
    std::string obtenerInformacionCompleta(const std::string& numeroCuenta);
};