#include "Fecha.h"
#include <ctime>
#include <sstream>
#include <stdexcept>
#include <vector>

// Constructor por defecto (usa la fecha del sistema)
Fecha::Fecha() {
    time_t t = time(0);
    tm now = {};
    localtime_s(&now, &t);
    dia = now.tm_mday;
    mes = now.tm_mon + 1;
    anio = now.tm_year + 1900;
    //corregirSiNoLaborable();
}

// Constructor con parámetros (día, mes, año)
Fecha::Fecha(int d, int m, int a) : dia(d), mes(m), anio(a) {
    //corregirSiNoLaborable();
}

// Constructor con fecha formateada "DD/MM/AAAA"
Fecha::Fecha(const std::string& fechaFormateada) {
    std::istringstream ss(fechaFormateada);
    char separador;
    if (!(ss >> dia >> separador >> mes >> separador >> anio) || separador != '/') {
        throw std::invalid_argument("Formato de fecha inválido. Use DD/MM/AAAA.");
    }
    //corregirSiNoLaborable();
}

// Verifica si el año es bisiesto
bool Fecha::esBisiesto(int a) const {
    return (a % 4 == 0 && (a % 100 != 0 || a % 400 == 0));
}

// Verifica si la fecha es fin de semana
bool Fecha::esFinDeSemana(int d, int m, int a) const {
    tm tiempo = {};
    tiempo.tm_mday = d;
    tiempo.tm_mon = m - 1;
    tiempo.tm_year = a - 1900;
    mktime(&tiempo);
    return (tiempo.tm_wday == 0 || tiempo.tm_wday == 6); // domingo o sábado
}

// Verifica si la fecha cae en un feriado
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

// Ajusta la fecha si no es laborable
void Fecha::corregirSiNoLaborable() {
    while (esFinDeSemana(dia, mes, anio) || esFeriado(dia, mes, anio)) {
        avanzarADiaLaborable();
    }
}

// Avanza hasta el siguiente día laborable
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

// Devuelve la fecha formateada "DD/MM/AAAA"
std::string Fecha::obtenerFechaFormateada() const {
    std::ostringstream oss;
    if (dia < 10) oss << '0';
    oss << dia << '/';
    if (mes < 10) oss << '0';
    oss << mes << '/' << anio;
    return oss.str();
}

// Verifica si la fecha del sistema difiere de la fecha real (indicio de manipulación)
bool Fecha::esFechaSistemaManipulada() const {
	time_t t = time(0); // Obtiene la fecha actual del sistema
	tm now = {}; // Estructura para almacenar la fecha actual
	localtime_s(&now, &t); // Convierte el tiempo a la estructura tm
	// Comparamos la fecha almacenada con la del sistema
	return (dia != now.tm_mday || mes != (now.tm_mon + 1) || anio != (now.tm_year + 1900)); // Verifica si hay discrepancias
}

// Establece la fecha y la corrige si es no laborable
void Fecha::setFecha(int d, int m, int a) {
    dia = d;
    mes = m;
    anio = a;
    //corregirSiNoLaborable();
}

// Devuelve si la fecha del sistema está (posiblemente) manipulada
bool Fecha::getEsFechaSistemaManipulada() const {
    return esFechaSistemaManipulada();
}

// Obtiene la fecha actual del sistema
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

std::string Fecha::toString() const
{
	return obtenerFechaFormateada();
}
