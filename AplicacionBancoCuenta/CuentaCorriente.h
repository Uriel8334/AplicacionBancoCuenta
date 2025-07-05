#pragma once
#ifndef CUENTACORRIENTE_H
#define CUENTACORRIENTE_H

#include "Cuenta.h"
#include "Validar.h"
#include "Cifrado.h"
#include "Fecha.h"

class CuentaCorriente : public Cuenta<double> {
private:
	// Atributo especifico de CuentaCorriente
	double montoMinimo; // monto minimo sino es $250.00, no se puede tener una CuentaCorriente / monto minimo en dolares
	Fecha fechaApertura; // Fecha de apertura de la cuenta
public:
	// Constructor por defecto
	CuentaCorriente() : Cuenta<double>(), montoMinimo(0.00) {}
	// Constructor con parametros sin monto minimo
	CuentaCorriente(std::string numeroCuenta, double saldo, const std::string& fechaApertura, const std::string estadoCuenta, double montoMinimo)
		: Cuenta<double>(numeroCuenta, saldo, fechaApertura, estadoCuenta), montoMinimo(montoMinimo) {}

	// SETTERS
	void setMontoMinimo(double montoMinimo) { this->montoMinimo = montoMinimo; }
	void setNumeroCuenta(std::string numeroCuenta) { this->numeroCuenta = numeroCuenta; }
	void setSaldo(double saldo) { this->saldo = saldo; }
	void setFechaApertura(const std::string& fecha) { this->fechaApertura = fecha; }
	void setEstadoCuenta(const std::string& estadoCuenta) { this->estadoCuenta = estadoCuenta; }
	CuentaCorriente* setCuentaCorriente(CuentaCorriente* cuentaCorriente) { return this; } // Metodo para establecer CuentaCorriente, aunque no se usa en este contexto
	CuentaCorriente* setSiguiente(CuentaCorriente* cuentaSiguiente) { this->siguiente = siguiente; return this; }
	CuentaCorriente* setAnterior(CuentaCorriente* cuentaAnterior) { this->anterior = anterior; return this; }

	// GETTERS
	double getMontoMinimo() const { return this->montoMinimo; }
	std::string getNumeroCuenta() const { return this->numeroCuenta; }
	double getSaldo() const { return this->saldo; }
	std::string getFechaApertura() const { return this->fechaApertura.toString(); }
	std::string getEstadoCuenta() const { return this->estadoCuenta; }
	CuentaCorriente* getCuentaCorriente() { return this; } // Metodo para obtener CuentaCorriente, aunque no se usa en este contexto
	CuentaCorriente* getSiguiente() const { 
		Cuenta<double>* sig = this->siguiente;
		return sig ? dynamic_cast<CuentaCorriente*>(sig) : nullptr;
	}
	CuentaCorriente* getAnterior() const { return dynamic_cast<CuentaCorriente*>(this->anterior); }

	// Metodos especificos de CuentaCorriente
	void depositar(double cantidad);
	void retirar(double cantidad);
	double consultarSaldo() const;
	std::string consultarEstado() const;
	std::string formatearSaldo() const;
	std::string formatearConComas(double saldo) const;
	void mostrarInformacion(const std::string& cedula, bool limpiarPantalla) const;
	void guardarEnArchivo(const std::string& nombreArchivo) const override;
	void cargarDesdeArchivo(const std::string& nombreArchivo) override;
	void esMontoMinimo(double montoMinimo);

};
#endif // CUENTACORRIENTE_H