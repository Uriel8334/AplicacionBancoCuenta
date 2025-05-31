#pragma once
#ifndef UTILIDADES_H
#define UTILIDADES_H

#include <string>


class Utilidades {
public:
    // Constructor y destructor
    Utilidades();
    ~Utilidades();

    // Metodos para conversiones
    static double ConvertirADouble(const std::string& texto);

    // Metodos para formateo
    static std::string FormatearMonto(double monto, int decimales = 2);
    static std::string FormatearFecha(int dia, int mes, int anio);

    // Validaciones
    static bool EsNumerico(const std::string& texto);
    static bool EsCorreoValido(const std::string& correo);

    // Operaciones con strings
    static std::string EliminarEspacios(const std::string& texto);
    static std::string ConvertirAMayusculas(const std::string& texto);
    static std::string ConvertirAMinusculas(const std::string& texto);

    // Metodo para regresar al menu principal
    static std::string Regresar(); 

	static void gotoxy(int x, int y); // Metodo para mover el cursor en la consola

	static void mostrarMenuAyuda(); // Metodo para mostrar el menu de ayuda


};
#endif // UTILIDADES_H