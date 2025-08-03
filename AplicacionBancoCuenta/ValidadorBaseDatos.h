#pragma once
#include "_BaseDatosPersona.h"
#include <memory>

/**
 * @class ValidadorBaseDatos
 * @brief Responsable únicamente de las validaciones contra la base de datos
 *
 * Aplicando SRP: Una sola responsabilidad - validar existencia de datos en MongoDB
 */
class ValidadorBaseDatos {
private:
    _BaseDatosPersona& baseDatos;

public:
    explicit ValidadorBaseDatos(_BaseDatosPersona& bd);

    /**
     * @brief Verifica si existen personas registradas en la base de datos
     * @return true si existen personas, false en caso contrario
     */
    bool tienePersonasRegistradas();

    /**
     * @brief Verifica si existen cuentas registradas en la base de datos
     * @return true si existen cuentas, false en caso contrario
     */
    bool tieneCuentasRegistradas();

    /**
     * @brief Obtiene estadísticas generales de la base de datos
     * @return Estructura con información de personas y cuentas
     */
    struct EstadisticasBaseDatos {
        long totalPersonas;
        long totalCuentas;
        bool tienePersonas;
        bool tieneCuentas;
    };

    EstadisticasBaseDatos obtenerEstadisticas();

    /**
     * @brief Muestra información detallada del estado de la base de datos
     */
    void mostrarEstadoBaseDatos();
};