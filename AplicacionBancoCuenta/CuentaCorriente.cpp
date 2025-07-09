/// @file CuentaCorriente.cpp
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
#include "Utilidades.h"

using namespace std;

#pragma region METODOS ESPECIFICOS DE LA CLASE
/**
 * @brief Método para depositar una cantidad en la cuenta corriente
 *
 * Verifica que el monto a depositar sea positivo antes de añadirlo al saldo
 *
 * @param cantidad Monto a depositar en la cuenta
 */
void CuentaCorriente::depositar(double cantidad) {
	if (cantidad > 0) {
		this->saldo += cantidad;
	}
	else {
		std::cout << "La cantidad a depositar debe ser positiva." << std::endl;
	}
}

/**
 * @brief Método para retirar una cantidad de la cuenta corriente
 *
 * Verifica que haya fondos suficientes antes de realizar el retiro
 *
 * @param cantidad Monto a retirar de la cuenta
 */
void CuentaCorriente::retirar(double cantidad) {
	if (cantidad <= this->saldo) {
		this->saldo -= cantidad;
	}
	else {
		std::cout << "Fondos insuficientes." << std::endl;
	}
}

/**
 * @brief Método para consultar el saldo actual de la cuenta corriente
 *
 * @return double Retorna el saldo actual de la cuenta
 */
double CuentaCorriente::consultarSaldo() const {
	return this->saldo;
}

/**
 * @brief Método para consultar el estado de la cuenta corriente
 *
 * @return std::string Retorna "ACTIVA" si no hay un estado explícito, o el estado actual de la cuenta
 */
std::string CuentaCorriente::consultarEstado() const {
	return this->estadoCuenta.empty() ? "ACTIVA" : this->estadoCuenta;
}

/**
 * @brief Método para formatear el saldo de la cuenta con formato de moneda
 *
 * @return std::string Saldo formateado con comas y dos decimales
 */
std::string CuentaCorriente::formatearSaldo() const {
	return formatearConComas(this->saldo);
}

/**
 * @brief Método para formatear un valor en centavos a un string con comas y dos decimales
 *
 * @param saldo Valor en centavos a formatear
 * @return std::string Valor formateado como cadena con formato de moneda
 */
std::string CuentaCorriente::formatearConComas(double saldo) const {
	// Convertir de centavos a valor decimal
	double valorReal = saldo / 100.0;
	std::ostringstream oss;
	oss.imbue(std::locale("en_US.UTF-8")); // Usar formato americano: 1,234.56
	oss << std::fixed << std::setprecision(2) << valorReal;
	return oss.str();
}

/**
 * @brief Muestra la información completa de la cuenta corriente
 *
 * Presenta en consola los detalles de la cuenta incluyendo número, fecha de apertura,
 * estado y saldo actual
 *
 * @param cedula Cédula del titular (opcional)
 * @param limpiarPantalla Indica si debe limpiarse la pantalla antes de mostrar información (por defecto true)
 */
void CuentaCorriente::mostrarInformacion(const std::string& cedula = "", bool limpiarPantalla = true) const {

	
	// Validacion basica
	if (this == nullptr) {
		return;
	}

	// Limpieza de pantalla solo si se solicita
	if (limpiarPantalla) {
		Utilidades::limpiarPantallaPreservandoMarquesina(3);
	}

	// Titulo con formato especifico para CuentaCorriente
	std::cout << "          INFORMACION DE CUENTA CORRIENTE" << std::endl;
	std::cout << std::string(50, '=') << "\n" << std::endl;

	// Informacion del titular si esta disponible
	if (!cedula.empty()) {
		std::cout << "Cedula del titular: " << cedula << std::endl;
		std::cout << std::string(30, '-') << std::endl;
	}

	// Informacion comun
	std::cout << "Tipo de cuenta: CORRIENTE" << std::endl;
	std::cout << "Numero de cuenta: " << this->numeroCuenta << std::endl;
	std::cout << "Fecha de apertura: "
		<< this->fechaApertura.obtenerFechaFormateada()
		<< std::endl;
	const_cast<std::string&>(this->estadoCuenta) = consultarEstado();
	std::cout << "Estado: " << this->estadoCuenta << std::endl;
	std::cout << "Saldo actual: $" << formatearConComas(this->saldo) << std::endl;

	// Pie de pagina
	std::cout << "\n" << std::string(50, '-') << std::endl;

}

/**
 * @brief Guarda la información de la cuenta corriente en un archivo binario
 *
 * @param nombreArchivo Ruta del archivo donde se guardará la información
 */
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

/**
 * @brief Carga la información de la cuenta corriente desde un archivo binario
 *
 * @param nombreArchivo Ruta del archivo desde donde se cargará la información
 */
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

/**
 * @brief Verifica si el monto mínimo es válido y lo asigna
 *
 * Comprueba que el monto sea al menos $250.00 antes de asignarlo
 *
 * @param monto Cantidad a establecer como monto mínimo
 */
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