/**
 * @file Fecha.cpp
 * @brief Implementación de la clase Fecha para manejo de fechas
 *
 * Esta clase proporciona funcionalidades para manejar fechas, verificar días laborables,
 * detectar manipulaciones en la fecha del sistema y formatear fechas.
 */
#include "Fecha.h"
#include <ctime>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <string>

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
 * @brief Constructor con parámetros
 *
 * @param d Día del mes (1-31)
 * @param m Mes del año (1-12)
 * @param a Año
 */
Fecha::Fecha(int d, int m, int a) : dia(d), mes(m), anio(a) {
	//corregirSiNoLaborable();
}

/**
 * @brief Constructor a partir de string formateado
 *
 * @param fechaFormateada Fecha en formato "DD/MM/AAAA"
 * @throws std::invalid_argument Si el formato de la fecha no es válido
 */
Fecha::Fecha(const std::string& fechaFormateada) {
	int d, m, a;
	try {

		if (sscanf_s(fechaFormateada.c_str(), "%d/%d/%d", &d, &m, &a) != 3) {
		//	std::ostringstream oss;
		//	oss << "Formato de fecha invalido. Use DD/MM/AAAA. Valor recibido: '" << fechaFormateada << "'";
		//	std::cerr << oss.str() << std::endl;
		//	d = m = a = 0; // Opcional: valores por defecto en caso de error
		}

		//std::cout << fechaFormateada << std::endl;
	}
	catch (const std::invalid_argument& ex) {
		std::cerr << "Excepción capturada: " << ex.what() << std::endl;
	}

	dia = d;
	mes = m;
	anio = a;

	//corregirSiNoLaborable();
}

/**
 * @brief Verifica si un año es bisiesto
 *
 * Un año es bisiesto si es divisible por 4 y no por 100, o si es divisible por 400
 *
 * @param a Año a verificar
 * @return true si el año es bisiesto, false en caso contrario
 */
bool Fecha::esBisiesto(int a) const {
	return (a % 4 == 0 && (a % 100 != 0 || a % 400 == 0));
}

/**
 * @brief Verifica si una fecha cae en fin de semana (sábado o domingo)
 *
 * @param d Día del mes
 * @param m Mes del año
 * @param a Año
 * @return true si es sábado o domingo, false en caso contrario
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
 * @brief Verifica si una fecha es un día feriado
 *
 * Comprueba contra una lista estática de feriados
 *
 * @param d Día del mes
 * @param m Mes del año
 * @param a Año
 * @return true si es un día feriado, false en caso contrario
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
 * @brief Ajusta la fecha actual si no es un día laborable
 *
 * Si la fecha cae en fin de semana o es feriado, se avanza hasta el siguiente día laborable
 */
void Fecha::corregirSiNoLaborable() {
	while (esFinDeSemana(dia, mes, anio) || esFeriado(dia, mes, anio)) {
		avanzarADiaLaborable();
	}
}

/**
 * @brief Avanza la fecha al siguiente día laborable
 *
 * Incrementa la fecha en un día y actualiza los valores de día, mes y año
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
 * Esta función puede ser útil para detectar manipulaciones en la fecha del sistema
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
 * @param d Día del mes
 * @param m Mes del año
 * @param a Año
 */
void Fecha::setFecha(int d, int m, int a) {
	dia = d;
	mes = m;
	anio = a;
	//corregirSiNoLaborable();
}

/**
 * @brief Obtiene el estado de posible manipulación de la fecha
 *
 * @return true si la fecha del sistema parece manipulada, false en caso contrario
 */
bool Fecha::getEsFechaSistemaManipulada() const {
	return esFechaSistemaManipulada();
}

/**
 * @brief Obtiene información detallada sobre la fecha actual del sistema
 *
 * Incluye tanto la fecha del sistema como la fecha almacenada y una indicación
 * de si la fecha del sistema podría estar manipulada
 *
 * @return std::string Información completa sobre la fecha actual
 */
std::string Fecha::obtenerFechaActual() const {
	time_t t = time(0);
	tm now = {};
	localtime_s(&now, &t);
	std::ostringstream oss;
	oss << "Fecha del sistema:"
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
 * @brief Convierte la fecha a una representación de cadena
 *
 * @return std::string Fecha en formato "DD/MM/AAAA"
 */
std::string Fecha::toString() const
{
	return obtenerFechaFormateada();
}