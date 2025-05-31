#pragma once
#ifndef CUENTA_H
#define CUENTA_H

#include <iostream>
#include <string>
#include <fstream>
#include <regex> // Incluye la libreria regex para validacion de fecha
#include "Validar.h" // Incluye la clase de validacion

// clase de cabecera para la clase Cuenta
// clase Cuenta abstracta, haciendo uso de templates, memoria dinamica
template <typename T>
class Cuenta {
	// Aseguramos en tiempo de compilacion que se utilice 'int' como tipo de plantilla
	static_assert(std::is_same<T, int>::value,
		"Error: La clase Cuenta<T> debe instanciarse con 'int' para este proyecto.");

protected:
	T numeroCuenta;
	T saldo;
	std::string fechaApertura;
	std::string estadoCuenta;

	Cuenta<T>* siguiente;
	Cuenta<T>* anterior;

	// Constructores protegidos para herencia
	Cuenta()
		: numeroCuenta(0), saldo(0), fechaApertura(""), estadoCuenta(""), siguiente(nullptr), anterior(nullptr) {
	}

	// Constructor con parametros
	Cuenta(T numCuenta, T sal, std::string fecha, std::string estado)
		: numeroCuenta(numCuenta), saldo(sal), fechaApertura(fecha), estadoCuenta(estado), siguiente(nullptr), anterior(nullptr) {
	}

public:
	virtual ~Cuenta() {}

	// Metodos virtuales puros
	virtual void depositar(T cantidad) = 0; // Metodo para depositar
	virtual void retirar(T cantidad) = 0; // Metodo para retirar
	virtual T consultarSaldo() const = 0; // Metodo para consultar saldo
	virtual std::string consultarEstado() const = 0; // Metodo para consultar estado
	virtual void guardarEnArchivo(const std::string& nombreArchivo) const = 0; // Metodo para guardar en archivo
	virtual void cargarDesdeArchivo(const std::string& nombreArchivo) = 0; // Metodo para cargar desde archivo
	virtual void crearCuenta(T* persona) = 0; // Metodo para crear cuenta

	// Metodo base para mostrar informacion de la cuenta
	// Este metodo puede ser sobrescrito por clases derivadas
	virtual void mostrarInformacion(const std::string& cedula = "", bool limpiarPantalla = true) const {
		// Validacion basica
		if (this == nullptr) {
			return;
		}
		
		// Limpieza de pantalla
		system("cls");
		
		// Titulo generico con formato
		std::cout << "\n" << std::string(50, '=') << std::endl;
		std::cout << "          INFORMACIoN DE CUENTA" << std::endl;
		std::cout << std::string(50, '=') << "\n" << std::endl;
		
		// Informacion del titular si esta disponible
		if (!cedula.empty()) {
			std::cout << "Cedula del titular: " << cedula << std::endl;
			std::cout << std::string(30, '-') << std::endl;
		}
		
		// Informacion comun a todas las cuentas
		std::cout << "Numero de cuenta: " << this->numeroCuenta << std::endl;
		std::cout << "Fecha de apertura: " << this->fechaApertura << std::endl;
		std::cout << "Estado: " << this->estadoCuenta << std::endl;
		
		// Mostrar saldo con formato (asumimos que las clases derivadas implementaran su formateo)
		std::cout << "Saldo actual: $" << formatearSaldo() << std::endl;
		
		// Pie de pagina
		std::cout << "\n" << std::string(50, '-') << std::endl;
		std::cout << "Presione cualquier tecla para continuar..." << std::endl;
		_getch();
	}

	// Metodo helper abstracto para formatear el saldo
	// Cada clase derivada lo implementara segun sus necesidades
	virtual std::string formatearSaldo() const = 0;

	// Sobrecarga del operador de asignacion para copiar los atributos de una cuenta a otra
	Cuenta<T>& operator=(const Cuenta<T>& otra) {
		if (this != &otra) {
			numeroCuenta = otra.numeroCuenta;
			saldo = otra.saldo;
			fechaApertura = otra.fechaApertura;
			estadoCuenta = otra.estadoCuenta;
			siguiente = otra.siguiente;
			anterior = otra.anterior;
		}
		return *this;
	}

	// Sobrecarga del operador de salida para imprimir la informacion de la cuenta
	friend std::ostream& operator<<(std::ostream& os, const Cuenta<T>& cuenta) {
		os << "Numero de cuenta: " << cuenta.numeroCuenta << "\n"
			// Se imprime el saldo actual, del monto ingresado por el usuario
			<< "Saldo: $" << cuenta.saldo << "\n"
			<< "Fecha de apertura: " << cuenta.fechaApertura << "\n"
			<< "Estado de la cuenta: " << cuenta.estadoCuenta << "\n";
		//<< "Siguiente cuenta: " << (cuenta.siguiente ? std::to_string(cuenta.siguiente->numeroCuenta) : "N/A") << "\n"
		//<< "Cuenta anterior: " << (cuenta.anterior ? std::to_string(cuenta.anterior->numeroCuenta) : "N/A") << "\n";
		return os;
	}

	// Getters y Setters
	T getNumeroCuenta() const {
		 // Asegurar que la instancia es valida - verificacion de seguridad
		#ifdef _DEBUG
		if (reinterpret_cast<uintptr_t>(this) > 0xFFFFFFFF00000000) {
			throw std::runtime_error("Acceso a objeto invalido en getNumeroCuenta()");
		}
		#endif
		return numeroCuenta;
	}
	

	void setNumeroCuenta(T numCuenta) { numeroCuenta = numCuenta; }

	T getSaldo() const { return saldo; }

	bool setSaldo(const T& nuevoSaldo) {
		// Convertir el valor numerico a string antes de validarlo
		if (Validar::ValidarSaldo(std::to_string(nuevoSaldo))) {
			this->saldo = nuevoSaldo;
			return true;
		}
		return false;
	}

	std::string getFechaApertura() const { return fechaApertura; }
	void setFechaApertura(const std::string& fecha) {
		// Validar formato dd/mm/aaaa antes de asignar
		std::regex formato(R"(\d{2}/\d{2}/\d{4})");
		if (!std::regex_match(fecha, formato)) {
			std::cerr << "Fecha de apertura invalida (use dd/mm/aaaa).\n";
			return; // No se asigna si es invalida
		}
		fechaApertura = fecha;
	}

	std::string getEstadoCuenta() const { return estadoCuenta; }
	void setEstadoCuenta(const std::string& estado) { estadoCuenta = estado; }

	Cuenta<T>* getSiguiente() const { return siguiente; }
	void setSiguiente(Cuenta<T>* sig) { siguiente = sig; }

	Cuenta<T>* getAnterior() const { return anterior; }
	void setAnterior(Cuenta<T>* ant) { anterior = ant; }
};

#endif // CUENTA_H
