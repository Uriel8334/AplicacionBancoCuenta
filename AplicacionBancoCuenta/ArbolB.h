#pragma once
#ifndef ARBOLB_H
#define ARBOLB_H

#include <string>
#include <set>
#include <functional>
#include <conio.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <regex>
#include <ctime>
#include <chrono>
#include <queue>
#include <limits>
#include "NodoArbolB.h"
#include "Persona.h"


/**
 * @class ArbolB
 * @brief Implementación didáctica de un Árbol B para propósitos educativos
 *
 * Esta clase implementa una versión simplificada de un Árbol B para
 * ilustrar sus principios básicos de funcionamiento. Permite buscar elementos
 * y visualizar la estructura del árbol.
 *
 * @tparam T Tipo de datos almacenados en el árbol
 */
template<typename T>
class ArbolB {
public:
	/** @brief Nodo raíz del árbol */
	NodoArbolB<T>* raiz;
	/** @brief Grado mínimo del árbol (determina el número de claves por nodo) */
	int grado;

	/**
	 * @brief Busca una clave en un nodo y sus subárboles
	 *
	 * @param nodo Nodo actual en el que buscar
	 * @param valor Valor a buscar
	 * @param comparador Función para comparar elementos
	 * @return T* Puntero al objeto encontrado o nullptr si no existe
	 */
	T* buscarEnNodo(NodoArbolB<T>* nodo, const std::string& valor,
		std::function<bool(const T*, const std::string&)> comparador) {
		if (!nodo) return nullptr;

		// Buscar la clave en el nodo actual
		for (auto& clave : nodo->claves) {
			if (comparador(clave, valor)) {
				return clave;
			}
		}

		// Si es hoja y no se encontrO, no existe
		if (nodo->esHoja) {
			return nullptr;
		}

		// Determinar subArbol donde podrIa estar la clave
		int i = 0;
		while (i < nodo->claves.size() && !comparador(nodo->claves[i], valor)) {
			i++;
		}

		// Buscar en el hijo correspondiente
		return buscarEnNodo(nodo->hijos[i < nodo->hijos.size() ? i : nodo->hijos.size() - 1], valor, comparador);
	}

	/**
	 * @brief Inserta un elemento en el árbol B, evitando duplicados
	 *
	 * @param elemento Elemento a insertar
	 */
	void insertarElemento(T* elemento) {
		// Si la raíz está vacía, simplemente insertamos el elemento
		if (!raiz) {
			raiz = new NodoArbolB<T>(true);
			raiz->claves.push_back(elemento);
			return;
		}

		// Verificar si el elemento ya existe en el árbol
		if (auto persona = dynamic_cast<Persona*>(elemento)) {
			// Buscar usando cédula como identificador único
			std::function<bool(const T*, const std::string&)> comparadorCedula =
				[](const T* p, const std::string& cedula) {
				if (auto persona = dynamic_cast<const Persona*>(p)) {
					return persona->getCedula() == cedula;
				}
				return false;
				};

			if (buscarEnNodo(raiz, persona->getCedula(), comparadorCedula)) {
				// El elemento ya existe, no lo insertamos
				return;
			}
		}

		// Si la raíz está llena (tiene 2 claves para grado 3), dividir primero
		if (raiz->claves.size() == 2) {
			NodoArbolB<T>* nuevaRaiz = new NodoArbolB<T>(false);
			nuevaRaiz->hijos.push_back(raiz);
			dividirHijo(nuevaRaiz, 0);
			raiz = nuevaRaiz;
		}

		// Ahora insertamos en la raíz (que sabemos que no está llena)
		insertarNoLleno(raiz, elemento);
	}

	/**
	 * @brief Divide un hijo del nodo cuando está lleno
	 *
	 * @param padre Nodo padre
	 * @param indice Índice del hijo a dividir
	 */
	void dividirHijo(NodoArbolB<T>* padre, int indice) {
		// El hijo que vamos a dividir
		NodoArbolB<T>* hijo = padre->hijos[indice];

		// Para árbol B de grado 3, el hijo lleno tiene exactamente 2 claves
		if (hijo->claves.size() != 2) {
			return; // No debería ocurrir, pero por seguridad
		}

		// Crear nuevo nodo para la división (hermano derecho)
		NodoArbolB<T>* nuevoNodo = new NodoArbolB<T>(hijo->esHoja);

		// En grado 3, la clave del medio (índice 1) se promueve al padre
		T* claveCentral = hijo->claves[1];

		// En un árbol B de grado 3, solo hay una clave a la derecha de la central
		// No agregamos la clave central al nuevo nodo, solo va al padre
		if (hijo->claves.size() > 1) {
			nuevoNodo->claves.push_back(hijo->claves[1]); // La segunda clave va al nuevo nodo
		}

		// Si no es hoja, mover también los hijos correspondientes
		if (!hijo->esHoja) {
			nuevoNodo->hijos.push_back(hijo->hijos[1]);
			nuevoNodo->hijos.push_back(hijo->hijos[2]);
			// Ajustar hijos del nodo original
			hijo->hijos.resize(1);
		}

		// Ajustar el tamaño del hijo original (mantener solo la clave izquierda)
		hijo->claves.resize(1);

		// Insertar la clave promovida en el padre en la posición correcta
		padre->claves.insert(padre->claves.begin() + indice, claveCentral);

		// Insertar el nuevo nodo como hijo del padre
		padre->hijos.insert(padre->hijos.begin() + indice + 1, nuevoNodo);
	}

	/**
	 * @brief Inserta un elemento en un nodo que no está lleno
	 *
	 * @param nodo Nodo donde insertar
	 * @param elemento Elemento a insertar
	 */
	void insertarNoLleno(NodoArbolB<T>* nodo, T* elemento) {
		// Verificar duplicados en el nodo actual para Personas
		Persona* personaElemento = dynamic_cast<Persona*>(elemento);
		if (personaElemento) {
			for (auto& clave : nodo->claves) {
				Persona* personaClave = dynamic_cast<Persona*>(clave);
				if (personaClave) {
					if (personaElemento->getCedula() == personaClave->getCedula()) {
						return; // Duplicado encontrado, no insertamos
					}
				}
			}
		}

		// Si es una hoja, insertamos directamente
		if (nodo->esHoja) {
			// Encontrar la posición correcta
			int i = static_cast<int>(nodo->claves.size()) - 1;
			while (i >= 0 && compararElementos(elemento, nodo->claves[i])) {
				i--;
			}

			// Insertar en la posición correcta
			nodo->claves.insert(nodo->claves.begin() + i + 1, elemento);
			return;
		}

		// Si no es hoja, encontrar el hijo donde debería ir
		int i = static_cast<int>(nodo->claves.size()) - 1;
		while (i >= 0 && compararElementos(elemento, nodo->claves[i])) {
			i--;
		}
		i++;

		// Verificar si ese hijo está lleno
		if (nodo->hijos[i]->claves.size() == 2) {
			// Dividir el hijo lleno antes de insertar
			dividirHijo(nodo, i);

			// Verificar si la clave promovida es igual al elemento a insertar
			if (auto personaElemento = dynamic_cast<Persona*>(elemento)) {
				if (auto personaClave = dynamic_cast<Persona*>(nodo->claves[i])) {
					if (personaElemento->getCedula() == personaClave->getCedula()) {
						return; // No insertar duplicado
					}
				}
			}

			// Determinar en qué hijo insertar
			if (compararElementos(nodo->claves[i], elemento)) {
				i++;
			}
		}

		// Insertar recursivamente en el hijo correspondiente
		insertarNoLleno(nodo->hijos[i], elemento);
	}
	/**
	 * @brief Compara dos elementos para determinar su orden
	 *
	 * @param a Primer elemento
	 * @param b Segundo elemento
	 * @return true si a va antes que b
	 */
	bool compararElementos(const T* a, const T* b) {
		// Para personas, comparamos por cédula para mantener orden único
		if (auto personaA = dynamic_cast<const Persona*>(a)) {
			if (auto personaB = dynamic_cast<const Persona*>(b)) {
				return personaA->getCedula() < personaB->getCedula();
			}
		}

		// Si no son del mismo tipo o no se pueden comparar de otra forma
		return a < b; // Comparación por dirección de memoria
	}

public:
	/**
	 * @brief Constructor del Árbol B
	 * @param _grado Grado mínimo del árbol (determina el número de claves por nodo)
	 */
	ArbolB(int _grado) : raiz(nullptr), grado(_grado) {
		if (grado < 2) grado = 2; // MInimo grado 2
	}

	/**
	 * @brief Destructor, libera la memoria de todos los nodos
	 */
	~ArbolB() {
		if (raiz) delete raiz;
	}

	/**
	 * @brief Construye el árbol a partir de un vector de elementos, eliminando duplicados
	 *
	 * @param elementos Vector de punteros a los elementos para construir el árbol
	 */
	void construirDesdeVector(std::vector<T*>& elementos) {
		// Eliminamos cualquier raíz previa
		if (raiz) {
			delete raiz;
			raiz = nullptr;
		}

		if (elementos.empty()) {
			return;
		}

		// Eliminar duplicados antes de construir el árbol
		std::vector<T*> elementosUnicos;
		std::set<std::string> idVistos; // Conjunto para rastrear cédulas ya procesadas

		for (T* elemento : elementos) {
			if (auto persona = dynamic_cast<Persona*>(elemento)) {
				// Usamos cédula como identificador único para Persona
				std::string identificador = persona->getCedula();

				// Verificar si este identificador ya ha sido procesado
				if (idVistos.find(identificador) == idVistos.end()) {
					idVistos.insert(identificador);
					elementosUnicos.push_back(elemento);
				}
			}
			else {
				// Para otros tipos, usar la dirección de memoria como criterio de unicidad
				elementosUnicos.push_back(elemento);
			}
		}

		// Ordenar elementos antes de insertarlos
		std::sort(elementosUnicos.begin(), elementosUnicos.end(), [this](const T* a, const T* b) {
			return compararElementos(a, b);
			});

		// Construir un árbol B con elementos únicos
		for (auto& elem : elementosUnicos) {
			insertarElemento(elem);
		}
	}
	/**
	 * @brief Busca un elemento en el árbol
	 *
	 * @param valor Valor a buscar
	 * @param comparador Función para comparar elementos
	 * @return T* Puntero al objeto encontrado o nullptr si no existe
	 */
	T* buscar(const std::string& valor, std::function<bool(const T*, const std::string&)> comparador) {
		return buscarEnNodo(raiz, valor, comparador);
	}

	/**
	 * @brief Muestra la estructura del árbol por niveles
	 */
	void mostrar() {
		if (!raiz) {
			std::cout << "Arbol vacIo" << std::endl;
			return;
		}

		// Recorrer por niveles
		std::queue<NodoArbolB<T>*> cola;
		cola.push(raiz);

		int nivel = 0;
		while (!cola.empty()) {
			int nodosEnNivel = cola.size();
			std::cout << "Nivel " << nivel << ": ";

			for (int i = 0; i < nodosEnNivel; i++) {
				NodoArbolB<T>* nodoActual = cola.front();
				cola.pop();

				// Mostrar claves del nodo
				std::cout << "[ ";
				for (auto& clave : nodoActual->claves) {
					if (auto persona = dynamic_cast<Persona*>(clave)) {
						std::cout << persona->getNombres() << " ";
					}
					else {
						std::cout << "objeto ";
					}
				}
				std::cout << "] ";

				// Encolar hijos
				for (auto& hijo : nodoActual->hijos) {
					cola.push(hijo);
				}
			}
			std::cout << std::endl;
			nivel++;
		}
	}

	/**
	 * @brief Calcula la altura aproximada del árbol
	 * @return int Altura aproximada del árbol
	 */
	int altura() const {
		if (!raiz) return 0;

		// Aproximamos la altura basada en el número de nodos
		int nodosEstimados = 0;
		for (auto& hijo : raiz->hijos) {
			nodosEstimados++;
			if (!hijo->esHoja) {
				nodosEstimados += static_cast<int>(hijo->hijos.size());
			}
		}

		// La altura del árbol es aproximadamente log(n)
		return raiz->hijos.empty() ? 1 : 2 + (nodosEstimados > 0 ? 1 : 0);
	}


	/**
	 * @brief Verifica si el árbol está vacío
	 * @return true si el árbol no tiene nodos, false en caso contrario
	 */
	bool estaVacio() const {
		return raiz == nullptr || raiz->claves.empty();
	}

	/**
	 * @brief Muestra el árbol con animación y conexiones gráficas entre nodos con soporte Unicode
	 *
	 * @param elementoResaltado Valor a resaltar en el árbol
	 * @param colorResaltado Color a usar para el resaltado
	 * @param animado Si true, muestra la búsqueda de forma animada
	 * @param selCriterio Criterio de selección para mostrar el valor adecuado (0=Cédula, 1=Nombre, etc.)
	 */
	void mostrarAnimado(const std::string& elementoResaltado, int colorResaltado, bool animado, int selCriterio) {
		if (!raiz) {
			std::cout << "Árbol vacío" << std::endl;
			return;
		}

		// Configurar consola para UTF-8
		SetConsoleOutputCP(CP_UTF8);
		SetConsoleCP(CP_UTF8);

		// Calcular la estructura completa del árbol primero para determinar posiciones
		std::vector<std::vector<NodoArbolB<T>*>> niveles;
		std::vector<std::vector<std::string>> valoresNodos;
		std::vector<std::vector<int>> posicionesX;

		// Conjunto para detectar duplicados durante la visualización
		std::set<std::string> nodosVistos;

		// Recorrido por niveles para almacenar todos los nodos
		std::queue<NodoArbolB<T>*> cola;
		cola.push(raiz);

		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		int colorNormal = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;

		while (!cola.empty()) {
			int nodosEnNivel = cola.size();
			std::vector<NodoArbolB<T>*> nivelActual;
			std::vector<std::string> valoresNivel;

			for (int i = 0; i < nodosEnNivel; i++) {
				NodoArbolB<T>* nodoActual = cola.front();
				cola.pop();
				nivelActual.push_back(nodoActual);

				// Construir representaciones de string para este nodo
				std::vector<std::string> valoresClave;
				for (auto& clave : nodoActual->claves) {
					std::string valorClave;
					bool esValorDuplicado = false;

					if (auto persona = dynamic_cast<Persona*>(clave)) {
						// Valor a mostrar según criterio seleccionado
						valorClave = selCriterio == 0 ? persona->getCedula() :
							selCriterio == 1 ? persona->getNombres() :
							selCriterio == 2 ? persona->getApellidos() :
							persona->getFechaNacimiento();

						// Verificar si ya hemos visto este valor (para destacar duplicados)
						std::string identificador = persona->getCedula(); // Usamos cédula como id único
						if (nodosVistos.find(identificador) != nodosVistos.end()) {
							esValorDuplicado = true;
						}
						else {
							nodosVistos.insert(identificador);
						}

						// Tomar solo los primeros caracteres para evitar nodos muy anchos
						if (valorClave.length() > 10)
							valorClave = valorClave.substr(0, 10) + "..";

						// Marcar duplicados visualmente
						if (esValorDuplicado) {
							valorClave = "*" + valorClave + "*";
						}
					}
					else {
						valorClave = "obj";
					}

					valoresClave.push_back(valorClave);
				}

				// Juntar valores del nodo con separadores
				std::string representacionNodo = "";
				for (size_t j = 0; j < valoresClave.size(); j++) {
					representacionNodo += valoresClave[j];
					if (j < valoresClave.size() - 1)
						representacionNodo += "│";
				}

				valoresNivel.push_back(representacionNodo);

				// Encolar hijos para el siguiente nivel
				if (!nodoActual->esHoja) {
					for (auto& hijo : nodoActual->hijos) {
						if (hijo != nullptr) {
							cola.push(hijo);
						}
					}
				}
			}

			niveles.push_back(nivelActual);
			valoresNodos.push_back(valoresNivel);
		}

		// Calcular posiciones X para cada nodo
		int maxWidth = 120; // Ancho máximo de la consola
		for (size_t nivel = 0; nivel < niveles.size(); nivel++) {
			std::vector<int> posicionesNivel;
			int espacioEntreNodos = maxWidth / (niveles[nivel].size() + 1);

			for (size_t i = 0; i < niveles[nivel].size(); i++) {
				int posX = (i + 1) * espacioEntreNodos;
				posicionesNivel.push_back(posX);
			}

			posicionesX.push_back(posicionesNivel);
		}

		// Limpiar pantalla y dibujar el árbol
		Utilidades::limpiarPantallaPreservandoMarquesina(1);
		std::cout << "=== ÁRBOL B DIDÁCTICO ===" << std::endl;
		std::cout << "Ordenado por: " << (selCriterio == 0 ? "Cédula" :
			selCriterio == 1 ? "Nombre" :
			selCriterio == 2 ? "Apellido" :
			"Fecha de nacimiento") << std::endl << std::endl;

		// Dibujar el árbol nivel por nivel con conexiones
		int yBase = 4; // Posición Y inicial
		int altoNodo = 3; // Alto de cada nodo con espacio para conexiones

		for (size_t nivel = 0; nivel < niveles.size(); nivel++) {
			int y = yBase + nivel * altoNodo;

			// Dibujar nodos de este nivel
			for (size_t i = 0; i < niveles[nivel].size(); i++) {
				int x = posicionesX[nivel][i] - valoresNodos[nivel][i].length() / 2;
				Utilidades::gotoxy(x, y);

				// Verificar si alguna clave del nodo debe ser resaltada
				bool resaltado = false;
				if (!elementoResaltado.empty()) {
					std::string nodoValorLower = Utilidades::ConvertirAMinusculas(valoresNodos[nivel][i]);
					std::string elementoLower = Utilidades::ConvertirAMinusculas(elementoResaltado);
					resaltado = nodoValorLower.find(elementoLower) != std::string::npos;
				}

				// Caracteres Unicode para dibujar los nodos
				const std::string esquinaSuperiorIzquierda = "╭";
				const std::string esquinaSuperiorDerecha = "╮";
				const std::string esquinaInferiorIzquierda = "╰";
				const std::string esquinaInferiorDerecha = "╯";
				const std::string lineaHorizontal = "─";
				const std::string lineaVertical = "│";

				// Dibujar el nodo con su recuadro usando caracteres Unicode
				if (resaltado) {
					SetConsoleTextAttribute(hConsole, colorResaltado);
					std::cout << esquinaSuperiorIzquierda << std::string(valoresNodos[nivel][i].length() + 2, '─') << esquinaSuperiorDerecha;
					Utilidades::gotoxy(x, y + 1);
					std::cout << lineaVertical << " " << valoresNodos[nivel][i] << " " << lineaVertical;
					Utilidades::gotoxy(x, y + 2);
					std::cout << esquinaInferiorIzquierda << std::string(valoresNodos[nivel][i].length() + 2, '─') << esquinaInferiorDerecha;
					SetConsoleTextAttribute(hConsole, colorNormal);

					// Pausa si está animado
					if (animado) Sleep(300);
				}
				else {
					std::cout << esquinaSuperiorIzquierda << std::string(valoresNodos[nivel][i].length() + 2, '─') << esquinaSuperiorDerecha;
					Utilidades::gotoxy(x, y + 1);
					std::cout << lineaVertical << " " << valoresNodos[nivel][i] << " " << lineaVertical;
					Utilidades::gotoxy(x, y + 2);
					std::cout << esquinaInferiorIzquierda << std::string(valoresNodos[nivel][i].length() + 2, '─') << esquinaInferiorDerecha;
				}

				// Dibujar líneas de conexión a hijos si no es el último nivel
				if (nivel < niveles.size() - 1 && !niveles[nivel][i]->esHoja) {
					// Encontrar los hijos de este nodo en el siguiente nivel
					size_t hijoInicio = 0;
					for (size_t k = 0; k < i; k++) {
						hijoInicio += niveles[nivel][k]->hijos.size();
					}

					for (size_t j = 0; j < niveles[nivel][i]->hijos.size(); j++) {
						if (hijoInicio + j < posicionesX[nivel + 1].size()) {
							// Punto de inicio (debajo del nodo padre)
							int xStart = posicionesX[nivel][i];
							int yStart = y + 3;

							// Punto final (arriba del nodo hijo)
							int xEnd = posicionesX[nivel + 1][hijoInicio + j];
							int yEnd = y + altoNodo - 1;

							// Dibujar línea diagonal con caracteres Unicode mejorados
							int steps = yEnd - yStart;
							for (int step = 0; step <= steps; step++) {
								int xLine = xStart + (xEnd - xStart) * step / steps;
								int yLine = yStart + step;
								Utilidades::gotoxy(xLine, yLine);

								// Elegir el carácter Unicode correcto según la dirección
								if (xEnd > xStart) {
									std::cout << "╱";  // Diagonal descendente hacia la derecha
								}
								else if (xEnd < xStart) {
									std::cout << "╲";  // Diagonal descendente hacia la izquierda
								}
								else {
									std::cout << "│";  // Línea vertical
								}

								// Pausa mínima si está animado
								if (animado) Sleep(10);
							}
						}
					}
				}
			}

			// Pausa entre niveles si está animado
			if (animado) Sleep(200);
		}

		std::cout << std::endl;
		// Mover el cursor después del árbol para próximas salidas
		Utilidades::gotoxy(0, yBase + niveles.size() * altoNodo + 1);

		// Restaurar configuración de consola original si es necesario
		// SetConsoleOutputCP(default_cp);
	}
	/**
	 * @brief Elimina un elemento del árbol
	 *
	 * @param valor Valor a buscar para eliminar
	 * @param comparador Función para comparar elementos
	 * @return true si se eliminó correctamente, false si no se encontró o hubo error
	 */
	bool eliminar(const std::string& valor, std::function<bool(const T*, const std::string&)> comparador) {
		if (!raiz) return false;

		// Buscar primero para verificar que existe
		T* aEliminar = buscar(valor, comparador);
		if (!aEliminar) return false;

		// Para esta implementación simple, vamos a reconstruir el árbol sin el elemento
		std::vector<T*> elementos;
		recolectarElementos(raiz, elementos);

		// Eliminar el elemento del vector (evitando duplicados)
		int eliminados = 0;
		auto it = std::remove_if(elementos.begin(), elementos.end(),
			[&](T* elem) {
				if (comparador(elem, valor)) {
					// Solo eliminar la primera ocurrencia para evitar eliminar múltiples elementos
					if (eliminados == 0) {
						eliminados++;
						return true;
					}
				}
				return false;
			});

		if (it != elementos.end()) {
			elementos.erase(it, elementos.end());

			// Reconstruir árbol
			if (raiz) delete raiz;
			raiz = nullptr;

			if (!elementos.empty()) {
				construirDesdeVector(elementos);
			}

			return true;
		}

		return false;
	}
	/**
	 * @brief Recolecta todos los elementos del árbol en un vector
	 *
	 * @param nodo Nodo actual en la recursión
	 * @param elementos Vector donde se almacenarán los elementos
	 */
	void recolectarElementos(NodoArbolB<T>* nodo, std::vector<T*>& elementos) {
		if (!nodo) return;

		// Agregar claves del nodo actual
		for (auto& clave : nodo->claves) {
			elementos.push_back(clave);
		}

		// Recorrer hijos recursivamente
		for (auto& hijo : nodo->hijos) {
			recolectarElementos(hijo, elementos);
		}
	}

};

#endif // ARBOLB_H
