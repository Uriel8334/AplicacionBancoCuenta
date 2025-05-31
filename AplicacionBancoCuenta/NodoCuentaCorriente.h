#pragma once
#ifndef NODOCUENTACORRIENTE_H
#define NODOCUENTACORRIENTE_H

// Se incluye la clase CuentaCorriente para poder usarla como puntero
class CuentaCorriente;

// Clase NodoCuentaCorriente que representa un nodo en una lista doblemente enlazada
class NodoCuentaCorriente {
public:
	CuentaCorriente* cuenta; // Usar un puntero a CuentaCorriente para evitar la inclusion del encabezado
    NodoCuentaCorriente* siguiente;
    NodoCuentaCorriente* anterior;
	//lista de cuentas corrientes

	// Constroctor que inicializa el nodo con un puntero a CuentaCorriente
    NodoCuentaCorriente(CuentaCorriente* cuentaPtr)
        : cuenta(cuentaPtr), siguiente(nullptr), anterior(nullptr) {}
};

#endif // NODOCUENTACORRIENTE_H