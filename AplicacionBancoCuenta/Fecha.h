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
    bool esFeriado(int d, int m, int a) const;
    bool esFinDeSemana(int d, int m, int a) const;
    bool esBisiesto(int a) const;
    bool esFechaSistemaManipulada() const;
    void corregirSiNoLaborable();

public:
    Fecha();
    Fecha(int d, int m, int a);
    void setFecha(int d, int m, int a);
    std::string obtenerFechaFormateada() const;
    void avanzarADiaLaborable();
	bool getEsFechaSistemaManipulada() const;

};

#endif // FECHA_H
