#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <chrono>
#include <iomanip>

/**
 * @class BancoManejaRegistro
 * @brief Clase para gestionar registros de operaciones bancarias
 */
class BancoManejaRegistro {
public:
    /**
     * @brief Constructor por defecto
     */
    BancoManejaRegistro();

    /**
     * @brief Agrega un registro de operación a la base de datos
     * @param tipoOperacion Tipo de operación realizada
     * @param cedula Número de cédula de la persona involucrada
     * @return true si se registró correctamente, false en caso contrario
     */
    bool agregarRegistroBaseDatos(const std::string& tipoOperacion, const std::string& cedula);

    /**
     * @brief Obtiene la fecha y hora actual en formato string
     * @return Fecha y hora formateada
     */
    std::string obtenerFechaHoraActual() const;

private:
    const std::string ARCHIVO_REGISTROS = "registros_operaciones.log";
};