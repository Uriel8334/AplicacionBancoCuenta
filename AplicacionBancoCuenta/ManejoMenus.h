#pragma once

#include <string>
#include <vector>

class Persona;
class CuentaAhorros;
class CuentaCorriente;

/**
 * @class ManejoMenus
 * @brief Responsable únicamente de la interfaz de usuario y menús
 *
 * Aplicando SRP: Una sola responsabilidad - manejo de interfaz de usuario
 * Aplicando ISP: Interfaz específica para operaciones de UI
 */
class ManejoMenus
{
public:

	static int mostrarMenuTipoCuenta();

    static int mostrarMenuBusqueda();

    // Solicitud de datos
    static std::string solicitarCedula();
    static std::string solicitarNumeroCuenta(const std::string& tipo = "");
    static double solicitarMonto();
    static std::string seleccionarFecha();

    // Confirmaciones
    static bool confirmarOperacion(const std::string& mensaje);
    static bool confirmarAgregarCuentaExistente(const std::string& cedula, const std::string& nombre);

    // Mostrar resultados - Aplicando SRP
    static void mostrarResultadosBusqueda(const std::vector<std::pair<Persona*, void*>>& resultados);
    static void mostrarResultadoBusquedaPersona(Persona* persona);
    static void mostrarResultadoBusquedaCuenta(void* cuenta);

    static void mostrarMensajeError(const std::string& mensaje);
    static void mostrarMensajeExito(const std::string& mensaje);
};