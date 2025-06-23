#pragma once
#ifndef UTILIDADES_H
#define UTILIDADES_H

#include <string>
#include <vector>
#include <functional>
#include <fstream>
#include <sstream>
#include <iomanip>

class NodoPersona;
class Persona;

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

	// Metodo para ordenar punteros a objetos T usando el algoritmo burbuja
    template<typename T>
    static void burbuja(std::vector<T*>& vec, std::function<bool(const T*, const T*)> criterio) {
        for (size_t i = 0; i < vec.size(); ++i) {
            for (size_t j = 0; j < vec.size() - i - 1; ++j) {
                if (!criterio(vec[j], vec[j + 1])) {
                    std::swap(vec[j], vec[j + 1]);
                }
            }
        }
    }

    // Ordena una lista enlazada simple de nodos usando burbuja (O(n^2)), version template
    template<typename Nodo, typename T>
    static void burbujaLista(Nodo* cabeza, std::function<bool(const T*, const T*)> criterio) {
        // Contar nodos
        int n = 0;
        for (Nodo* tmp = cabeza; tmp; tmp = tmp->siguiente) ++n;
        if (n < 2) return;

        for (int i = 0; i < n - 1; ++i) {
            Nodo* actual = cabeza;
            for (int j = 0; j < n - i - 1; ++j) {
                if (!criterio(actual->persona, actual->siguiente->persona)) {
                    std::swap(actual->persona, actual->siguiente->persona);
                }
                actual = actual->siguiente;
            }
        }
    }

    // Metodos para hash de archivos
    static std::string calcularSHA1(const std::string& rutaArchivo); 
    static bool verificarSHA1(const std::string& rutaArchivo, const std::string& hashEsperado);
    
    // Metodos para el manejo de archivos de hash
    static void guardarHashArchivo(const std::string& rutaArchivo, const std::string& hash);
    static std::string leerHashArchivo(const std::string& rutaHashArchivo);
};

#endif // UTILIDADES_H