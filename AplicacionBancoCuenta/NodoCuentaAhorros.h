#pragma once
#ifndef NODOCUENTAAHORROS_H
#define NODOCUENTAAHORROS_H

// Se incluye la clase CuentaAhorros para poder usarla como puntero en el nodo
class CuentaAhorros;

// Clase NodoCuentaAhorros que representa un nodo en una lista doblemente enlazada
class NodoCuentaAhorros {
public:
	CuentaAhorros* cuenta; // Usa un puntero a la clase CuentaAhorros
    NodoCuentaAhorros* siguiente;
    NodoCuentaAhorros* anterior;
	// lista de cuentas de ahorros
    
	// Constructor que inicializa el nodo con un puntero a una cuenta de ahorros
    NodoCuentaAhorros(CuentaAhorros* cuentaPtr)
        : cuenta(cuentaPtr), siguiente(nullptr), anterior(nullptr) {}

	
    
};

#endif // NODOCUENTAAHORROS_H
