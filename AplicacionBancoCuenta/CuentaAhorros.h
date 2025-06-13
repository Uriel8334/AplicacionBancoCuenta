#pragma once
#ifndef CUENTAAHORROS_H
#define CUENTAAHORROS_H

#include "Cuenta.h"
#include "Validar.h" // Include validation class
#include "Cifrado.h" // Include encryption class
#include "Fecha.h"   // Include date class
#include "Utilidades.h" // Include utilities class

// CuentaAhorros class that inherits from Cuenta<int>
class CuentaAhorros : public Cuenta<double> {
private:
	double tasaInteres;
	Fecha fechaApertura; // Fecha de apertura de la cuenta
public:
	// contrictor por defecto
	CuentaAhorros() : Cuenta<double>(), tasaInteres(0.0) {}

	// Constructor con parametros
	CuentaAhorros(std::string numCuenta, double saldo, const std::string& fecha, const std::string& estado, double tasa)
		: Cuenta<double>(numCuenta, saldo, fecha, estado), tasaInteres(tasa) {}
	// destructor
	~CuentaAhorros() {}

	//CuentaAhorros() : Cuenta<int>(), tasaInteres(0) {}
	//CuentaAhorros(int numCuenta, int sal, std::string fecha, std::string estado, double tasa)
	//	: Cuenta<int>(numCuenta, sal, fecha, estado), tasaInteres(tasa) {}
	//CuentaAhorros(int numCuenta, int sal, std::string fecha,
	//	std::string estado, double tasa);

	// Setters 
	void setTasaInteres(double tast) { this->tasaInteres = tast; }
	void setNumeroCuenta(std::string numCuenta) { this->numeroCuenta = numCuenta; }
	void setSaldo(double saldo) { this->saldo = saldo; }
	void setFechaApertura(const std::string& fecha) { this->fechaApertura = fecha; }
	void setEstadoCuenta(const std::string& estado) { this->estadoCuenta = estado; }
	CuentaAhorros* setCuentaAhorros(CuentaAhorros* cuenta) { return this; }
	CuentaAhorros* setSiguiente(CuentaAhorros* siguiente) { this->siguiente = siguiente; return this; }
	CuentaAhorros* setAnterior(CuentaAhorros* anterior) { this->anterior = anterior; return this; }

	// Getters
	void getTasaInteres(double tasa) { this->tasaInteres = tasa; }
	std::string getNumeroCuenta() const { return this->numeroCuenta; }
	std::string getFechaApertura() const { return this->fechaApertura.toString(); }
	double getSaldo() const { return this->saldo; }
	std::string getEstadoCuenta() const { return this->estadoCuenta; }
	CuentaAhorros* getCuentaAhorros() { return this; }
	CuentaAhorros* getSiguiente() const {
		Cuenta<double>* sig = this->siguiente;
		return sig ? dynamic_cast<CuentaAhorros*>(sig) : nullptr;
	}	
	CuentaAhorros* getAnterior() const { return dynamic_cast<CuentaAhorros*>(this->anterior); }

	// Metodos de la clase CuentaAhorros
	void depositar(double cantidad);
	void retirar(double cantidad);
	double consultarSaldo() const;
	std::string consultarEstado() const;
	std::string formatearSaldo() const;
	std::string formatearConComas(double saldo) const;
	void mostrarInformacion(const std::string& cedula, bool limpiarPantalla) const;
	void guardarEnArchivo(const std::string& nombreArchivo) const override;
	void cargarDesdeArchivo(const std::string& nombreArchivo) override;
	int calcularInteres() const;


};

#endif // CUENTAAHORROS_H
