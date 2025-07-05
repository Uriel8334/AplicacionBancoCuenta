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

    // Tabla de caracteres alfanuméricos para QR
    const std::string alphanumeric = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ $%*+-./:";

    // Patrones de máscara para mejorar la legibilidad
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
        ESTRUCTURADO,  // Formato legible estructurado
        JSON          // Formato JSON estándar
    };

    FormatoDatos formatoSalida;

    // Métodos privados para construcción del QR
    std::string convertirAMayusculas(const std::string& str);
    int calcularLongitudDatos();

    // Métodos principales para generación QR según ISO/IEC 18004
    std::vector<bool> codificarDatos();          // Codifica datos en modo byte
    std::vector<bool> generarECC(const std::vector<bool>& datos);  // Genera Reed-Solomon ECC
    void construirMatriz();                      // Construye matriz QR completa
    void aplicarEnmascaramiento(int patron);     // Aplica enmascaramiento XOR
    void imprimirEnConsola();                    // Imprime QR en consola con '█' y espacios

    // Métodos de codificación y generación QR optimizados para móviles
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

    // Métodos para datos según ISO/IEC 18004
    std::string generarDatosEstructurados();
    std::string generarFormatoEstructurado(); // Optimizado para mostrar solo NOMBRE y N. CUENTA
    std::string generarFormatoJSON(); // Optimizado para formato compacto

    // Métodos para construcción estándar ISO/IEC 18004
    void agregarPatronesAlineamiento(); // Implementación completa para versiones 2-5
    void agregarInformacionVersion();
    std::vector<bool> generarCodigoCorreccion(const std::vector<bool>& datos);

public:
    // Constructores
    GeneradorQRBanco(const Persona& persona, const std::string& numCuenta);
    GeneradorQRBanco(const std::string& nombreCompleto, const std::string& numCuenta);

    // Métodos principales
    void generar(); // Genera código QR según estándar ISO/IEC 18004
    void imprimir(); // Muestra QR en consola con símbolos especiales

    // Métodos para guardar y exportar QR
    void guardarComoSVG(const std::string& nombreArchivo);
    void guardarInformacionCuenta(const std::string& nombreArchivo);

#ifdef _WIN32
    void guardarComoPNG(const std::string& nombreArchivo);
#endif

    // Método para cambiar el formato de salida
    void setFormatoSalida(bool useJSON);

    // Método para obtener el contenido codificado en el QR
    std::string obtenerContenidoQR() {
        return generarDatosEstructurados();
    }

    // Métodos de validación y compatibilidad
    static bool esNumeroCuentaValido(const std::string& cuenta); // Valida exactamente 10 dígitos numéricos
    static bool esNombreValido(const std::string& nombre);
    bool generarCodigoQRCompatible(); // Optimizado para estándar ISO/IEC 18004
    bool verificarCompatibilidadQR(); // Verifica compatibilidad con lectores móviles
};

#endif // GENERADORQRBANCO_H
