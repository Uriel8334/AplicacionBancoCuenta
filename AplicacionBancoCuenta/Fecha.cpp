/**
 * @file Fecha.cpp
 * @brief Implementaci�n de la clase Fecha para manejo de fechas
 *
 * Esta clase proporciona funcionalidades para manejar fechas, verificar d�as laborables,
 * detectar manipulaciones en la fecha del sistema y formatear fechas.
 */
#include "Fecha.h"
#include <ctime>
#include <sstream>
#include <stdexcept>
#include <vector>

 /**
  * @brief Constructor por defecto
  *
  * Inicializa la fecha con la fecha actual del sistema
  */
Fecha::Fecha() {
    time_t t = time(0);
    tm now = {};
    localtime_s(&now, &t);
    dia = now.tm_mday;
    mes = now.tm_mon + 1;
    anio = now.tm_year + 1900;
    //corregirSiNoLaborable();
}

/**
 * @brief Constructor con par�metros
 *
 * @param d D�a del mes (1-31)
 * @param m Mes del a�o (1-12)
 * @param a A�o
 */
Fecha::Fecha(int d, int m, int a) : dia(d), mes(m), anio(a) {
    //corregirSiNoLaborable();
}

/**
 * @brief Constructor a partir de string formateado
 *
 * @param fechaFormateada Fecha en formato "DD/MM/AAAA"
 * @throws std::invalid_argument Si el formato de la fecha no es v�lido
 */
Fecha::Fecha(const std::string& fechaFormateada) {
    std::istringstream ss(fechaFormateada);
    char separador;
    if (!(ss >> dia >> separador >> mes >> separador >> anio) || separador != '/') {
        throw std::invalid_argument("Formato de fecha invalido. Use DD/MM/AAAA.");
    }
    //corregirSiNoLaborable();
}

/**
 * @brief Verifica si un a�o es bisiesto
 *
 * Un a�o es bisiesto si es divisible por 4 y no por 100, o si es divisible por 400
 *
 * @param a A�o a verificar
 * @return true si el a�o es bisiesto, false en caso contrario
 */
bool Fecha::esBisiesto(int a) const {
    return (a % 4 == 0 && (a % 100 != 0 || a % 400 == 0));
}

/**
 * @brief Verifica si una fecha cae en fin de semana (s�bado o domingo)
 *
 * @param d D�a del mes
 * @param m Mes del a�o
 * @param a A�o
 * @return true si es s�bado o domingo, false en caso contrario
 */
bool Fecha::esFinDeSemana(int d, int m, int a) const {
    tm tiempo = {};
    tiempo.tm_mday = d;
    tiempo.tm_mon = m - 1;
    tiempo.tm_year = a - 1900;
    mktime(&tiempo);
    return (tiempo.tm_wday == 0 || tiempo.tm_wday == 6); // domingo o sabado
}

/**
 * @brief Verifica si una fecha es un d�a feriado
 *
 * Comprueba contra una lista est�tica de feriados
 *
 * @param d D�a del mes
 * @param m Mes del a�o
 * @param a A�o
 * @return true si es un d�a feriado, false en caso contrario
 */
bool Fecha::esFeriado(int d, int m, int a) const {
    static const std::vector<std::pair<int, int>> feriados = {
        {1, 1}, {20, 2}, {29, 3}, {1, 5},
        {24, 5}, {10, 8}, {9, 10}, {2, 11},
        {3, 11}, {25, 12}
    };
    for (const auto& f : feriados) {
        if (d == f.first && m == f.second) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Ajusta la fecha actual si no es un d�a laborable
 *
 * Si la fecha cae en fin de semana o es feriado, se avanza hasta el siguiente d�a laborable
 */
void Fecha::corregirSiNoLaborable() {
    while (esFinDeSemana(dia, mes, anio) || esFeriado(dia, mes, anio)) {
        avanzarADiaLaborable();
    }
}

/**
 * @brief Avanza la fecha al siguiente d�a laborable
 *
 * Incrementa la fecha en un d�a y actualiza los valores de d�a, mes y a�o
 */
void Fecha::avanzarADiaLaborable() {
    tm tiempo = {};
    tiempo.tm_mday = dia + 1;
    tiempo.tm_mon = mes - 1;
    tiempo.tm_year = anio - 1900;
    mktime(&tiempo);
    dia = tiempo.tm_mday;
    mes = tiempo.tm_mon + 1;
    anio = tiempo.tm_year + 1900;
}

/**
 * @brief Obtiene la fecha en formato "DD/MM/AAAA"
 *
 * @return std::string Fecha formateada como "DD/MM/AAAA"
 */
std::string Fecha::obtenerFechaFormateada() const {
    std::ostringstream oss;
    if (dia < 10) oss << '0';
    oss << dia << '/';
    if (mes < 10) oss << '0';
    oss << mes << '/' << anio;
    return oss.str();
}

/**
 * @brief Verifica si la fecha del sistema difiere de la fecha almacenada
 *
 * Esta funci�n puede ser �til para detectar manipulaciones en la fecha del sistema
 *
 * @return true si la fecha del sistema parece manipulada, false en caso contrario
 */
bool Fecha::esFechaSistemaManipulada() const {
    time_t t = time(0); // Obtiene la fecha actual del sistema
    tm now = {}; // Estructura para almacenar la fecha actual
    localtime_s(&now, &t); // Convierte el tiempo a la estructura tm
    // Comparamos la fecha almacenada con la del sistema
    return (dia != now.tm_mday || mes != (now.tm_mon + 1) || anio != (now.tm_year + 1900)); // Verifica si hay discrepancias
}

/**
 * @brief Establece una nueva fecha
 *
 * @param d D�a del mes
 * @param m Mes del a�o
 * @param a A�o
 */
void Fecha::setFecha(int d, int m, int a) {
    dia = d;
    mes = m;
    anio = a;
    //corregirSiNoLaborable();
}

/**
 * @brief Obtiene el estado de posible manipulaci�n de la fecha
 *
 * @return true si la fecha del sistema parece manipulada, false en caso contrario
 */
bool Fecha::getEsFechaSistemaManipulada() const {
    return esFechaSistemaManipulada();
}

/**
 * @brief Obtiene informaci�n detallada sobre la fecha actual del sistema
 *
 * Incluye tanto la fecha del sistema como la fecha almacenada y una indicaci�n
 * de si la fecha del sistema podr�a estar manipulada
 *
 * @return std::string Informaci�n completa sobre la fecha actual
 */
std::string Fecha::obtenerFechaActual() const {
    time_t t = time(0);
    tm now = {};
    localtime_s(&now, &t);
    std::ostringstream oss;
    oss << "Fecha del sistema: "
        << (now.tm_mday < 10 ? "0" : "") << now.tm_mday << '/'
        << (now.tm_mon + 1 < 10 ? "0" : "") << (now.tm_mon + 1) << '/'
        << (now.tm_year + 1900)
        << (esFechaSistemaManipulada() ? " (posiblemente manipulada)" : " (correcta)")
        << "\nFecha almacenada (no manipulada): "
        << (dia < 10 ? "0" : "") << dia << '/'
        << (mes < 10 ? "0" : "") << mes << '/'
        << anio;

    return oss.str();
}

/**
 * @brief Convierte la fecha a una representaci�n de cadena
 *
 * @return std::string Fecha en formato "DD/MM/AAAA"
 */
std::string Fecha::toString() const
{
    return obtenerFechaFormateada();
}