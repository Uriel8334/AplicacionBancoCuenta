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
 * @brief Enumeraci�n que define los tipos de entrada para validaci�n
 *
 * Define los distintos tipos de datos que pueden ser validados
 * por las funciones de la clase Validar.
 */
enum class TipoEntrada {
    /** @brief Entrada num�rica para c�dulas de identidad */
    NUMERICO_CEDULA,
    /** @brief Entrada num�rica para n�meros de cuenta */
    NUMERICO_CUENTA,
    /** @brief Entrada que solo acepta caracteres alfab�ticos */
    ALFABETICO,
    /** @brief Entrada que acepta caracteres alfanum�ricos */
    ALFANUMERICO,
    /** @brief Entrada para direcciones de correo electr�nico */
    CORREO,
    /** @brief Entrada para n�meros telef�nicos */
    TELEFONO
};

/**
 * @class Validar
 * @brief Clase est�tica que proporciona m�todos de validaci�n para datos bancarios
 *
 * Esta clase contiene m�todos est�ticos para validar diferentes tipos de datos
 * utilizados en un sistema bancario, como n�meros de cuenta, c�dulas, fechas,
 * montos, etc. Se organiza en regiones seg�n el tipo de validaci�n.
 */
class Validar {
public:
#pragma region VALIDACIONES PARA CUENTA
    /**
     * @brief Valida que el n�mero de cuenta tenga un formato v�lido
     * @param numero N�mero de cuenta a validar
     * @return true si el n�mero de cuenta es v�lido, false en caso contrario
     */
    static bool ValidarNumeroCuenta(const std::string numero);

    /**
     * @brief Valida que el saldo tenga un formato v�lido
     * @param saldoStr Saldo en formato de cadena
     * @return true si el formato del saldo es v�lido, false en caso contrario
     */
    static bool ValidarSaldo(const std::string& saldoStr);

    /**
     * @brief Valida que el estado de la cuenta sea v�lido
     * @param estado Estado de la cuenta a validar
     * @return true si el estado es v�lido, false en caso contrario
     */
    static bool ValidarEstadoCuenta(const std::string& estado);

    /**
     * @brief Valida que el tipo de cuenta sea v�lido
     * @param tipo Tipo de cuenta ("ahorros" o "corriente")
     * @return true si el tipo de cuenta es v�lido, false en caso contrario
     */
    static bool ValidarTipoCuenta(const std::string& tipo);

    /**
     * @brief Valida que el tipo de transacci�n sea v�lido
     * @param tipo Tipo de transacci�n ("retiro" o "deposito")
     * @return true si el tipo de transacci�n es v�lido, false en caso contrario
     */
    static bool ValidarTipoTransaccion(const std::string& tipo);

    /**
     * @brief Valida que el monto de la transacci�n sea v�lido
     * @param monto Valor entero del monto de la transacci�n
     * @return true si el monto es v�lido, false en caso contrario
     */
    static bool ValidarMontoTransaccion(int monto);
#pragma endregion

    /**
     * @brief Valida que el formato de la fecha sea correcto
     * @param fecha Fecha en formato de cadena (DD/MM/AAAA)
     * @return true si el formato de fecha es v�lido, false en caso contrario
     */
    static bool ValidarFecha(const std::string& fecha);

    /**
     * @brief Valida que un n�mero tenga exactamente dos decimales
     * @param monto Valor num�rico en formato de cadena
     * @return true si el n�mero tiene formato correcto con dos decimales, false en caso contrario
     */
    static bool ValidarNumeroConDosDecimales(const std::string& monto);

    /**
     * @brief Lee un texto desde el portapapeles del sistema
     * @return Cadena con el contenido del portapapeles
     */
    static std::string leerDesdePortapapeles();

#pragma region VALIDACIONES PARA PERSONA
    /**
     * @brief Valida que el nombre de una persona sea v�lido
     * @param nombre Nombre a validar
     * @return true si el nombre es v�lido (solo letras y espacios), false en caso contrario
     */
    static bool ValidarNombrePersona(const std::string& nombre);

    /**
     * @brief Valida que una c�dula de identidad tenga formato v�lido
     * @param cedula N�mero de c�dula a validar
     * @return true si la c�dula es v�lida, false en caso contrario
     */
    static bool ValidarCedula(const std::string& cedula);

    /**
     * @brief Verifica si todos los d�gitos de una c�dula son iguales
     * @param cedula N�mero de c�dula a verificar
     * @return true si todos los d�gitos son iguales, false en caso contrario
     */
    static bool todosLosDigitosIguales(const std::string& cedula);

    /**
     * @brief Valida la tecla ingresada seg�n el tipo de entrada esperado
     * @param tecla Car�cter ingresado
     * @param tipo Tipo de entrada esperada seg�n enumeraci�n TipoEntrada
     * @return true si la tecla es v�lida para el tipo de entrada, false en caso contrario
     */
    static bool ValidarTecla(char tecla, TipoEntrada tipo);

    /**
     * @brief Valida y lee un n�mero de cuenta desde la entrada est�ndar
     * @return N�mero de cuenta validado
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
     * @brief Genera un nombre �nico para archivos de respaldo
     * @param rutaDirectorio Directorio donde se guardar� el archivo
     * @param fechaFormateada Fecha formateada para incluir en el nombre
     * @return Nombre �nico para el archivo de respaldo
     */
    static std::string generarNombreConIndice(const std::string& rutaDirectorio, const std::string& fechaFormateada);
};
#endif // VALIDAR_H