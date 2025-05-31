#pragma once  
#ifndef PERSONA_H  
#define PERSONA_H  

#include <string>  
#include "CuentaAhorros.h"  
#include "CuentaCorriente.h"  
#include "NodoCuentaAhorros.h"
#include "NodoCuentaCorriente.h"

using namespace std;

class Persona {
private:
	string cedula;
	string nombres;
	string apellidos;
	string fechaNacimiento;
	string correo;
	string direccion;
	NodoCuentaAhorros* cabezaAhorros = nullptr; // Inicializa el puntero a nullptr
	NodoCuentaCorriente* cabezaCorriente = nullptr; // Inicializa el puntero a nullptr
	int numAhorros = 0; // Para limitar a 5 cuentas de ahorros
    bool isDestroyed = false; // Nuevo indicador para evitar uso despues de destruccion

public:
	// Constructor por defecto  
	Persona(); // Constructor por defecto para inicializar los atributos a valores predeterminados
	~Persona(); // Destructor para liberar memoria de las listas enlazadas

	// Metodo interno opcional para verificar si la instancia es valida
    bool isValidInstance() const { return !isDestroyed; }

	// Setters  
	bool setCedula(const string& cedula);
	bool setNombres(const string& nombres);
	bool setApellidos(const string& apellidos);
	bool setFechaNacimiento(const string& fechaNacimiento);
	bool setCorreo(const string& correo);
	bool setDireccion(const string& direccion);
	void setCabezaAhorros(NodoCuentaAhorros* cabeza) { cabezaAhorros = cabeza; }
	void setCabezaCorriente(NodoCuentaCorriente* cabeza) { cabezaCorriente = cabeza; }

	// Getters  
	string getCedula() const;
	string getNombres() const;
	string getApellidos() const;
	string getFechaNacimiento() const;
	string getCorreo() const;
	string getDireccion() const;
	NodoCuentaAhorros* getListaCuentasAhorros() const { return cabezaAhorros; }
	NodoCuentaCorriente* getListaCuentasCorriente() const { return cabezaCorriente; }


	void ingresarDatos(); // Metodo para ingresar los datos de la persona
	void ingresarDatos(const std::string& cedulaEsperada); // Metodo para ingresar los datos de la persona con una cedula esperada
	void mostrarDatos() const; // Muestra los datos de la persona  
	void guardarEnArchivo() const; // Guarda los datos de la persona en un archivo

	// Busca cuentas por criterio
	int buscarPersonaPorCriterio(const std::string& criterio, const std::string& valorStr, double valorNum) const; 
	// Busca cuentas por fecha de apertura

	void buscarPersonaPorFecha(const std::string& fecha) const; 
	// Busca una persona por sus cuentas
	int buscarPersonaPorCuentas(const string& numeroCuenta) const;

	// Metodo para guardar las cuentas de ahorro en el archivo
	int guardarCuentasAhorro(std::ofstream& archivo) const;

	// Metodo para guardar las cuentas corrientes en el archivo
	int guardarCuentasCorriente(std::ofstream& archivo) const;

	// Metodo unificado para crear y agregar cuenta de ahorros
	bool crearYAgregarCuentaAhorros(Persona* persona) {
	    // Verificar limite
	    if (numAhorros >= 5) {
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
	    numAhorros++;
	    
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
	    NodoCuentaCorriente* nuevo = new NodoCuentaCorriente(nuevaCuenta);
	    
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
};
#endif
