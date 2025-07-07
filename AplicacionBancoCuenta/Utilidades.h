#pragma once
#ifndef UTILIDADES_H
#define UTILIDADES_H

#include <string>
#include <vector>
#include <functional>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "CodigoQR.h"


class NodoPersona;
class Persona;

/**
 * @class Utilidades
 * @brief Clase est�tica con funciones auxiliares para todo el sistema bancario
 *
 * Esta clase proporciona m�todos utilitarios para manipulaci�n de datos,
 * formateo, validaciones, operaciones con archivos, generaci�n de c�digos QR,
 * y otras funcionalidades de soporte que son utilizadas por todo el sistema bancario.
 */
class Utilidades {
public:

	/**
	 * @brief Convierte una cadena de texto a un valor num�rico double
	 * @param texto Cadena a convertir
	 * @return Valor num�rico double
	 */
	static double ConvertirADouble(const std::string& texto);

	/**
	 * @brief Formatea un monto num�rico para presentaci�n
	 * @param monto Valor a formatear
	 * @param decimales N�mero de decimales a mostrar (por defecto 2)
	 * @return Cadena formateada
	 */
	static std::string FormatearMonto(double monto, int decimales = 2);

	/**
	 * @brief Formatea una fecha a partir de sus componentes
	 * @param dia D�a del mes
	 * @param mes Mes del a�o
	 * @param anio A�o
	 * @return Cadena con la fecha formateada
	 */
	static std::string FormatearFecha(int dia, int mes, int anio);

	/**
	 * @brief Verifica si una cadena contiene solo caracteres num�ricos
	 * @param texto Cadena a verificar
	 * @return true si la cadena es num�rica, false en caso contrario
	 */
	static bool EsNumerico(const std::string& texto);

	/**
	 * @brief Verifica si una cadena representa un correo electr�nico v�lido
	 * @param correo Direcci�n de correo a validar
	 * @return true si el correo es v�lido, false en caso contrario
	 */
	static bool EsCorreoValido(const std::string& correo);

	/**
	 * @brief Elimina espacios en blanco de una cadena
	 * @param texto Cadena a procesar
	 * @return Cadena sin espacios
	 */
	static std::string EliminarEspacios(const std::string& texto);

	/**
	 * @brief Convierte una cadena a may�sculas
	 * @param texto Cadena a convertir
	 * @return Cadena convertida a may�sculas
	 */
	static std::string ConvertirAMayusculas(const std::string& texto);

	/**
	 * @brief Convierte una cadena a min�sculas
	 * @param texto Cadena a convertir
	 * @return Cadena convertida a min�sculas
	 */
	static std::string ConvertirAMinusculas(const std::string& texto);

	/**
	 * @brief Retorna una cadena para la opci�n de regresar al men� principal
	 * @return Cadena con la opci�n de regreso
	 */
	static std::string Regresar();

	/**
	 * @brief Posiciona el cursor en coordenadas espec�ficas de la consola
	 * @param x Coordenada horizontal
	 * @param y Coordenada vertical
	 */
	static void gotoxy(int x, int y);

	/**
	 * @brief Muestra el men� de ayuda del sistema
	 */
	static void mostrarMenuAyuda();

	/**
	 * @brief Ordena un vector de punteros utilizando el algoritmo burbuja
	 * @tparam T Tipo de los objetos a ordenar
	 * @param vec Vector de punteros a ordenar
	 * @param criterio Funci�n que define el criterio de ordenamiento
	 */
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

	/**
	 * @brief Ordena una lista enlazada utilizando el algoritmo burbuja
	 * @tparam Nodo Tipo del nodo en la lista enlazada
	 * @tparam T Tipo de dato contenido en los nodos
	 * @param cabeza Puntero al primer nodo de la lista
	 * @param criterio Funci�n que define el criterio de ordenamiento
	 */
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

	/**
	 * @brief Calcula el hash SHA1 de un archivo
	 * @param rutaArchivo Ruta del archivo
	 * @return Hash SHA1 calculado como cadena hexadecimal
	 */
	static std::string calcularSHA1(const std::string& rutaArchivo);

	/**
	 * @brief Verifica si un archivo tiene el hash SHA1 esperado
	 * @param rutaArchivo Ruta del archivo a verificar
	 * @param hashEsperado Hash SHA1 esperado
	 * @return true si el hash coincide, false en caso contrario
	 */
	static bool verificarSHA1(const std::string& rutaArchivo, const std::string& hashEsperado);

	/**
	 * @brief Guarda el hash de un archivo en otro archivo
	 * @param rutaArchivo Ruta del archivo cuyo hash se guarda
	 * @param hash Hash a guardar
	 */
	static void guardarHashArchivo(const std::string& rutaArchivo, const std::string& hash);

	/**
	 * @brief Lee el hash almacenado en un archivo
	 * @param rutaHashArchivo Ruta del archivo que contiene el hash
	 * @return Hash le�do como cadena
	 */
	static std::string leerHashArchivo(const std::string& rutaHashArchivo);

	/**
	 * @brief Muestra una demostraci�n did�ctica de �rbol B con personas
	 * @param cabeza Puntero al primer nodo de personas
	 */
	static void PorArbolB(NodoPersona* cabeza);

	/**
	 * @brief Marca el inicio de una operaci�n cr�tica de interfaz
	 *
	 * Notifica a la marquesina para evitar interferencias durante operaciones cr�ticas
	 */
	static void iniciarOperacionCritica();

	/**
	 * @brief Marca el fin de una operaci�n cr�tica de interfaz
	 */
	static void finalizarOperacionCritica();

	/**
	 * @brief Limpia la pantalla preservando la marquesina superior
	 * @param lineasMarquesina N�mero de l�neas que ocupa la marquesina (por defecto 2)
	 */
	static void limpiarPantallaPreservandoMarquesina(int lineasMarquesina = 2);

	/**
	 * @brief Genera un archivo PDF con c�digo QR
	 * @param persona Referencia a un objeto Persona
	 * @param numeroCuenta N�mero de cuenta asociada
	 * @return true si se gener� correctamente, false en caso contrario
	 */
	static bool generarQR(const Persona& persona, const std::string& numeroCuenta);
};

#endif // UTILIDADES_H