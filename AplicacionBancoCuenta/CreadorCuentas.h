#pragma once
#include "_BaseDatosPersona.h"
#include "Persona.h"
#include "CuentaAhorros.h"
#include "CuentaCorriente.h"
#include "PersonaUI.h"
#include <memory>
#include <utility>

/**
 * @class CreadorCuentas
 * @brief Responsable únicamente de la creación y configuración de cuentas
 *
 * Aplicando SRP: Una sola responsabilidad - crear cuentas con todos sus datos
 */
class CreadorCuentas {
private:
    _BaseDatosPersona& baseDatos;

public:
    explicit CreadorCuentas(_BaseDatosPersona& bd) : baseDatos(bd) {}

    /**
     * @brief Crea una cuenta de ahorros completa con persistencia
     */
    std::pair<bool, std::string> crearCuentaAhorros(
        Persona* persona,
        const std::string& cedula,
        CuentaAhorros* cuenta
    );

    /**
     * @brief Crea una cuenta corriente completa con persistencia
     */
    std::pair<bool, std::string> crearCuentaCorriente(
        Persona* persona,
        const std::string& cedula,
        CuentaCorriente* cuenta
    );

private:
    double solicitarMontoInicialAhorros();
    double solicitarMontoInicialCorriente();
    bsoncxx::document::value crearDocumentoCuenta(
        const std::string& numeroCuenta,
        const std::string& tipo,
        double saldo,
        const std::string& fecha,
        const std::string& sucursal
    );
    void configurarDatosBasicosCuenta(Cuenta<double>* cuenta);
    void finalizarConfiguracionCuenta(Cuenta<double>* cuenta, const std::string& numeroCuenta, double monto);
    void agregarCuentaAMemoria(Persona* persona, CuentaAhorros* cuenta);
    void agregarCuentaAMemoria(Persona* persona, CuentaCorriente* cuenta);
};