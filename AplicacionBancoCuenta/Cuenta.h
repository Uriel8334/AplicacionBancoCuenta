#pragma once
#ifndef CUENTA_H
#define CUENTA_H

#include <iostream>
#include <string>
#include <fstream>
#include <regex> // Incluye la libreria regex para validacion de fecha
#include "Validar.h" // Incluye la clase de validacion
#include "Fecha.h" // Incluye la clase Fecha para manejar fechas

// clase de cabecera para la clase Cuenta
// clase Cuenta abstracta, haciendo uso de templates, memoria dinamica
template <typename T>
class Cuenta {
protected:
	// Atributos comunes a todas las cuentas
	std::string numeroCuenta;
	T saldo;
	Fecha fechaApertura;
	std::string estadoCuenta;

	// Punteros para implementar una lista doblemente enlazada
	Cuenta<T>* siguiente;
	Cuenta<T>* anterior;

	// Constructores protegidos para herencia
	Cuenta()
		: numeroCuenta(""), saldo(0), fechaApertura(),estadoCuenta(""), siguiente(nullptr), anterior(nullptr) {
	}

	// Constructor con parametros
	Cuenta(std::string numeroCuenta, T saldo, const std::string& fechaStr, std::string estadoCuenta)
		: numeroCuenta(numeroCuenta), saldo(saldo),fechaApertura(fechaStr), // Inicializa la fecha con el string
		estadoCuenta(estadoCuenta), siguiente(nullptr), anterior(nullptr) {
	}

	// destructor virtual para permitir la liberacion de recursos en clases derivadas
	virtual ~Cuenta() {}

public:
	// Getters
	std::string getNumeroCuenta() const { return numeroCuenta; }
	T getSaldo() const { return saldo; }
	Fecha getFechaApertura() const { return fechaApertura; }
	std::string getEstadoCuenta() const { return estadoCuenta; }
	Cuenta<T>* getSiguiente() const { return siguiente; }
	Cuenta<T>* getAnterior() const { return anterior; }
	
	// Setters
	std::string setNumeroCuenta(const std::string& numero) { numeroCuenta = numero; return numeroCuenta; }
	T setSaldo(T nuevoSaldo) { saldo = nuevoSaldo; return saldo; }
	Fecha setFechaApertura(const std::string& fechaStr) { fechaApertura = Fecha(fechaStr); return fechaApertura; } // Asigna la fecha a partir de un string
	std::string setEstadoCuenta(const std::string& estado) { estadoCuenta = estado; return estadoCuenta; }
	void setSiguiente(Cuenta<T>* sig) { siguiente = sig; }
	void setAnterior(Cuenta<T>* ant) { anterior = ant; }

	// Metodos virtuales puros
	virtual void depositar(T cantidad) = 0; // Metodo para depositar
	virtual void retirar(T cantidad) = 0; // Metodo para retirar
	virtual double consultarSaldo() const = 0; // Metodo para consultar saldo
	virtual std::string consultarEstado() const = 0; // Metodo para consultar estado
	virtual void guardarEnArchivo(const std::string& nombreArchivo) const = 0; // Metodo para guardar en archivo
	virtual void cargarDesdeArchivo(const std::string& nombreArchivo) = 0; // Metodo para cargar desde archivo
	virtual void mostrarInformacion(const std::string& cedula = "", bool limpiarPantalla = true) const = 0; // Metodo para mostrar informacion de la cuenta
};
#endif // CUENTA_H
