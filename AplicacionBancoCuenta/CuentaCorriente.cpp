/// @file CuentaAhorros.cpp
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <conio.h>
#include "Cuenta.h"
#include "Validar.h"
#include "Cifrado.h"
#include "Fecha.h"
#include "CuentaCorriente.h"

using namespace std;

#pragma region METODOS ESPECIFICOS DE LA CLASE
/// <summary>
/// Metodo para depositar una cantidad en la cuenta de ahorros
/// </summary>
/// <param name="cantidad"></param>
void CuentaCorriente::depositar(double cantidad) {
	if (cantidad > 0) {
		this->saldo += cantidad;
	}
	else {
		std::cout << "La cantidad a depositar debe ser positiva." << std::endl;
	}
}
/// <summary>
/// Metodo para retirar una cantidad de la cuenta de ahorros
/// </summary>
/// <param name="cantidad"></param>
void CuentaCorriente::retirar(double cantidad) {
	if (cantidad <= this->saldo) {
		this->saldo -= cantidad;
	}
	else {
		std::cout << "Fondos insuficientes." << std::endl;
	}
}

/// <summary>
/// Metodo para consultar el saldo de la cuenta de ahorros
/// </summary>
/// <returns></returns>
double CuentaCorriente::consultarSaldo() const {
	return this->saldo;
}
/// <summary>
/// Metodo para consultar el estado de la cuenta de ahorros
/// </summary>
/// <returns></returns>
std::string CuentaCorriente::consultarEstado() const {
	return this->estadoCuenta.empty() ? "ACTIVA" : this->estadoCuenta;
}

/// <summary>
/// Metodo para formatear el saldo de la cuenta de ahorros
/// </summary>
/// <returns></returns>
std::string CuentaCorriente::formatearSaldo() const {
	return formatearConComas(this->saldo);
}

/// <summary>
/// Metodo para formatear un valor en centavos a un string con comas y dos decimales
/// </summary>
/// <param name="saldo"></param>
/// <returns></returns>
std::string CuentaCorriente::formatearConComas(double saldo) const {
	// Convertir de centavos a valor decimal
	double valorReal = saldo / 100.0;
	std::ostringstream oss;
	oss.imbue(std::locale("en_US.UTF-8")); // Usar formato americano: 1,234.56
	oss << std::fixed << std::setprecision(2) << valorReal;
	return oss.str();
}

/// <summary>
/// Metodo para mostrar la informacion de la cuenta de ahorros
/// </summary>
/// <param name="cedula"></param>
/// <param name="limpiarPantalla"></param>
void CuentaCorriente::mostrarInformacion(const std::string& cedula = "", bool limpiarPantalla = true) const {
	
	// Validacion basica
	if (this == nullptr) {
		return;
	}

	// Limpieza de pantalla solo si se solicita
	if (limpiarPantalla) {
		std::system("cls");
	}

	// Título con formato específico para CuentaCorriente
	std::cout << "\n" << std::string(50, '=') << std::endl;
	std::cout << "          INFORMACION DE CUENTA CORRIENTE" << std::endl;
	std::cout << std::string(50, '=') << "\n" << std::endl;

	// Información del titular si está disponible
	if (!cedula.empty()) {
		std::cout << "Cedula del titular: " << cedula << std::endl;
		std::cout << std::string(30, '-') << std::endl;
	}

	// Información común
	std::cout << "Tipo de cuenta: CORRIENTE" << std::endl;
	std::cout << "Numero de cuenta: " << this->numeroCuenta << std::endl;

	// Imprimir usando el método de la clase Fecha
	std::cout << "Fecha de apertura: "
		<< this->fechaApertura.obtenerFechaFormateada()
		<< std::endl;

	const_cast<std::string&>(this->estadoCuenta) = consultarEstado();
	std::cout << "Estado: " << this->estadoCuenta << std::endl;
	std::cout << "Saldo actual: $" << formatearConComas(this->saldo) << std::endl;

	// Pie de página
	std::cout << "\n" << std::string(50, '-') << std::endl;
	std::cout << "Presione cualquier tecla para continuar..." << std::endl;
	int tecla = _getch();
	(void)tecla;
	// Limpieza de pantalla
	std::system("cls");
}

/// <summary>
/// Metodo para guardar la cuenta de ahorros en un archivo binario
/// </summary>
/// <param name="nombreArchivo"></param>
void CuentaCorriente::guardarEnArchivo(const std::string& nombreArchivo) const {
	std::ofstream archivo(nombreArchivo, std::ios::binary);
	if (archivo.is_open()) {
		archivo.write(reinterpret_cast<const char*>(&this->numeroCuenta), sizeof(this->numeroCuenta));
		archivo.write(reinterpret_cast<const char*>(&this->saldo), sizeof(this->saldo));

		// Convertir Fecha a string y escribir
		std::string fechaStr = this->fechaApertura.obtenerFechaFormateada();
		archivo.write(fechaStr.c_str(), fechaStr.size() + 1);

		archivo.write(this->estadoCuenta.c_str(), this->estadoCuenta.size() + 1);
		archivo.write(reinterpret_cast<const char*>(&montoMinimo), sizeof(montoMinimo));
		archivo.close();
	}
}

/// <summary>
/// Metodo para cargar la cuenta de ahorros desde un archivo binario
/// </summary>
/// <param name="nombreArchivo"></param>
void CuentaCorriente::cargarDesdeArchivo(const std::string& nombreArchivo) {
	std::ifstream archivo(nombreArchivo, std::ios::binary);
	if (archivo.is_open()) {
		archivo.read(reinterpret_cast<char*>(&this->numeroCuenta), sizeof(this->numeroCuenta));
		archivo.read(reinterpret_cast<char*>(&this->saldo), sizeof(this->saldo));
		char buffer[100];
		archivo.getline(buffer, 100, '\0');
		// Reconstruir la fecha desde el string
		this->fechaApertura = Fecha(buffer);

		archivo.getline(buffer, 100, '\0');
		this->estadoCuenta = buffer;

		archivo.read(reinterpret_cast<char*>(&montoMinimo), sizeof(montoMinimo));
		archivo.close();
	}
}

/// <summary>
/// Metodo para saber si el monto minimo es valido
/// </summary>
/// <param name="monto"></param>
void CuentaCorriente::esMontoMinimo(double monto) {
	// monto debe ser positivo y mayor o igual $250.00
	if (monto < 250.00) {
		std::cout << "El monto debe ser mayor o igual a $250.00" << std::endl;
		return;
	}// si es valido, asignamos el monto minimo
	else {
		this->montoMinimo = monto;
	}
}
#pragma endregion




