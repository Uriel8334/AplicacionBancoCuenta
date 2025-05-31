#include "Fecha.h"
#include <ctime>
#include <sstream>

// Constructor por defecto: usa la fecha del sistema
Fecha::Fecha() {
	time_t t = time(0);
	tm now = {};
	localtime_s(&now, &t); // Uso seguro de localtime
	dia = now.tm_mday;
	mes = now.tm_mon + 1;
	anio = now.tm_year + 1900;
	corregirSiNoLaborable();
}

// Constructor con parametros
Fecha::Fecha(int d, int m, int a) : dia(d), mes(m), anio(a) {
	corregirSiNoLaborable();
}

// Establece la fecha
bool Fecha::esBisiesto(int a) const {
	return (a % 4 == 0 && a % 100 != 0) || (a % 400 == 0);
}

// Verifica si la fecha es valida segun el sistema operativo
bool Fecha::esFinDeSemana(int d, int m, int a) const {
	tm tiempo = {};
	tiempo.tm_mday = d;
	tiempo.tm_mon = m - 1;
	tiempo.tm_year = a - 1900;
	mktime(&tiempo);
	return tiempo.tm_wday == 0 || tiempo.tm_wday == 6; // domingo o sabado
}

// Verifica si la fecha es un feriado
bool Fecha::esFeriado(int d, int m, int a) const {
	// Lista de feriados (dia, mes) - ejemplo Ecuador
	static const std::vector<std::pair<int, int>> feriados = {
		{1, 1},		// Anio nuevo
		{20, 2},	// Carnaval
		{29, 3},	// Viernes Santo
		{1, 5},		// Dia del trabajo
		{24, 5},	// Batalla de Pichincha
		{10, 8},	// Primer grito de independencia
		{9, 10},	// Independencia de Guayaquil
		{2, 11},	// Dia de los muertos
		{3, 11},	// Independencia de Cuenca
		{25, 12},	// Navidad
	};
	for (const auto& f : feriados) {
		if (d == f.first && m == f.second) {
			return true;
		}
	}
	return false;
}

// Verifica si la fecha es un feriado
void Fecha::corregirSiNoLaborable() {
	while (esFinDeSemana(dia, mes, anio) || esFeriado(dia, mes, anio)) {
		avanzarADiaLaborable();
	}
}

// Avanza la fecha al siguiente dia laborable
void Fecha::avanzarADiaLaborable() {
	tm tiempo = {};
	tiempo.tm_mday = dia + 1;
	tiempo.tm_mon = mes - 1;
	tiempo.tm_year = anio - 1900;
	mktime(&tiempo);
	dia = tiempo.tm_mday;
	mes = tiempo.tm_mon + 1;
	anio = tiempo.tm_year + 1900;
	corregirSiNoLaborable();
}

// Devuelve la fecha formateada como "DD/MM/AAAA"
std::string Fecha::obtenerFechaFormateada() const {
	std::ostringstream oss;
	oss << (dia < 10 ? "0" : "") << dia << "/"
		<< (mes < 10 ? "0" : "") << mes << "/"
		<< anio;
	return oss.str();
}

// Verifica que la fecha no sea manipulada por el sistema
bool Fecha::esFechaSistemaManipulada() const {
	time_t t = time(0);
	tm now = {};
	localtime_s(&now, &t); // Uso seguro de localtime
	return (dia != now.tm_mday || mes != now.tm_mon + 1 || anio != now.tm_year + 1900);
}

// Establece la fecha
void Fecha::setFecha(int d, int m, int a) {
	dia = d;
	mes = m;
	anio = a;
	corregirSiNoLaborable();
}

bool Fecha::getEsFechaSistemaManipulada() const {
	return esFechaSistemaManipulada();
}