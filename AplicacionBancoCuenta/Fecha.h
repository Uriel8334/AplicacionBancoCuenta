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
 * funcionalidades para determinar días laborables, feriados,
 * y formatear fechas para su presentación.
 */
class Fecha {
private:
    /** @brief Día del mes (1-31) */
    int dia;

    /** @brief Mes del año (1-12) */
    int mes;

    /** @brief Año (formato de 4 dígitos) */
    int anio;

public:
    /**
     * @brief Establece el día del mes
     * @param d Día a establecer
     */
    void setDia(int d) { dia = d; }

    /**
     * @brief Establece el mes del año
     * @param m Mes a establecer
     */
    void setMes(int m) { mes = m; }

    /**
     * @brief Establece el año
     * @param a Año a establecer
     */
    void setAnio(int a) { anio = a; }

    /**
     * @brief Obtiene el día del mes
     * @return Día actual
     */
    int getDia() const { return dia; }

    /**
     * @brief Obtiene el mes del año
     * @return Mes actual
     */
    int getMes() const { return mes; }

    /**
     * @brief Obtiene el año
     * @return Año actual
     */
    int getAnio() const { return anio; }

    /**
     * @brief Constructor por defecto
     *
     * Inicializa la fecha con la fecha actual del sistema
     */
    Fecha();

    /**
     * @brief Constructor con parámetros
     *
     * @param d Día del mes
     * @param m Mes del año
     * @param a Año
     */
    Fecha(int d, int m, int a);

    /**
     * @brief Constructor que recibe una fecha formateada como cadena
     *
     * @param fechaFormateada Cadena con formato de fecha (ej. "DD/MM/AAAA")
     */
    Fecha(const std::string& fechaFormateada);

    /**
     * @brief Determina si un año es bisiesto
     *
     * @param a Año a verificar
     * @return true si el año es bisiesto, false en caso contrario
     */
    bool esBisiesto(int a) const;

    /**
     * @brief Determina si una fecha corresponde a un fin de semana (sábado o domingo)
     *
     * @param d Día del mes
     * @param m Mes del año
     * @param a Año
     * @return true si la fecha es fin de semana, false en caso contrario
     */
    bool esFinDeSemana(int d, int m, int a) const;

    /**
     * @brief Determina si una fecha es un día feriado
     *
     * @param d Día del mes
     * @param m Mes del año
     * @param a Año
     * @return true si la fecha es un día feriado, false en caso contrario
     */
    bool esFeriado(int d, int m, int a) const;

    /**
     * @brief Corrige la fecha actual si no es un día laborable
     *
     * Ajusta la fecha al siguiente día laborable si la fecha actual
     * es un fin de semana o feriado
     */
    void corregirSiNoLaborable();

    /**
     * @brief Avanza la fecha hasta el siguiente día laborable
     *
     * Incrementa la fecha hasta encontrar un día que no sea
     * fin de semana ni feriado
     */
    void avanzarADiaLaborable();

    /**
     * @brief Obtiene la fecha actual en formato de cadena
     *
     * @return Cadena que representa la fecha en un formato específico
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
     * @param d Día del mes
     * @param m Mes del año
     * @param a Año
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
     * @brief Convierte la fecha a una representación de cadena
     *
     * @return Cadena que representa la fecha en formato DD/MM/AAAA
     */
    std::string toString() const;
};
#endif // FECHA_H