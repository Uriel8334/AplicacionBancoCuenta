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
#include "CuentaAhorros.h" 

using namespace std;

#pragma region METODOS ESPECIFICOS DE LA CLASE 
/// <summary>
/// Metodo para depositar una cantidad en la cuenta de ahorros
/// </summary>
/// <param name="cantidad"></param>
void CuentaAhorros::depositar(double cantidad) {
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
void CuentaAhorros::retirar(double cantidad) {
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
double CuentaAhorros::consultarSaldo() const {
	return this->saldo;
}
/// <summary>
/// Metodo para consultar el estado de la cuenta de ahorros
/// </summary>
/// <returns></returns>
std::string CuentaAhorros::consultarEstado() const {
	return this->estadoCuenta.empty() ? "ACTIVA" : this->estadoCuenta;
}

/// <summary>
/// Metodo para formatear el saldo de la cuenta de ahorros
/// </summary>
/// <returns></returns>
std::string CuentaAhorros::formatearSaldo() const {
	return formatearConComas(this->saldo);
}

/// <summary>
/// Metodo para formatear un valor en centavos a un string con comas y dos decimales
/// </summary>
/// <param name="saldo"></param>
/// <returns></returns>
std::string CuentaAhorros::formatearConComas(double saldo) const {
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
void CuentaAhorros::mostrarInformacion(const std::string& cedula = "", bool limpiarPantalla = true) const {
	// Validacion basica
	if (this == nullptr) {
		return;
	}
	// Limpieza de pantalla solo si se solicita
	if (limpiarPantalla) { // Limpiar pantalla si se solicita
		std::system("cls");
	}
	// Titulo con formato especifico para CuentaAhorros
	std::cout << "\n" << std::string(50, '=') << std::endl;
	std::cout << "          INFORMACION DE CUENTA DE AHORROS" << std::endl;
	std::cout << std::string(50, '=') << "\n" << std::endl;
	// Informacion del titular si esta disponible
	if (!cedula.empty()) {
		std::cout << "Cedula del titular: " << cedula << std::endl;
		std::cout << std::string(30, '-') << std::endl;
	}

	std::cout << "Tipo de cuenta: AHORROS" << std::endl;
	std::cout << "Numero de cuenta: " << this->numeroCuenta << std::endl;

	// Imprimir usando el metodo de la clase Fecha
	std::cout << "Fecha de apertura: "
		<< this->fechaApertura.obtenerFechaFormateada()
		<< std::endl;

	// Estado de la cuenta por defecto es "ACTIVA"
	const_cast<std::string&>(this->estadoCuenta) = consultarEstado();
	std::cout << "Estado de la cuenta: " << this->estadoCuenta << std::endl;
	std::cout << "Saldo actual: $" << formatearConComas(this->saldo) << std::endl;
	std::cout << "Tasa de interes: " << this->tasaInteres << "%" << std::endl;

	// Pie de pagina
	std::cout << "\n" << std::string(50, '-') << std::endl;
	std::cout << "Presione cualquier tecla para continuar..." << std::endl;
	int tecla = _getch();
	(void)tecla;
}

/// <summary>
/// Metodo para guardar la cuenta de ahorros en un archivo binario
/// </summary>
/// <param name="nombreArchivo"></param>
void CuentaAhorros::guardarEnArchivo(const std::string& nombreArchivo) const {
	std::ofstream archivo(nombreArchivo, std::ios::binary);
	if (archivo.is_open()) {
		archivo.write(reinterpret_cast<const char*>(&this->numeroCuenta), sizeof(this->numeroCuenta));
		archivo.write(reinterpret_cast<const char*>(&this->saldo), sizeof(this->saldo));

		// Convertir Fecha a string y escribir
		std::string fechaStr = this->fechaApertura.obtenerFechaFormateada();
		archivo.write(fechaStr.c_str(), fechaStr.size() + 1);

		archivo.write(this->estadoCuenta.c_str(), this->estadoCuenta.size() + 1);
		archivo.write(reinterpret_cast<const char*>(&tasaInteres), sizeof(tasaInteres));
		archivo.close();
	}
}

/// <summary>
/// Metodo para cargar la cuenta de ahorros desde un archivo binario
/// </summary>
/// <param name="nombreArchivo"></param>
void CuentaAhorros::cargarDesdeArchivo(const std::string& nombreArchivo) {
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

		archivo.read(reinterpret_cast<char*>(&tasaInteres), sizeof(tasaInteres));
		archivo.close();
	}
}

/// <summary>
/// Metodo para calcular el interes de la cuenta de ahorros
/// </summary>
/// <returns></returns>
int CuentaAhorros::calcularInteres() const {
	// Validamos que la tasa de interes sea positiva
	if (this->tasaInteres < 0.0) {
		std::cout << "La tasa de interes no puede ser negativa." << std::endl;
		return 0;
	}
	// Calculamos el interes simple
	double interes = (this->saldo * this->tasaInteres) / 100;
	return static_cast<int>(interes);
}
#pragma endregion