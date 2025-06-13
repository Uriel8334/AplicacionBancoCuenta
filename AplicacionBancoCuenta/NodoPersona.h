#pragma once
#ifndef NODOPERSONA_H
#define NODOPERSONA_H

// Declaracion forward en lugar de incluir el archivo completo
class Persona;

class NodoPersona {
public:
    Persona* persona;  // Puntero en lugar de valor
    NodoPersona* siguiente;

    // Constructor que toma un puntero
    NodoPersona(Persona* p) : persona(p), siguiente(nullptr) {}
    
    // Destructor 
    ~NodoPersona() {
        // No delete persona - lo maneja quien creo la persona
        persona = nullptr;
        siguiente = nullptr;
    }
};

#endif // NODOPERSONA_H