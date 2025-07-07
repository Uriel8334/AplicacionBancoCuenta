#pragma once
#ifndef NODOPERSONA_H
#define NODOPERSONA_H

// Declaracion forward en lugar de incluir el archivo completo
/**
 * @class Persona
 * @brief Declaración adelantada (forward declaration) de la clase Persona
 *
 * Esta clase se declara aquí pero se define completamente en otro archivo.
 * Contiene información personal de clientes y gestiona sus cuentas bancarias.
 */
class Persona;

/**
 * @class NodoPersona
 * @brief Clase que implementa un nodo para una lista enlazada de personas
 *
 * Esta clase forma parte de una estructura de datos tipo lista enlazada
 * donde cada nodo contiene un puntero a un objeto Persona y un puntero
 * al siguiente nodo en la lista.
 */
class NodoPersona {
public:
    /** @brief Puntero al objeto Persona contenido en este nodo */
    Persona* persona;

    /** @brief Puntero al siguiente nodo en la lista enlazada */
    NodoPersona* siguiente;

    /**
     * @brief Constructor que inicializa el nodo con un puntero a Persona
     * @param p Puntero al objeto Persona que se almacenará en este nodo
     */
    NodoPersona(Persona* p) : persona(p), siguiente(nullptr) {}

    /**
     * @brief Destructor del nodo
     *
     * Libera las referencias pero no destruye el objeto Persona,
     * ya que la responsabilidad de eliminar dicho objeto pertenece
     * a quien lo creó.
     */
    ~NodoPersona() {
        // No delete persona - lo maneja quien creo la persona
        persona = nullptr;
        siguiente = nullptr;
    }
};

#endif // NODOPERSONA_H