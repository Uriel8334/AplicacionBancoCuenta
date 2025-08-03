#pragma once
#include "_BaseDatosPersona.h"
#include "ManejoMenus.h"
#include "Utilidades.h"
#include <vector>
#include <string>
#include <functional>
#include <map>

/**
 * @class BuscadorCuentas
 * @brief Responsable únicamente de las operaciones de búsqueda de cuentas
 *
 * Aplicando SRP: Una sola responsabilidad - gestionar búsquedas de cuentas
 */
class BuscadorCuentas {
private:
    _BaseDatosPersona& baseDatos;

    std::map<int, std::function<void()>> mapaEstrategiasBusqueda;

    void inicializarEstrategias();

    // Estrategias específicas de búsqueda
    void buscarPorFechaCreacion();
    void buscarPorCriterioUsuario();
    void buscarPorCedula();

    // Métodos auxiliares para criterios de usuario
    void buscarPorNombre();
    void buscarPorApellido();
    void buscarPorFechaNacimiento();
    void buscarPorCorreo();
    void buscarPorDireccion();
    void buscarPorNumeroAhorros();
    void buscarPorNumeroCorrientes();
    void buscarPorTotalCuentas();

    // Métodos de presentación de resultados
    void mostrarResultadosFechas(const std::vector<bsoncxx::document::value>& resultados);
    void mostrarResultadosCriterio(const std::vector<bsoncxx::document::value>& resultados);
    void mostrarResultadoCedula(const bsoncxx::document::value& resultado);
    void mostrarPersonaCompleta(const bsoncxx::document::view& persona);
    void mostrarCuentasPersona(const bsoncxx::array::view& cuentas);

public:
    void buscarPorNumeroCuenta();
    explicit BuscadorCuentas(_BaseDatosPersona& bd);
    void ejecutarBusqueda(int tipoBusqueda);
};