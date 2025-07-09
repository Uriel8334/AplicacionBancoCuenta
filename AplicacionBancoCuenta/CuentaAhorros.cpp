/// @file CuentaAhorros.cpp

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <conio.h>
#include <limits>
#include "Cuenta.h"
#include "Validar.h"
#include "Cifrado.h"
#include "Fecha.h"
#include "CuentaAhorros.h" 
#include "Utilidades.h"

using namespace std;

#pragma region METODOS ESPECIFICOS DE LA CLASE 
/**
 * @brief Método para depositar una cantidad en la cuenta de ahorros
 *
 * Valida que la cantidad sea positiva y que el saldo resultante no exceda
 * el límite máximo establecido de $15,000.00
 *
 * @param cantidad Monto a depositar en la cuenta
 */
void CuentaAhorros::depositar(double cantidad) {

	if (cantidad <= 0) {
		std::cout << "El monto debe ser mayor a cero.\n";
		return;
	}

	// Convertir a un tipo más amplio para evitar desbordamiento  
	long double nuevoSaldo = static_cast<long double>(saldo) + static_cast<long double>(cantidad);

	// Verificar si el nuevo saldo excede el límite de la cuenta (15000.00 dólares)
	constexpr double LIMITE_MAXIMO = 15000.00;
	if (nuevoSaldo > LIMITE_MAXIMO) {
		std::cout << "Error: El saldo no puede exceder el límite de $15,000.00\n";
		return;
	}

	saldo = static_cast<double>(nuevoSaldo);
	std::cout << "Depósito realizado con éxito. Nuevo saldo: $" << formatearSaldo() << std::endl;
}

/**
 * @brief Método para retirar una cantidad de la cuenta de ahorros
 *
 * Verifica que la cuenta tenga fondos suficientes para realizar el retiro
 *
 * @param cantidad Monto a retirar de la cuenta
 */
void CuentaAhorros::retirar(double cantidad) {
	if (cantidad <= this->saldo) {
		this->saldo -= cantidad;
	}
	else {
		std::cout << "Fondos insuficientes." << std::endl;
	}
}

/**
 * @brief Método para consultar el saldo actual de la cuenta de ahorros
 *
 * @return double Retorna el saldo actual de la cuenta
 */
double CuentaAhorros::consultarSaldo() const {
	return this->saldo;
}

/**
 * @brief Método para consultar el estado de la cuenta de ahorros
 *
 * @return std::string Retorna "ACTIVA" si no hay un estado explícito, o el estado actual de la cuenta
 */
std::string CuentaAhorros::consultarEstado() const {
	return this->estadoCuenta.empty() ? "ACTIVA" : this->estadoCuenta;
}

/**
 * @brief Método para formatear el saldo de la cuenta con formato de moneda
 *
 * @return std::string Saldo formateado con comas y dos decimales
 */
std::string CuentaAhorros::formatearSaldo() const {
	return formatearConComas(this->saldo);
}

/**
 * @brief Método para formatear un valor en centavos a un string con comas y dos decimales
 *
 * @param saldo Valor en centavos a formatear
 * @return std::string Valor formateado como cadena con formato de moneda
 */
std::string CuentaAhorros::formatearConComas(double saldo) const {
	// Convertir de centavos a valor decimal
	double valorReal = saldo / 100.0;
	std::ostringstream oss;
	oss.imbue(std::locale("en_US.UTF-8")); // Usar formato americano: 1,234.56
	oss << std::fixed << std::setprecision(2) << valorReal;
	return oss.str();
}

/**
 * @brief Muestra la información completa de la cuenta de ahorros
 *
 * Presenta en consola los detalles de la cuenta incluyendo número, fecha de apertura,
 * estado, saldo actual y tasa de interés
 *
 * @param cedula Cédula del titular (opcional)
 * @param limpiarPantalla Indica si debe limpiarse la pantalla antes de mostrar información (por defecto true)
 */
void CuentaAhorros::mostrarInformacion(const std::string& cedula = "", bool limpiarPantalla = true) const {
	// Validacion basica
	if (this == nullptr) {
		return;
	}
	// Limpieza de pantalla solo si se solicita
	if (limpiarPantalla) { // Limpiar pantalla si se solicita
		Utilidades::limpiarPantallaPreservandoMarquesina();
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

/**
 * @brief Guarda la información de la cuenta en un archivo binario
 *
 * @param nombreArchivo Ruta del archivo donde se guardará la información
 */
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

/**
 * @brief Carga la información de la cuenta desde un archivo binario
 *
 * @param nombreArchivo Ruta del archivo desde donde se cargará la información
 */
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

/**
 * @brief Calcula el interés generado por la cuenta según su tasa actual
 *
 * @return int Valor del interés calculado basado en el saldo actual y la tasa de interés
 */
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