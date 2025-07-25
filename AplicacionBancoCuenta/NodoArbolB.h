#pragma once
#ifndef NODOARBOLB_H
#define NODOARBOLB_H

#include <vector>

/**
 * @class NodoArbolB
 * @brief Implementación de un nodo para un Árbol B
 *
 * Clase genérica que representa un nodo en una estructura de árbol B,
 * almacenando punteros a objetos y referencias a nodos hijos.
 *
 * @tparam T Tipo de datos almacenados en el nodo
 */
template<typename T>
class NodoArbolB {
public:

	/** @brief Indica si este nodo es una hoja (no tiene hijos) */
	bool esHoja;
	/** @brief Vector de punteros a las claves (objetos) almacenados en este nodo */
	std::vector<T*> claves;
	/** @brief Vector de punteros a los nodos hijos */
	std::vector<NodoArbolB<T>*> hijos; // Punteros a los hijos

	/**
	 * @brief Constructor del nodo de Árbol B
	 * @param hoja Define si el nodo es una hoja (por defecto true)
	 */
	NodoArbolB(bool hoja = true) : esHoja(hoja) {}

	/**
	 * @brief Destructor, libera la memoria de los nodos hijos recursivamente
	 */
	~NodoArbolB() {
		for (auto& hijo : hijos) {
			delete hijo;
		}
	}
};

#endif //NODOARBOLB_H