#pragma once
#ifndef UTILIDADES_H
#define UTILIDADES_H

#define NOMINMAX
#include <string>
#include <vector>
#include <functional>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "CodigoQR.h"
#include "Utilidades.h"
#include <iostream>
#include <windows.h>
#include <mutex>
#include <thread>



class NodoPersona;
class Persona;

/**
 * @class Utilidades
 * @brief Clase estática con funciones auxiliares para todo el sistema bancario
 *
 * Esta clase proporciona métodos utilitarios para manipulación de datos,
 * formateo, validaciones, operaciones con archivos, generación de códigos QR,
 * y otras funcionalidades de soporte que son utilizadas por todo el sistema bancario.
 */
class Utilidades {
public:


	/**
	 * @brief Muestra un menú en la consola
	 * @param seleccion Opción seleccionada
	 * @param opciones Lista de opciones a mostrar
	 * @param numOpciones Número de opciones en la lista
	 * @param x Coordenada X para la posición del menú
	 * @param y Coordenada Y para la posición del menú
	 * @param seleccionAnterior Opción seleccionada anteriormente (por defecto -1)
	 */
	static void mostrarMenu(int seleccion, const std::vector<std::string>& opciones, int x, int y, int seleccionAnterior);

	/**
	 * @brief Muestra un menú interactivo con opciones y permite seleccionar una
	 * @param titulo Título del menú
	 * @param opciones Lista de opciones a mostrar
	 * @param numOpciones Número de opciones en la lista
	 * @param x Coordenada X para la posición del menú
	 * @param y Coordenada Y para la posición del menú
	 * @return Índice de la opción seleccionada
	 */
	static int menuInteractivo(const std::string& titulo, const std::vector<std::string>& opciones, int x, int y);


	/**
	 * @brief Convierte una cadena de texto a un valor numérico double
	 * @param texto Cadena a convertir
	 * @return Valor numérico double
	 */
	static double ConvertirADouble(const std::string& texto);

	/**
	 * @brief Formatea un monto numérico para presentación
	 * @param monto Valor a formatear
	 * @param decimales Número de decimales a mostrar (por defecto 2)
	 * @return Cadena formateada
	 */
	static std::string FormatearMonto(double monto, int decimales = 2);

	/**
	 * @brief Formatea una fecha a partir de sus componentes
	 * @param dia Día del mes
	 * @param mes Mes del año
	 * @param anio Año
	 * @return Cadena con la fecha formateada
	 */
	static std::string FormatearFecha(int dia, int mes, int anio);

	/**
	 * @brief Verifica si una cadena contiene solo caracteres numéricos
	 * @param texto Cadena a verificar
	 * @return true si la cadena es numérica, false en caso contrario
	 */
	static bool EsNumerico(const std::string& texto);

	/**
	 * @brief Verifica si una cadena representa un correo electrónico válido
	 * @param correo Dirección de correo a validar
	 * @return true si el correo es válido, false en caso contrario
	 */
	static bool EsCorreoValido(const std::string& correo);

	/**
	 * @brief Elimina espacios en blanco de una cadena
	 * @param texto Cadena a procesar
	 * @return Cadena sin espacios
	 */
	static std::string EliminarEspacios(const std::string& texto);

	/**
	 * @brief Convierte una cadena a mayúsculas
	 * @param texto Cadena a convertir
	 * @return Cadena convertida a mayúsculas
	 */
	static std::string ConvertirAMayusculas(const std::string& texto);

	/**
	 * @brief Convierte una cadena a minúsculas
	 * @param texto Cadena a convertir
	 * @return Cadena convertida a minúsculas
	 */
	static std::string ConvertirAMinusculas(const std::string& texto);

	/**
	 * @brief Retorna una cadena para la opción de regresar al menú principal
	 * @return Cadena con la opción de regreso
	 */
	static std::string Regresar();

	/**
	 * @brief Posiciona el cursor en coordenadas específicas de la consola
	 * @param x Coordenada horizontal
	 * @param y Coordenada vertical
	 */
	static void gotoxy(int x, int y);

	/**
	 * @brief Muestra el menú de ayuda del sistema
	 */
	static void mostrarMenuAyuda();

	/**
	 * @brief Ordena un vector de punteros utilizando el algoritmo burbuja
	 * @tparam T Tipo de los objetos a ordenar
	 * @param vec Vector de punteros a ordenar
	 * @param criterio Función que define el criterio de ordenamiento
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
	 * @param criterio Función que define el criterio de ordenamiento
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
	 * @return Hash leído como cadena
	 */
	static std::string leerHashArchivo(const std::string& rutaHashArchivo);

	/**
	 * @brief Muestra una demostración didáctica de Árbol B con personas
	 * @param cabeza Puntero al primer nodo de personas
	 */
	static void PorArbolB(NodoPersona* cabeza);

	/**
	 * @brief Marca el inicio de una operación crítica de interfaz
	 *
	 * Notifica a la marquesina para evitar interferencias durante operaciones críticas
	 */
	static void iniciarOperacionCritica();

	/**
	 * @brief Marca el fin de una operación crítica de interfaz
	 */
	static void finalizarOperacionCritica();

	/**
	 * @brief Limpia la pantalla preservando la marquesina superior
	 * @param lineasMarquesina Número de líneas que ocupa la marquesina (por defecto 2)
	 */
	static void limpiarPantallaPreservandoMarquesina(int lineasMarquesina = 2);

	/**
	 * @brief Genera un archivo PDF con código QR
	 * @param persona Referencia a un objeto Persona
	 * @param numeroCuenta Número de cuenta asociada
	 * @return true si se generó correctamente, false en caso contrario
	 */
	static bool generarQR(const Persona& persona, const std::string& numeroCuenta);


	/**
	 * @brief Oculta el cursor de la consola
	 */
	static void ocultarCursor();

	/**
	 * @brief Muestra el cursor de la consola
	 */
	static void mostrarCursor();

	/**
	 * @brief Extrae los caracteres válidos para un double de una cadena
	 * @param texto Cadena a procesar
	 * @return Cadena con solo los caracteres válidos para un double
	 */
	static std::string ExtraerNumerosParaDouble(const std::string& texto);

	/**
	 * @brief Inserta separadores de miles en una cadena numérica
	 * @param numero Cadena numérica
	 * @param posDecimal Posición del punto decimal
	 * @return Cadena con separadores de miles
	 */
	static std::string InsertarSeparadoresMiles(const std::string& numero, size_t posDecimal);
};

#endif // UTILIDADES_H