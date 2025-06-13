#pragma once
#ifndef FECHA_H
#define FECHA_H

#include <string>
#include <vector>

class Fecha {
private:
    int dia;
    int mes;
    int anio;

public:
    // SETTERS
	void setDia(int d) { dia = d; }
	void setMes(int m) { mes = m; }
	void setAnio(int a) { anio = a; }

	// GETTERS
	int getDia() const { return dia; }
	int getMes() const { return mes; }
	int getAnio() const { return anio; }


    Fecha();
    Fecha(int d, int m, int a);
    Fecha(const std::string& fechaFormateada);
    bool esBisiesto(int a) const;
    bool esFinDeSemana(int d, int m, int a) const;
    bool esFeriado(int d, int m, int a) const;
    void corregirSiNoLaborable();
    void avanzarADiaLaborable();
    std::string obtenerFechaFormateada() const;
    bool esFechaSistemaManipulada() const;
    void setFecha(int d, int m, int a);
    bool getEsFechaSistemaManipulada() const;
	std::string obtenerFechaActual() const;
	std::string toString() const;
};
#endif // FECHA_H
