#pragma once  
#ifndef PERSONA_H  
#define PERSONA_H  

#include <iostream>
#include <string>  
#include "CuentaAhorros.h"  
#include "CuentaCorriente.h"  
#include <functional>

namespace PersonaUI {
	extern const std::function<bool(const std::string&)> seleccionarSiNo;
	extern const std::function<double(double, double, const std::string&)> ingresarMonto;
}

using namespace std;

class Persona {
private:
	string cedula;
	string nombres;
	string apellidos;
	string fechaNacimiento;
	string correo;
	string direccion;
	CuentaAhorros* cabezaAhorros; // Puntero a la cabeza de la lista de cuentas de ahorros
	CuentaCorriente* cabezaCorriente; // Puntero a la cabeza de la lista de cuentas corrientes
	int numCuentas = 0; // Para limitar a 5 cuentas de ahorros y sin limite para corrientes
	int numCorrientes = 0; // Para contar las cuentas corrientes
	bool isDestroyed = false; // Nuevo indicador para evitar uso despues de destruccion

public:
	// Constructor por defecto
	Persona() : cabezaAhorros(nullptr), cabezaCorriente(nullptr), numCuentas(0), numCorrientes(0), isDestroyed(false) {}

	// Constructor con parametros
	Persona(const string& cedula, const string& nombres, const string& apellidos,
		const string& fechaNacimiento, const string& correo, const string& direccion)
		: cedula(cedula), nombres(nombres), apellidos(apellidos),
		fechaNacimiento(fechaNacimiento), correo(correo), direccion(direccion),
		cabezaAhorros(nullptr), cabezaCorriente(nullptr), numCuentas(0), numCorrientes(0), isDestroyed(false) {
	}

	// Destructor 
	~Persona() {
		// Liberar memoria de las cuentas de ahorros  
		while (cabezaAhorros != nullptr) {
			CuentaAhorros* temp = cabezaAhorros;
			cabezaAhorros = static_cast<CuentaAhorros*>(cabezaAhorros->getSiguiente());
			delete temp;
		}

		// Liberar memoria de las cuentas corrientes
		while (cabezaCorriente != nullptr) {
			CuentaCorriente* temp = cabezaCorriente;
			cabezaCorriente = static_cast<CuentaCorriente*>(cabezaCorriente->getSiguiente());
			delete temp;
		}
		isDestroyed = true; // Marcar como destruido  
	}


	// Metodo interno opcional para verificar si la instancia es valida
	bool isValidInstance() const { return !isDestroyed; }

	// Setters  
	bool setCedula(const string& cedula) { this->cedula = cedula; return true; }
	bool setNombres(const string& nombres) { this->nombres = nombres; return true; }
	bool setApellidos(const string& apellidos) { this->apellidos = apellidos; return true; }
	bool setFechaNacimiento(const string& fechaNacimiento) { this->fechaNacimiento = fechaNacimiento; return true; }
	bool setCorreo(const string& correo) { this->correo = correo; return true; }
	bool setDireccion(const string& direccion) { this->direccion = direccion; return true; }
	void setNumeCuentas(int numCuentas) { this->numCuentas = numCuentas; }
	void setNumCorrientes(int numCorrientes) { this->numCorrientes = numCorrientes; }
	CuentaAhorros* setCabezaAhorros(CuentaAhorros* nuevaCabeza) {
		if (nuevaCabeza) {
			nuevaCabeza->setSiguiente(cabezaAhorros);
			nuevaCabeza->setAnterior(nullptr);

			if (cabezaAhorros)
				cabezaAhorros->setAnterior(nuevaCabeza);
		}

		cabezaAhorros = nuevaCabeza;
		return cabezaAhorros;
	}
	CuentaCorriente* setCabezaCorriente(CuentaCorriente* nuevaCabeza) {
		if (nuevaCabeza)
		{
			nuevaCabeza->setSiguiente(cabezaCorriente);
			nuevaCabeza->setAnterior(nullptr);
			if(cabezaCorriente)
				cabezaCorriente->setAnterior(nuevaCabeza);
		}
		cabezaCorriente = nuevaCabeza;
		return cabezaCorriente; 
	}

	// Getters  
	string getCedula() const { return this->cedula; }
	string getNombres() const { return this->nombres; }
	string getApellidos() const { return this->apellidos; }
	string getFechaNacimiento() const { return this->fechaNacimiento; }
	string getCorreo() const { return this->correo; }
	string getDireccion() const { return this->direccion; }
	int getNumCuentas() const { return this->numCuentas; }
	int getNumCorrientes() const { return this->numCorrientes; }
	CuentaAhorros* getCabezaAhorros() const { return this->cabezaAhorros; }
	CuentaCorriente* getCabezaCorriente() const { return this->cabezaCorriente; }

	void ingresarDatos(); // Metodo para ingresar los datos de la persona

	void ingresarDatos(const std::string& cedulaEsperada); // Metodo para ingresar los datos de la persona con una cedula esperada
	std::string ingresarCedula(std::string& cedula); // Metodo para ingresar la cedula de la persona
	std::string ingresarNombres(std::string& nombres); // Metodo para ingresar los nombres de la persona
	std::string ingresarApellidos(std::string& apellidos); // Metodo para ingresar los apellidos de la persona
	std::string ingresarFechaNacimiento(std::string& fechaNacimiento); // Metodo para ingresar la fecha de nacimiento de la persona
	std::string ingresarCorreo(std::string& correo); // Metodo para ingresar el correo de la persona
	std::string ingresarDireccion(std::string& direccion); // Metodo para ingresar la direccion de la persona
	bool corregirDatos(); // Metodo para corregir los datos de la persona

	void mostrarDatos() const; // Muestra los datos de la persona  
	int mostrarCuentas(const std::string& tipoCuenta) const; // Muestra las cuentas de la persona por tipo (ahorros/corriente)
	void guardarEnArchivo() const; // Guarda los datos de la persona en un archivo
	// Busca cuentas por criterio
	int buscarPersonaPorCriterio(const std::string& criterioBusqueda, const std::string& numeroCuenta, const std::string& fechaApertura, double saldo) const;
	// Busca cuentas por fecha de apertura
	void buscarPersonaPorFecha(const std::string& fechaApertura) const;
	// Busca una persona por sus cuentas
	int buscarPersonaPorCuentas(const string& numeroCuenta) const;
	int guardarCuentas(std::ofstream& archivo, std::string tipo) const; // Guarda las cuentas de la persona en un archivo, sea ahorros o corriente
	bool crearAgregarCuentaAhorros(CuentaAhorros* nuevaCuenta, const std::string& cedulaEsperada) ; // Metodo para crear y agregar una cuenta de ahorros
	bool crearAgregarCuentaCorriente(CuentaCorriente* nuevaCuenta, const std::string& cedulaEsperada) ; // Metodo para crear y agregar una cuenta corriente
	bool crearSoloCuentaAhorros(CuentaAhorros* nuevaCuenta, const std::string& cedulaEsperada); // Metodo para crear una cuenta sin ingresar datos de la persona
	bool crearSoloCuentaCorriente(CuentaCorriente* nuevaCuenta, const std::string& cedulaEsperada); // Metodo para crear una cuenta corriente sin ingresar datos de la persona
	std::string crearNumeroCuenta(Cuenta<double>* nuevaCuenta) ; // Metodo para crear un numero de cuenta unico

	/*
	// Metodo unificado para crear y agregar cuenta de ahorros
	bool crearYAgregarCuentaAhorros(Persona* persona) {
		// Verificar limite
		if (numCuentas >= 5) {
			std::cout << "No puede tener mas de 5 cuentas de ahorros.\n";
			return false;
		}

		// Crear la cuenta directamente como objeto dinamico
		CuentaAhorros* nuevaCuenta;

		// Llamar al metodo crearCuenta() del objeto nuevaCuenta
		// que configura numero de cuenta, fecha, saldo inicial, etc.
		nuevaCuenta->crearCuenta(persona);

		// Crear el nodo y agregarlo a la lista directamente
		NodoCuentaAhorros* nuevo = new NodoCuentaAhorros(nuevaCuenta);

		// Verificacion de seguridad
		if (nuevo->cuenta == nullptr) {
			std::cout << "[ERROR] Cuenta no inicializada" << std::endl;
			delete nuevo;
			return false;
		}

		// Agregar a la lista con las validaciones normales
		nuevo->siguiente = cabezaAhorros;
		nuevo->anterior = nullptr;
		if (cabezaAhorros != nullptr) {
			cabezaAhorros->anterior = nuevo;
		}
		cabezaAhorros = nuevo;
		numCuentas++;

		std::cout << "Cuenta de Ahorros creada y agregada con exito." << std::endl;
		return true;
	}

	// Metodo unificado para crear y agregar cuenta corriente
	bool crearYAgregarCuentaCorriente(Persona* persona) {
		// No hay limite para cuentas corrientes (a diferencia de las de ahorro)

		// Crear la cuenta directamente como objeto dinamico
		CuentaCorriente* nuevaCuenta;

		// Llamar al metodo crearCuenta() del objeto nuevaCuenta
		// que configura numero de cuenta, fecha, saldo inicial, etc.
		nuevaCuenta->crearCuenta(persona);

		// Crear el nodo y agregarlo a la lista directamente
		//NodoCuentaCorriente* nuevo = new NodoCuentaCorriente(nuevaCuenta);

		// Verificacion de seguridad
		if (nuevo->cuenta == nullptr) {
			std::cout << "[ERROR] Cuenta corriente no inicializada" << std::endl;
			delete nuevo;
			return false;
		}

		// Agregar a la lista con las validaciones normales
		nuevo->siguiente = cabezaCorriente;
		nuevo->anterior = nullptr;
		if (cabezaCorriente != nullptr) {
			cabezaCorriente->anterior = nuevo;
		}
		cabezaCorriente = nuevo;

		std::cout << "Cuenta Corriente creada y agregada con exito." << std::endl;
		return true;
	}

	void seleccionarFecha();
	}; */
};
#endif // PERSONA_H
