#pragma once
#ifndef FECHA_H
#define FECHA_H

#include <string>
#include <vector>

/**
 * @class Fecha
 * @brief Clase que representa y manipula fechas
 *
 * Esta clase permite crear, validar y manipular fechas, incluyendo
 * funcionalidades para determinar d�as laborables, feriados,
 * y formatear fechas para su presentaci�n.
 */
class Fecha {
private:
    /** @brief D�a del mes (1-31) */
    int dia;

    /** @brief Mes del a�o (1-12) */
    int mes;

    /** @brief A�o (formato de 4 d�gitos) */
    int anio;

public:
    /**
     * @brief Establece el d�a del mes
     * @param d D�a a establecer
     */
    void setDia(int d) { dia = d; }

    /**
     * @brief Establece el mes del a�o
     * @param m Mes a establecer
     */
    void setMes(int m) { mes = m; }

    /**
     * @brief Establece el a�o
     * @param a A�o a establecer
     */
    void setAnio(int a) { anio = a; }

    /**
     * @brief Obtiene el d�a del mes
     * @return D�a actual
     */
    int getDia() const { return dia; }

    /**
     * @brief Obtiene el mes del a�o
     * @return Mes actual
     */
    int getMes() const { return mes; }

    /**
     * @brief Obtiene el a�o
     * @return A�o actual
     */
    int getAnio() const { return anio; }

    /**
     * @brief Constructor por defecto
     *
     * Inicializa la fecha con la fecha actual del sistema
     */
    Fecha();

    /**
     * @brief Constructor con par�metros
     *
     * @param d D�a del mes
     * @param m Mes del a�o
     * @param a A�o
     */
    Fecha(int d, int m, int a);

    /**
     * @brief Constructor que recibe una fecha formateada como cadena
     *
     * @param fechaFormateada Cadena con formato de fecha (ej. "DD/MM/AAAA")
     */
    Fecha(const std::string& fechaFormateada);

    /**
     * @brief Determina si un a�o es bisiesto
     *
     * @param a A�o a verificar
     * @return true si el a�o es bisiesto, false en caso contrario
     */
    bool esBisiesto(int a) const;

    /**
     * @brief Determina si una fecha corresponde a un fin de semana (s�bado o domingo)
     *
     * @param d D�a del mes
     * @param m Mes del a�o
     * @param a A�o
     * @return true si la fecha es fin de semana, false en caso contrario
     */
    bool esFinDeSemana(int d, int m, int a) const;

    /**
     * @brief Determina si una fecha es un d�a feriado
     *
     * @param d D�a del mes
     * @param m Mes del a�o
     * @param a A�o
     * @return true si la fecha es un d�a feriado, false en caso contrario
     */
    bool esFeriado(int d, int m, int a) const;

    /**
     * @brief Corrige la fecha actual si no es un d�a laborable
     *
     * Ajusta la fecha al siguiente d�a laborable si la fecha actual
     * es un fin de semana o feriado
     */
    void corregirSiNoLaborable();

    /**
     * @brief Avanza la fecha hasta el siguiente d�a laborable
     *
     * Incrementa la fecha hasta encontrar un d�a que no sea
     * fin de semana ni feriado
     */
    void avanzarADiaLaborable();

    /**
     * @brief Obtiene la fecha actual en formato de cadena
     *
     * @return Cadena que representa la fecha en un formato espec�fico
     */
    std::string obtenerFechaFormateada() const;

    /**
     * @brief Verifica si la fecha del sistema ha sido manipulada
     *
     * @return true si se detecta que la fecha del sistema ha sido manipulada,
     *         false en caso contrario
     */
    bool esFechaSistemaManipulada() const;

    /**
     * @brief Establece una fecha completa
     *
     * @param d D�a del mes
     * @param m Mes del a�o
     * @param a A�o
     */
    void setFecha(int d, int m, int a);

    /**
     * @brief Verifica si la fecha del sistema ha sido manipulada
     *
     * @return true si se detecta que la fecha del sistema ha sido manipulada,
     *         false en caso contrario
     */
    bool getEsFechaSistemaManipulada() const;

    /**
     * @brief Obtiene la fecha actual del sistema
     *
     * @return Cadena que representa la fecha actual del sistema
     */
    std::string obtenerFechaActual() const;

    /**
     * @brief Convierte la fecha a una representaci�n de cadena
     *
     * @return Cadena que representa la fecha en formato DD/MM/AAAA
     */
    std::string toString() const;
};
#endif // FECHA_H