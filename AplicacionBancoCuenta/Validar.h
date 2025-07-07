#pragma once
// Validar.h
#ifndef VALIDAR_H
#define VALIDAR_H

// Validar.h de cabecera de la clase Validar
#include <iostream>
#include <string>
#include <regex>
#include <stdexcept>
#include <Windows.h>

/**
 * @enum TipoEntrada
 * @brief Enumeración que define los tipos de entrada para validación
 *
 * Define los distintos tipos de datos que pueden ser validados
 * por las funciones de la clase Validar.
 */
enum class TipoEntrada {
    /** @brief Entrada numérica para cédulas de identidad */
    NUMERICO_CEDULA,
    /** @brief Entrada numérica para números de cuenta */
    NUMERICO_CUENTA,
    /** @brief Entrada que solo acepta caracteres alfabéticos */
    ALFABETICO,
    /** @brief Entrada que acepta caracteres alfanuméricos */
    ALFANUMERICO,
    /** @brief Entrada para direcciones de correo electrónico */
    CORREO,
    /** @brief Entrada para números telefónicos */
    TELEFONO
};

/**
 * @class Validar
 * @brief Clase estática que proporciona métodos de validación para datos bancarios
 *
 * Esta clase contiene métodos estáticos para validar diferentes tipos de datos
 * utilizados en un sistema bancario, como números de cuenta, cédulas, fechas,
 * montos, etc. Se organiza en regiones según el tipo de validación.
 */
class Validar {
public:
#pragma region VALIDACIONES PARA CUENTA
    /**
     * @brief Valida que el número de cuenta tenga un formato válido
     * @param numero Número de cuenta a validar
     * @return true si el número de cuenta es válido, false en caso contrario
     */
    static bool ValidarNumeroCuenta(const std::string numero);

    /**
     * @brief Valida que el saldo tenga un formato válido
     * @param saldoStr Saldo en formato de cadena
     * @return true si el formato del saldo es válido, false en caso contrario
     */
    static bool ValidarSaldo(const std::string& saldoStr);

    /**
     * @brief Valida que el estado de la cuenta sea válido
     * @param estado Estado de la cuenta a validar
     * @return true si el estado es válido, false en caso contrario
     */
    static bool ValidarEstadoCuenta(const std::string& estado);

    /**
     * @brief Valida que el tipo de cuenta sea válido
     * @param tipo Tipo de cuenta ("ahorros" o "corriente")
     * @return true si el tipo de cuenta es válido, false en caso contrario
     */
    static bool ValidarTipoCuenta(const std::string& tipo);

    /**
     * @brief Valida que el tipo de transacción sea válido
     * @param tipo Tipo de transacción ("retiro" o "deposito")
     * @return true si el tipo de transacción es válido, false en caso contrario
     */
    static bool ValidarTipoTransaccion(const std::string& tipo);

    /**
     * @brief Valida que el monto de la transacción sea válido
     * @param monto Valor entero del monto de la transacción
     * @return true si el monto es válido, false en caso contrario
     */
    static bool ValidarMontoTransaccion(int monto);
#pragma endregion

    /**
     * @brief Valida que el formato de la fecha sea correcto
     * @param fecha Fecha en formato de cadena (DD/MM/AAAA)
     * @return true si el formato de fecha es válido, false en caso contrario
     */
    static bool ValidarFecha(const std::string& fecha);

    /**
     * @brief Valida que un número tenga exactamente dos decimales
     * @param monto Valor numérico en formato de cadena
     * @return true si el número tiene formato correcto con dos decimales, false en caso contrario
     */
    static bool ValidarNumeroConDosDecimales(const std::string& monto);

    /**
     * @brief Lee un texto desde el portapapeles del sistema
     * @return Cadena con el contenido del portapapeles
     */
    static std::string leerDesdePortapapeles();

#pragma region VALIDACIONES PARA PERSONA
    /**
     * @brief Valida que el nombre de una persona sea válido
     * @param nombre Nombre a validar
     * @return true si el nombre es válido (solo letras y espacios), false en caso contrario
     */
    static bool ValidarNombrePersona(const std::string& nombre);

    /**
     * @brief Valida que una cédula de identidad tenga formato válido
     * @param cedula Número de cédula a validar
     * @return true si la cédula es válida, false en caso contrario
     */
    static bool ValidarCedula(const std::string& cedula);

    /**
     * @brief Verifica si todos los dígitos de una cédula son iguales
     * @param cedula Número de cédula a verificar
     * @return true si todos los dígitos son iguales, false en caso contrario
     */
    static bool todosLosDigitosIguales(const std::string& cedula);

    /**
     * @brief Valida la tecla ingresada según el tipo de entrada esperado
     * @param tecla Carácter ingresado
     * @param tipo Tipo de entrada esperada según enumeración TipoEntrada
     * @return true si la tecla es válida para el tipo de entrada, false en caso contrario
     */
    static bool ValidarTecla(char tecla, TipoEntrada tipo);

    /**
     * @brief Valida y lee un número de cuenta desde la entrada estándar
     * @return Número de cuenta validado
     */
    static std::string ValidarLeerNumeroCuenta();
#pragma endregion

    /**
     * @brief Verifica si un archivo existe en el sistema
     * @param rutaArchivo Ruta completa del archivo
     * @return true si el archivo existe, false en caso contrario
     */
    static bool archivoExiste(const std::string& rutaArchivo);

    /**
     * @brief Genera un nombre único para archivos de respaldo
     * @param rutaDirectorio Directorio donde se guardará el archivo
     * @param fechaFormateada Fecha formateada para incluir en el nombre
     * @return Nombre único para el archivo de respaldo
     */
    static std::string generarNombreConIndice(const std::string& rutaDirectorio, const std::string& fechaFormateada);
};
#endif // VALIDAR_H