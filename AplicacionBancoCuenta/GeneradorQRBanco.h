#pragma once

#ifndef GENERADORQRBANCO_H
#define GENERADORQRBANCO_H

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include "Persona.h"

class GeneradorQRBanco {
private:
    std::vector<std::vector<bool>> matrix;
    int size;
    std::string numeroCuenta;
    std::string nombreCompleto;

    // Tabla de caracteres alfanum�ricos para QR
    const std::string alphanumeric = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ $%*+-./:";

    // Patrones de m�scara para mejorar la legibilidad
    enum PatronMascara {
        MASCARA_000 = 0,
        MASCARA_001 = 1,
        MASCARA_010 = 2,
        MASCARA_011 = 3,
        MASCARA_100 = 4,
        MASCARA_101 = 5,
        MASCARA_110 = 6,
        MASCARA_111 = 7
    };

    // Enum para formato de datos
    enum class FormatoDatos {
        ESTRUCTURADO,
        JSON
    };
    
    FormatoDatos formatoSalida;

    // M�todos privados para construcci�n del QR
    std::string convertirAMayusculas(const std::string& str);
    int calcularLongitudDatos();
    int caracterAAlfonumerico(char c);
    std::vector<bool> codificarDatosBancarios();
    void agregarPatronesPosicion();
    void agregarSeparadores();
    void agregarPatronesTiempo();
    void agregarInformacionFormato();
    void agregarModuloOscuro();
    bool estaReservado(int fila, int col);
    void colocarDatos(const std::vector<bool>& data);
    void aplicarMascara(PatronMascara patron);
    int obtenerCapacidadDatos();

    // Nuevos m�todos privados
    std::string generarDatosEstructurados();
    std::string generarFormatoEstructurado();
    std::string generarFormatoJSON();

public:
    // Constructor que recibe datos de persona y cuenta
    GeneradorQRBanco(const Persona& persona, const std::string& numCuenta);

    // Constructor directo con strings
    GeneradorQRBanco(const std::string& nombreCompleto, const std::string& numCuenta);

    // M�todos p�blicos
    void generar();
    void imprimir();
    void guardarComoSVG(const std::string& nombreArchivo);
    void guardarInformacionCuenta(const std::string& nombreArchivo);

    // M�todo para cambiar el formato de salida
    void setFormatoSalida(bool useJSON) {
        formatoSalida = useJSON ? FormatoDatos::JSON : FormatoDatos::ESTRUCTURADO;
    }
    
    // M�todo para obtener el contenido que se codificar� en el QR
    std::string obtenerContenidoQR() {
        return generarDatosEstructurados();
    }

    // Validadores est�ticos
    static bool esNumeroCuentaValido(const std::string& cuenta); // Valida exactamente 10 d�gitos num�ricos
    static bool esNombreValido(const std::string& nombre);
};

#endif // GENERADORQRBANCO_H